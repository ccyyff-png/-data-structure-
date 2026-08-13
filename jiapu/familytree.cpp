#include "familytree.h"

#include <QFile>
#include <QTextStream>
#include <QStringDecoder>
#include <QSet>
#include <QQueue>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <functional>

// ---------------- 内部工具 ----------------

// 记录三元组比较（char[] 存 UTF-8 字节，统一经 QString::fromUtf8 比较）
static bool recordEquals(const FamType& r, const QString& f, const QString& w, const QString& s)
{
    return QString::fromUtf8(r.father) == f
        && QString::fromUtf8(r.wife)   == w
        && QString::fromUtf8(r.son)    == s;
}

// 由 records 重建派生状态：成员索引、代际、最长支系、二叉树
static void RebuildDerived(FamData* d, QStringList* warnings = nullptr)
{
    QSet<QString> fatherSet, wifeSet, sonSet;
    for (const FamType& r : d->records) {
        fatherSet.insert(QString::fromUtf8(r.father));
        wifeSet.insert(QString::fromUtf8(r.wife));
        sonSet.insert(QString::fromUtf8(r.son));
    }
    if (warnings) {
        const QSet<QString> conflicts = fatherSet & wifeSet;
        for (const QString& n : conflicts)
            *warnings << QString("姓名「%1」同时出现在父亲与母亲列，按男性处理").arg(n);
    }

    d->members.clear();
    for (const FamType& r : d->records) {
        const QString f = QString::fromUtf8(r.father);
        const QString w = QString::fromUtf8(r.wife);
        const QString s = QString::fromUtf8(r.son);

        MemberInfo& fm = d->members[f];
        fm.name = f;
        fm.male = true;
        fm.spouse = w;
        if (!fm.children.contains(s))
            fm.children.append(s);

        MemberInfo& wm = d->members[w];
        wm.name = w;
        wm.male = false;
        wm.spouse = f;

        MemberInfo& sm = d->members[s];
        sm.name = s;
        sm.male = !wifeSet.contains(s);   // 出现在母亲列 = 女，否则男
        sm.father = f;
        sm.mother = w;
    }

    // 配偶的公婆信息 = 丈夫的父母
    for (auto& [name, m] : d->members) {
        if (!m.male && !m.spouse.isEmpty()) {
            auto h = d->members.find(m.spouse);
            if (h != d->members.end()) {
                m.father = h->second.father;
                m.mother = h->second.mother;
            }
        }
    }

    // 始祖 = 未出现在儿子列的父亲
    QStringList roots;
    for (const FamType& r : d->records) {
        const QString f = QString::fromUtf8(r.father);
        if (!sonSet.contains(f) && !roots.contains(f))
            roots.append(f);
    }
    d->rootName = roots.isEmpty() ? QString::fromUtf8(d->records.front().father) : roots.first();
    if (warnings && roots.size() > 1)
        *warnings << "检测到多个根节点，以第一条记录的父亲为始祖";

    // 代际：自始祖 BFS（配偶与丈夫同代）
    QSet<QString> visited;
    QQueue<QString> q;
    q.enqueue(d->rootName);
    visited.insert(d->rootName);
    d->members[d->rootName].generation = 0;
    while (!q.isEmpty()) {
        const QString cur = q.dequeue();
        auto it = d->members.find(cur);
        if (it == d->members.end())
            continue;
        for (const QString& c : it->second.children) {
            if (!visited.contains(c)) {
                visited.insert(c);
                d->members[c].generation = it->second.generation + 1;
                q.enqueue(c);
            }
        }
        if (!it->second.spouse.isEmpty() && !visited.contains(it->second.spouse)) {
            visited.insert(it->second.spouse);
            d->members[it->second.spouse].generation = it->second.generation;
        }
    }
    int unreached = 0;
    for (auto& [name, m] : d->members) {
        if (!visited.contains(name)) {
            m.generation = 0;
            ++unreached;
        }
    }
    if (warnings && unreached > 0)
        *warnings << QString("有 %1 名成员无法从始祖到达（循环或孤立记录），已按第 1 代处理").arg(unreached);

    d->maxGeneration = 0;
    for (const auto& [name, m] : d->members)
        d->maxGeneration = std::max(d->maxGeneration, m.generation);
    for (auto& [name, m] : d->members)
        m.isRoot = (name == d->rootName);

    // 重建二叉树（课设展示）
    FreeTree(d->tree);
    d->tree = CreatBTree(d->rootName.toUtf8().constData(),
                         d->records.data(), static_cast<int>(d->records.size()));
}

// ---------------- IO ----------------

FamData* LoadFamilyData(const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMsg) *errorMsg = "无法打开数据文件：" + filePath;
        return nullptr;
    }
    const QByteArray bytes = file.readAll();
    file.close();
    if (bytes.trimmed().isEmpty()) {
        if (errorMsg) *errorMsg = "数据文件为空";
        return nullptr;
    }

    // 编码：UTF-8 优先，失败回退系统编码（旧版 GBK 数据），并自动迁移为 UTF-8
    QString text;
    bool gbkFallback = false;
    {
        QStringDecoder utf8Dec(QStringConverter::Utf8, QStringConverter::Flag::Stateless);
        text = utf8Dec.decode(bytes);
        if (utf8Dec.hasError()) {
            QStringDecoder sysDec(QStringConverter::System, QStringConverter::Flag::Stateless);
            text = sysDec.decode(bytes);
            if (sysDec.hasError()) {
                if (errorMsg) *errorMsg = "数据文件编码无法识别（需要 UTF-8 或 GBK）";
                return nullptr;
            }
            gbkFallback = true;
        }
    }

    FamData* d = new FamData;
    QStringList warnings;

    // 解析与校验
    QMap<QString, QString> fatherWife;   // 父亲 -> 配偶（校验一夫一妻）
    QMap<QString, QString> sonFather;    // 儿子 -> 父亲（校验一子一父）
    QMap<QString, QString> wifeHusband;  // 妻子 -> 丈夫（校验一妻一夫）
    int lineNo = 0;
    const QStringList lines = text.split('\n');
    for (const QString& rawLine : lines) {
        ++lineNo;
        const QString line = rawLine.trimmed();
        if (line.isEmpty())
            continue;
        const QStringList parts = line.split(',');
        if (parts.size() != 3) {
            warnings << QString("第 %1 行格式错误，已跳过").arg(lineNo);
            continue;
        }
        const QString f = parts[0].trimmed();
        const QString w = parts[1].trimmed();
        const QString s = parts[2].trimmed();
        if (f.isEmpty() || w.isEmpty() || s.isEmpty()) {
            warnings << QString("第 %1 行存在空字段，已跳过").arg(lineNo);
            continue;
        }
        if (fatherWife.contains(f) && fatherWife[f] != w) {
            warnings << QString("父亲「%1」存在多个不同配偶的记录，已忽略该行").arg(f);
            continue;
        }
        if (sonFather.contains(s) && sonFather[s] != f) {
            warnings << QString("成员「%1」存在多个父亲，已忽略该行").arg(s);
            continue;
        }
        if (wifeHusband.contains(w) && wifeHusband[w] != f) {
            warnings << QString("成员「%1」存在多个丈夫，已忽略该行").arg(w);
            continue;
        }
        const bool duplicate = std::any_of(d->records.begin(), d->records.end(),
                                           [&](const FamType& r) { return recordEquals(r, f, w, s); });
        if (duplicate) {
            warnings << QString("重复记录已忽略（%1,%2,%3）").arg(f, w, s);
            continue;
        }
        if (d->records.size() >= 1000) {
            warnings << "记录数超过上限 1000，后续行已忽略";
            break;
        }
        fatherWife[f] = w;
        sonFather[s] = f;
        wifeHusband[w] = f;
        FamType rec{};
        qstrncpy(rec.father, f.toUtf8().constData(), MaxSize);
        qstrncpy(rec.wife,   w.toUtf8().constData(), MaxSize);
        qstrncpy(rec.son,    s.toUtf8().constData(), MaxSize);
        d->records.push_back(rec);
    }

    if (d->records.empty()) {
        if (errorMsg) *errorMsg = "没有可用的家谱记录";
        delete d;
        return nullptr;
    }

    RebuildDerived(d, &warnings);

    if (gbkFallback) {
        warnings << "检测到旧版 GBK 编码数据，已自动迁移为 UTF-8";
        SaveFamilyData(filePath, *d);
    }

    if (errorMsg)
        *errorMsg = warnings.join('\n');
    return d;
}

bool SaveFamilyData(const QString& filePath, const FamData& data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream out(&file);  // Qt6 默认 UTF-8
    for (const FamType& r : data.records)
        out << QString::fromUtf8(r.father) << ','
            << QString::fromUtf8(r.wife)   << ','
            << QString::fromUtf8(r.son)    << '\n';
    file.close();
    return true;
}

void FreeFamData(FamData* data)
{
    if (!data)
        return;
    FreeTree(data->tree);
    delete data;
}

// ---------------- BTree（课设展示） ----------------

BTree* CreatBTree(const char* root, const FamType* fam, int n)
{
    if (!root || !*root)
        return nullptr;

    BTree* bt = static_cast<BTree*>(malloc(sizeof(BTree)));
    qstrncpy(bt->name, root, MaxSize);
    bt->lchild = bt->rchild = nullptr;

    // 配偶节点：father == root 的首条带母亲记录
    for (int i = 0; i < n; ++i) {
        if (strcmp(fam[i].father, root) == 0 && fam[i].wife[0] != '\0') {
            BTree* wife = static_cast<BTree*>(malloc(sizeof(BTree)));
            qstrncpy(wife->name, fam[i].wife, MaxSize);
            wife->lchild = wife->rchild = nullptr;
            bt->lchild = wife;
            break;
        }
    }

    // 儿子链：father == root 的全部记录，依次挂在（配偶节点 / 自身）rchild 上
    BTree* tail = bt->lchild ? bt->lchild : bt;
    for (int i = 0; i < n; ++i) {
        if (strcmp(fam[i].father, root) == 0 && fam[i].son[0] != '\0') {
            BTree* son = CreatBTree(fam[i].son, fam, n);
            tail->rchild = son;
            tail = son;
        }
    }
    return bt;
}

void FreeTree(BTree* bt)
{
    if (!bt)
        return;
    FreeTree(bt->lchild);
    FreeTree(bt->rchild);
    free(bt);
}

QString DispTreeInBracketFormat(const BTree* bt)
{
    QString result;
    std::function<void(const BTree*)> helper = [&](const BTree* b) {
        if (!b)
            return;
        result += QString::fromUtf8(b->name);
        if (b->lchild || b->rchild) {
            result += '(';
            helper(b->lchild);
            if (b->lchild && b->rchild)
                result += ',';
            helper(b->rchild);
            result += ')';
        }
    };
    helper(bt);
    return result;
}

QString DisplayFamilyTreeText(const BTree* bt)
{
    if (!bt)
        return QString();

    QString result;
    std::function<void(const BTree*, const QString&)> helper = [&](const BTree* node, const QString& prefix) {
        // 子女 = 配偶节点的 rchild 链（无配偶时退化为自身 rchild）
        BTree* child = node->lchild ? node->lchild->rchild : node->rchild;
        while (child) {
            const bool last = (child->rchild == nullptr);
            result += prefix + (last ? QStringLiteral("└─ ") : QStringLiteral("├─ "))
                   + QString::fromUtf8(child->name);
            if (child->lchild)
                result += QStringLiteral("（妻：%1）").arg(QString::fromUtf8(child->lchild->name));
            result += '\n';
            helper(child, prefix + (last ? QStringLiteral("   ") : QStringLiteral("│  ")));
            child = child->rchild;
        }
    };

    result += QString::fromUtf8(bt->name);
    if (bt->lchild)
        result += QStringLiteral("（妻：%1）").arg(QString::fromUtf8(bt->lchild->name));
    result += '\n';
    helper(bt, QString());
    return result;
}

QString RecordsToText(const FamData& data)
{
    QString result = QStringLiteral("父亲   母亲   儿子\n------------------------------\n");
    for (const FamType& r : data.records)
        result += QStringLiteral("%1,%2,%3\n").arg(QString::fromUtf8(r.father),
                                                   QString::fromUtf8(r.wife),
                                                   QString::fromUtf8(r.son));
    result += QStringLiteral("------------------------------");
    return result;
}

// ---------------- 查询 ----------------

const MemberInfo* FindMember(const FamData& d, const QString& name)
{
    auto it = d.members.find(name);
    return it == d.members.end() ? nullptr : &it->second;
}

QStringList FindAllAncestors(const FamData& d, const QString& name)
{
    auto it = d.members.find(name);
    if (it == d.members.end())
        return {};
    QStringList result;
    QString cur = it->second.father;
    int guard = 0;
    while (!cur.isEmpty() && guard++ < static_cast<int>(d.members.size())) {
        auto p = d.members.find(cur);
        if (p == d.members.end())
            break;
        result << p->second.name;
        if (!p->second.spouse.isEmpty())
            result << p->second.spouse;
        cur = p->second.father;
    }
    return result;
}

QStringList FindAllChildren(const FamData& d, const QString& name)
{
    const MemberInfo* m = FindMember(d, name);
    if (!m)
        return {};
    if (m->male)
        return m->children;
    if (m->spouse.isEmpty())
        return {};
    auto h = d.members.find(m->spouse);
    return h == d.members.end() ? QStringList() : h->second.children;
}

QStringList FuzzySearch(const FamData& d, const QString& keyword)
{
    if (keyword.isEmpty())
        return {};
    QVector<QPair<int, QString>> hits;
    for (const auto& [name, m] : d.members) {
        if (name.contains(keyword))
            hits.append({m.generation, name});
    }
    std::sort(hits.begin(), hits.end(), [](const QPair<int, QString>& a, const QPair<int, QString>& b) {
        return a.first != b.first ? a.first < b.first : a.second < b.second;
    });
    QStringList result;
    for (const auto& h : hits)
        result << h.second;
    return result;
}

// ---------------- 修改 ----------------

QString AddMember(FamData* d, const QString& father, const QString& wife, const QString& son)
{
    const QString f = father.trimmed();
    const QString w = wife.trimmed();
    const QString s = son.trimmed();
    if (f.isEmpty() || w.isEmpty() || s.isEmpty())
        return "父亲、母亲、儿子均不能为空";
    for (const QString& x : {f, w, s}) {
        if (x.contains(',') || x.contains('\n') || x.contains('\r'))
            return "姓名不能包含逗号或换行";
    }
    if (d->members.find(f) == d->members.end())
        return QString("父亲「%1」不存在于家谱中，无法添加").arg(f);
    if (d->members.find(s) != d->members.end())
        return QString("成员「%1」已存在").arg(s);

    const MemberInfo& fi = d->members[f];
    if (!fi.spouse.isEmpty() && fi.spouse != w)
        return QString("父亲「%1」已有配偶「%2」，不能添加不同母亲").arg(f, fi.spouse);
    if (d->members.find(w) != d->members.end() && fi.spouse != w)
        return QString("姓名「%1」已被其他成员占用").arg(w);

    FamType rec{};
    qstrncpy(rec.father, f.toUtf8().constData(), MaxSize);
    qstrncpy(rec.wife,   w.toUtf8().constData(), MaxSize);
    qstrncpy(rec.son,    s.toUtf8().constData(), MaxSize);
    d->records.push_back(rec);
    RebuildDerived(d);
    return {};
}

QString DeleteMember(FamData* d, const QString& name)
{
    auto it = d->members.find(name);
    if (it == d->members.end())
        return QString("成员「%1」不存在").arg(name);
    if (it->second.isRoot)
        return "不能删除家谱始祖";
    if (!it->second.male)
        return QString("配偶不能单独删除，请删除其丈夫「%1」").arg(it->second.spouse);

    // 收集本人及全部血亲后代
    QSet<QString> doomed{name};
    QQueue<QString> q;
    q.enqueue(name);
    while (!q.isEmpty()) {
        const QString cur = q.dequeue();
        auto m = d->members.find(cur);
        if (m == d->members.end())
            continue;
        for (const QString& c : m->second.children) {
            if (!doomed.contains(c)) {
                doomed.insert(c);
                q.enqueue(c);
            }
        }
    }

    auto& rs = d->records;
    rs.erase(std::remove_if(rs.begin(), rs.end(), [&](const FamType& r) {
        return doomed.contains(QString::fromUtf8(r.father))
            || doomed.contains(QString::fromUtf8(r.wife))
            || doomed.contains(QString::fromUtf8(r.son));
    }), rs.end());
    RebuildDerived(d);
    return QString("已删除「%1」及其后代共 %2 人").arg(name).arg(doomed.size());
}

QString RenameMember(FamData* d, const QString& oldName, const QString& newName)
{
    if (d->members.find(oldName) == d->members.end())
        return QString("成员「%1」不存在").arg(oldName);
    const QString nn = newName.trimmed();
    if (nn.isEmpty())
        return "新姓名不能为空";
    if (nn.contains(',') || nn.contains('\n') || nn.contains('\r'))
        return "姓名不能包含逗号或换行";
    if (nn == oldName)
        return {};
    if (d->members.find(nn) != d->members.end())
        return QString("姓名「%1」已被占用").arg(nn);

    for (FamType& r : d->records) {
        if (QString::fromUtf8(r.father) == oldName)
            qstrncpy(r.father, nn.toUtf8().constData(), MaxSize);
        if (QString::fromUtf8(r.wife) == oldName)
            qstrncpy(r.wife, nn.toUtf8().constData(), MaxSize);
        if (QString::fromUtf8(r.son) == oldName)
            qstrncpy(r.son, nn.toUtf8().constData(), MaxSize);
    }
    RebuildDerived(d);
    return {};
}

int CountDescendants(const FamData& d, const QString& name)
{
    auto it = d.members.find(name);
    if (it == d.members.end() || !it->second.male)
        return 0;
    QSet<QString> descendants;
    QQueue<QString> q;
    q.enqueue(name);
    while (!q.isEmpty()) {
        const QString cur = q.dequeue();
        auto m = d.members.find(cur);
        if (m == d.members.end())
            continue;
        for (const QString& c : m->second.children) {
            if (!descendants.contains(c)) {
                descendants.insert(c);
                q.enqueue(c);
            }
        }
    }
    return static_cast<int>(descendants.size());
}

// ---------------- 统计 ----------------

StatsSummary ComputeStats(const FamData& d)
{
    StatsSummary s;
    s.totalMembers = static_cast<int>(d.members.size());
    s.generationCount = d.maxGeneration + 1;
    s.longestBranch = LongestBranchPath(d).size();
    for (const auto& [name, m] : d.members) {
        if (m.male && static_cast<int>(m.children.size()) > s.maxChildren) {
            s.maxChildren = m.children.size();
            s.busiestName = name;
        }
    }
    return s;
}

QVector<int> CountPerGeneration(const FamData& d)
{
    QVector<int> counts(d.maxGeneration + 1, 0);
    for (const auto& [name, m] : d.members)
        counts[m.generation]++;
    return counts;
}

QMap<int, int> ChildrenCountDistribution(const FamData& d)
{
    QMap<int, int> dist;
    for (const auto& [name, m] : d.members) {
        if (m.male)
            dist[m.children.size()]++;
    }
    return dist;
}

QVector<QPair<QString, int>> BranchSizes(const FamData& d, int topK)
{
    auto rootIt = d.members.find(d.rootName);
    if (rootIt == d.members.end())
        return {};
    QStringList branchRoots = rootIt->second.children;
    if (branchRoots.size() == 1)  // 始祖仅一子时，支系根取其子女
        branchRoots = d.members.at(branchRoots.first()).children;

    QVector<QPair<QString, int>> sizes;
    for (const QString& br : branchRoots) {
        QSet<QString> blood{br};
        QQueue<QString> q;
        q.enqueue(br);
        while (!q.isEmpty()) {
            const QString cur = q.dequeue();
            auto m = d.members.find(cur);
            if (m == d.members.end())
                continue;
            for (const QString& c : m->second.children) {
                if (!blood.contains(c)) {
                    blood.insert(c);
                    q.enqueue(c);
                }
            }
        }
        // 支系人数 = 血亲 + 嫁入的配偶
        int size = static_cast<int>(blood.size());
        for (const QString& n : blood) {
            auto m = d.members.find(n);
            if (m != d.members.end() && !m->second.spouse.isEmpty())
                ++size;
        }
        sizes.append({br, size});
    }
    std::sort(sizes.begin(), sizes.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
        return a.second != b.second ? a.second > b.second : a.first < b.first;
    });
    if (topK > 0 && sizes.size() > topK)
        sizes.resize(topK);
    return sizes;
}

QStringList LongestBranchPath(const FamData& d)
{
    if (d.members.empty())
        return {};
    std::function<QStringList(const QString&)> dfs = [&](const QString& n) -> QStringList {
        const auto it = d.members.find(n);
        if (it == d.members.end())
            return {n};
        if (it->second.children.isEmpty())
            return {n};
        QStringList best;
        for (const QString& c : it->second.children) {
            const QStringList p = dfs(c);
            if (p.size() > best.size())
                best = p;
        }
        best.prepend(n);
        return best;
    };
    return dfs(d.rootName);
}

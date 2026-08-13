#include <QtTest>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QStringEncoder>
#include <QStringDecoder>
#include "familytree.h"

// 数据层单元测试
// 预期数值经独立脚本（Perl）复核：29 条记录、54 名成员（30 男 / 24 女）、11 代、
// 每代人数 [2,2,8,12,11,8,4,2,2,2,1]、最长支系 11 人、子女分布 {0:6,1:21,2:2,4:1}
class TestDataLayer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // 加载与统计
    void loadStats();
    void perGeneration();
    void childrenDistribution();
    void branchSizes();
    void longestBranchPath();

    // 查询
    void ancestors();
    void children();        // 回归：无子男性误报兄弟（旧 bug 5）
    void fuzzySearch();
    void memberDetail();

    // 修改
    void addMemberValidations();   // 回归：幽灵记录/逗号/重复等（旧 bug 9）
    void addMemberSuccess();
    void deleteMemberRejects();
    void deleteMemberCascade();
    void renameMember();

    // 二叉树课设函数
    void bracketFormat();
    void textTree();

    // 编码回退（旧 GBK 数据自动迁移）
    void gbkFallback();

private:
    void reset();                    // 用原始数据重置临时文件并重新加载
    FamData* data = nullptr;
    QTemporaryDir dir;
    QString filePath;
    QByteArray originalData;
};

void TestDataLayer::initTestCase()
{
    QVERIFY2(dir.isValid(), "无法创建临时目录");
    filePath = dir.filePath("familytree.txt");

    QFile src(QStringLiteral(SOURCE_DIR) + "/familytreedata.txt");
    QVERIFY2(src.open(QIODevice::ReadOnly), "无法打开源数据文件");
    originalData = src.readAll();
    src.close();
    QVERIFY(originalData.size() > 0);
    reset();
}

void TestDataLayer::cleanupTestCase()
{
    FreeFamData(data);
    data = nullptr;
}

void TestDataLayer::reset()
{
    FreeFamData(data);
    data = nullptr;
    QFile f(filePath);
    QVERIFY2(f.open(QIODevice::WriteOnly | QIODevice::Truncate), "无法写入临时数据文件");
    f.write(originalData);
    f.close();
    QString msg;
    data = LoadFamilyData(filePath, &msg);
    QVERIFY2(data != nullptr, qPrintable("加载失败: " + msg));
    QVERIFY2(msg.isEmpty(), qPrintable("加载出现意外警告: " + msg));
}

void TestDataLayer::loadStats()
{
    QCOMPARE(static_cast<int>(data->members.size()), 54);
    QCOMPARE(data->rootName, QString("陈鼎元"));
    QCOMPARE(data->maxGeneration + 1, 11);

    int males = 0, females = 0;
    for (const auto& [name, m] : data->members)
        m.male ? ++males : ++females;
    QCOMPARE(males, 30);
    QCOMPARE(females, 24);

    const StatsSummary s = ComputeStats(*data);
    QCOMPARE(s.totalMembers, 54);
    QCOMPARE(s.generationCount, 11);
    QCOMPARE(s.longestBranch, 11);
    QCOMPARE(s.maxChildren, 4);
    QCOMPARE(s.busiestName, QString("陈建国"));

    // 始祖与性别推断
    const MemberInfo* root = FindMember(*data, "陈鼎元");
    QVERIFY(root && root->isRoot && root->male && root->father.isEmpty());
    const MemberInfo* wife = FindMember(*data, "奚秀兰");
    QVERIFY(wife && !wife->male && wife->spouse == "陈鼎元");
    const MemberInfo* leaf = FindMember(*data, "陈清晨");
    QVERIFY(leaf && leaf->male && leaf->children.isEmpty() && leaf->spouse.isEmpty());
    QCOMPARE(leaf->generation, 10);
}

void TestDataLayer::perGeneration()
{
    const QVector<int> counts = CountPerGeneration(*data);
    const QVector<int> expected = {2, 2, 8, 12, 11, 8, 4, 2, 2, 2, 1};
    QCOMPARE(counts, expected);
}

void TestDataLayer::childrenDistribution()
{
    const QMap<int, int> dist = ChildrenCountDistribution(*data);
    QCOMPARE(dist.value(0), 6);
    QCOMPARE(dist.value(1), 21);
    QCOMPARE(dist.value(2), 2);
    QCOMPARE(dist.value(4), 1);
}

void TestDataLayer::branchSizes()
{
    const auto branches = BranchSizes(*data, 4);
    QCOMPARE(branches.size(), 4);
    QCOMPARE(branches[0], qMakePair(QString("陈永梅"), 24));
    QCOMPARE(branches[1], qMakePair(QString("陈永强"), 12));
    QCOMPARE(branches[2], qMakePair(QString("陈永乐"), 7));   // 并列 7 人，字典序在前
    QCOMPARE(branches[3], qMakePair(QString("陈永刚"), 7));
}

void TestDataLayer::longestBranchPath()
{
    const QStringList path = LongestBranchPath(*data);
    const QStringList expected = {
        "陈鼎元", "陈建国", "陈永梅", "陈志芳", "陈俊文", "陈天宇",
        "陈建华", "陈永亮", "陈志峰", "陈俊浩", "陈清晨"};
    QCOMPARE(path, expected);
}

void TestDataLayer::ancestors()
{
    const QStringList a = FindAllAncestors(*data, "陈宇航");
    const QStringList expected = {
        "陈俊杰", "何婷婷", "陈志远", "林静怡", "陈永刚",
        "刘慧芳", "陈建国", "李秀英", "陈鼎元", "奚秀兰"};
    QCOMPARE(a, expected);

    QVERIFY(FindAllAncestors(*data, "陈鼎元").isEmpty());     // 始祖无祖先
    QVERIFY(FindAllAncestors(*data, "查无此人").isEmpty());   // 不存在
}

void TestDataLayer::children()
{
    // 男性：4 个儿子
    const QStringList c1 = FindAllChildren(*data, "陈建国");
    QCOMPARE(c1, QStringList({"陈永刚", "陈永强", "陈永梅", "陈永乐"}));

    // 女性：返回丈夫的子女
    const QStringList c2 = FindAllChildren(*data, "奚秀兰");
    QCOMPARE(c2, QStringList({"陈建国"}));

    // 回归（旧 bug 5）：无子男性不得把兄弟报成儿子
    QVERIFY(FindAllChildren(*data, "陈宇航").isEmpty());
    QVERIFY(FindAllChildren(*data, "陈清晨").isEmpty());
}

void TestDataLayer::fuzzySearch()
{
    const QStringList jun = FuzzySearch(*data, "俊");
    QCOMPARE(jun.size(), 7);
    const QSet<QString> junSet(jun.begin(), jun.end());
    const QSet<QString> expectedJun = {
        "陈俊杰", "陈俊豪", "陈俊雅", "陈俊鹏", "陈俊熙", "陈俊文", "陈俊浩"};
    QCOMPARE(junSet, expectedJun);

    const QStringList zhi = FuzzySearch(*data, "志");
    QCOMPARE(zhi.size(), 7);

    QVERIFY(FuzzySearch(*data, "不存在关键字").isEmpty());
    QVERIFY(FuzzySearch(*data, "").isEmpty());
}

void TestDataLayer::memberDetail()
{
    const MemberInfo* m = FindMember(*data, "陈永强");
    QVERIFY(m);
    QCOMPARE(m->father, QString("陈建国"));
    QCOMPARE(m->mother, QString("李秀英"));
    QCOMPARE(m->spouse, QString("周雅琴"));
    QCOMPARE(m->children, QStringList({"陈志明", "陈志华"}));
    QCOMPARE(m->generation, 2);

    QVERIFY(!FindMember(*data, "查无此人"));
}

void TestDataLayer::addMemberValidations()
{
    QCOMPARE(AddMember(data, "", "李秀英", "陈永康"), QString("父亲、母亲、儿子均不能为空"));
    QCOMPARE(AddMember(data, "陈建国", "李秀英", ""), QString("父亲、母亲、儿子均不能为空"));
    QCOMPARE(AddMember(data, "陈,建国", "李秀英", "陈永康"), QString("姓名不能包含逗号或换行"));

    // 回归（旧 bug 9）：父亲不存在 → 幽灵记录被拒绝
    const QString ghostErr = AddMember(data, "陈九千", "某氏", "陈永康");
    QVERIFY2(ghostErr.contains("不存在于家谱"), qPrintable(ghostErr));

    // 儿子重名
    QVERIFY(AddMember(data, "陈建国", "李秀英", "陈永刚").contains("已存在"));

    // 母亲不一致
    const QString wifeErr = AddMember(data, "陈建国", "王五", "陈永康");
    QVERIFY2(wifeErr.contains("已有配偶"), qPrintable(wifeErr));

    // 重复记录（儿子已存在即被拦截，等价于拒绝重复记录）
    QCOMPARE(AddMember(data, "陈鼎元", "奚秀兰", "陈建国"), QString("成员「陈建国」已存在"));

    // 全部校验通过后数据不变
    QCOMPARE(static_cast<int>(data->records.size()), 29);
    QCOMPARE(static_cast<int>(data->members.size()), 54);
}

void TestDataLayer::addMemberSuccess()
{
    QVERIFY(AddMember(data, "陈建国", "李秀英", "陈永康").isEmpty());
    QCOMPARE(static_cast<int>(data->members.size()), 55);
    QCOMPARE(static_cast<int>(data->records.size()), 30);
    const MemberInfo* m = FindMember(*data, "陈永康");
    QVERIFY(m && m->father == "陈建国" && m->mother == "李秀英" && m->generation == 2);

    // 落盘并重载，数据持久
    QVERIFY(SaveFamilyData(filePath, *data));
    QString msg;
    FamData* reloaded = LoadFamilyData(filePath, &msg);
    QVERIFY2(reloaded != nullptr, qPrintable(msg));
    QCOMPARE(static_cast<int>(reloaded->members.size()), 55);
    QVERIFY(FindMember(*reloaded, "陈永康") != nullptr);
    FreeFamData(reloaded);
    reset();
}

void TestDataLayer::deleteMemberRejects()
{
    QVERIFY2(DeleteMember(data, "陈鼎元").contains("不能删除家谱始祖"), "应拒删始祖");
    const QString wifeErr = DeleteMember(data, "李秀英");
    QVERIFY2(wifeErr.contains("请删除其丈夫「陈建国」"), qPrintable(wifeErr));
    QVERIFY(DeleteMember(data, "查无此人").contains("不存在"));
    QCOMPARE(static_cast<int>(data->members.size()), 54);  // 数据未变
}

void TestDataLayer::deleteMemberCascade()
{
    QCOMPARE(CountDescendants(*data, "陈志芳"), 7);
    const QString msg = DeleteMember(data, "陈志芳");
    QCOMPARE(msg, QString("已删除「陈志芳」及其后代共 8 人"));

    QCOMPARE(static_cast<int>(data->members.size()), 39);   // 54 - 8 血亲 - 7 配偶
    QCOMPARE(static_cast<int>(data->records.size()), 21);   // 29 - 8 条记录
    QVERIFY(!FindMember(*data, "陈志芳"));
    QVERIFY(!FindMember(*data, "陈清晨"));
    QVERIFY(FindMember(*data, "陈永梅"));                   // 上游不受影响

    // 落盘并重载，删除持久
    QVERIFY(SaveFamilyData(filePath, *data));
    QString msg2;
    FamData* reloaded = LoadFamilyData(filePath, &msg2);
    QVERIFY2(reloaded != nullptr, qPrintable(msg2));
    QCOMPARE(static_cast<int>(reloaded->members.size()), 39);
    FreeFamData(reloaded);
    reset();
}

void TestDataLayer::renameMember()
{
    QVERIFY(RenameMember(data, "陈清晨", "陈晨").isEmpty());
    QVERIFY(FindMember(*data, "陈晨"));
    QVERIFY(!FindMember(*data, "陈清晨"));
    QVERIFY(FindMember(*data, "陈俊浩")->children == QStringList({"陈晨"}));

    QVERIFY2(RenameMember(data, "陈晨", "陈俊浩").contains("已被占用"), "重名应被拒绝");
    QVERIFY(RenameMember(data, "查无此人", "X").contains("不存在"));
    QVERIFY2(RenameMember(data, "陈晨", "陈,晨").contains("逗号"), "逗号应被拒绝");

    // 落盘并重载
    QVERIFY(SaveFamilyData(filePath, *data));
    QString msg;
    FamData* reloaded = LoadFamilyData(filePath, &msg);
    QVERIFY2(reloaded != nullptr, qPrintable(msg));
    QVERIFY(FindMember(*reloaded, "陈晨"));
    FreeFamData(reloaded);

    // 改回原名，恢复初始状态
    QVERIFY(RenameMember(data, "陈晨", "陈清晨").isEmpty());
    QVERIFY(FindMember(*data, "陈清晨"));
}

void TestDataLayer::bracketFormat()
{
    const QString bracket = DispTreeInBracketFormat(data->tree);
    QVERIFY2(bracket.startsWith("陈鼎元("), qPrintable(bracket));
    QVERIFY(bracket.contains("李秀英"));
    QVERIFY(bracket.contains("陈清晨"));

    // FreeTree 不崩溃（后序释放）
    BTree* bt = CreatBTree("陈鼎元", data->records.data(), static_cast<int>(data->records.size()));
    QVERIFY(bt != nullptr);
    FreeTree(bt);
}

void TestDataLayer::textTree()
{
    const QString text = DisplayFamilyTreeText(data->tree);
    QVERIFY2(text.startsWith("陈鼎元（妻：奚秀兰）"), qPrintable(text.left(60)));
    QVERIFY(text.contains("├─ 陈永刚"));
    QVERIFY(text.contains("└─ 陈永乐"));
    QVERIFY(text.contains("陈清晨"));

    const QString recordsText = RecordsToText(*data);
    QVERIFY(recordsText.contains("陈鼎元,奚秀兰,陈建国"));
}

void TestDataLayer::gbkFallback()
{
    // 构造旧版 GBK 数据文件
    const QString gbkFile = dir.filePath("gbk_old.txt");
    const QString content = "始祖,始祖妻,长子\n长子,长媳,长孙\n";
    QStringEncoder gbkEnc(QStringLiteral("GBK"));
    QVERIFY(gbkEnc.isValid());
    {
        QFile f(gbkFile);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(gbkEnc.encode(content));
    }

    QString msg;
    FamData* d = LoadFamilyData(gbkFile, &msg);
    QVERIFY2(d != nullptr, qPrintable(msg));
    QCOMPARE(static_cast<int>(d->members.size()), 5);
    QCOMPARE(d->rootName, QString("始祖"));
    QVERIFY2(msg.contains("GBK"), qPrintable("应提示 GBK 迁移: " + msg));

    // 文件已被自动重写为 UTF-8
    QFile f2(gbkFile);
    QVERIFY(f2.open(QIODevice::ReadOnly));
    const QByteArray rewritten = f2.readAll();
    QStringDecoder utf8Dec(QStringConverter::Utf8, QStringConverter::Flag::Stateless);
    utf8Dec.decode(rewritten);
    QVERIFY2(!utf8Dec.hasError(), "迁移后文件应为合法 UTF-8");
    FreeFamData(d);
}

QTEST_MAIN(TestDataLayer)
#include "tst_datalayer.moc"

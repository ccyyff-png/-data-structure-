#ifndef FAMILYTREE_H
#define FAMILYTREE_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QPair>
#include <map>
#include <vector>

/**
 * 家谱数据结构层（数据结构课设核心）
 *
 * 存储模型：BTree（二叉树），采用「左孩子-右兄弟」编码表示家谱：
 *
 *     父亲节点->lchild  = 妻（配偶节点）
 *     妻节点  ->rchild  = 长子；每个儿子->rchild = 下一个兄弟（儿子链）
 *     儿子节点->lchild  = 自己的妻
 *
 * 例：陈鼎元(lchild=奚秀兰) → 奚秀兰(rchild=陈建国) → 陈建国(lchild=李秀英) → ...
 *
 * 业务层：FamData —— 从记录(父亲,母亲,儿子)构建的平面成员索引（map<姓名, MemberInfo>）。
 * 所有查询/统计/可视化均基于 FamData（结构上消除了二叉树遍历导致的查找错误）；
 * BTree 仅在「二叉树结构」页面重建，用于展示课设要求的括号表示法等输出。
 *
 * 数据文件：UTF-8 编码 CSV，每行一条记录：父亲,母亲,儿子
 * 姓名唯一性不变量：同一姓名只对应一名成员（加载/添加时校验）。
 */

constexpr int MaxSize = 100;  // 姓名最大长度 + 1（含 '\0'）

// 一条家谱记录：父亲,母亲,儿子
typedef struct fnode
{
    char father[MaxSize];
    char wife[MaxSize];
    char son[MaxSize];
} FamType;

// 二叉树节点：家谱中的一名成员（name 存 UTF-8 字节，'\0' 结尾）
typedef struct tnode
{
    char name[MaxSize];
    struct tnode *lchild, *rchild;
} BTree;

// 成员完整档案（业务层）
struct MemberInfo
{
    QString name;
    bool male = true;          // 性别：出现在母亲列 = 女，否则男
    QString father;            // 父亲（始祖为空）
    QString mother;            // 母亲（始祖为空）
    QString spouse;            // 配偶（未婚为空）
    QStringList children;      // 子女（自己的儿子列记录）
    int generation = 0;        // 代数（始祖 = 0，UI 显示「第 generation+1 代」）
    bool isRoot = false;       // 是否家谱始祖
};

// 家谱数据全集
struct FamData
{
    std::vector<FamType> records;              // 全部原始记录
    std::map<QString, MemberInfo> members;     // 姓名 -> 成员索引
    QString rootName;                          // 始祖姓名
    int maxGeneration = 0;                     // 最大代数下标（总代数 = maxGeneration + 1）
    BTree* tree = nullptr;                     // 二叉树（课设展示用，随数据重建）
};

// ---------------- IO ----------------
// 加载家谱数据。filePath 优先按 UTF-8 解析，失败（如旧版 GBK 数据）自动回退系统编码
// 并原地重写为 UTF-8。错误/警告写入 errorMsg（可为 nullptr）；失败返回 nullptr。
FamData* LoadFamilyData(const QString& filePath, QString* errorMsg = nullptr);
// 将全部记录写回文件（UTF-8 CSV）
bool SaveFamilyData(const QString& filePath, const FamData& data);
// 释放家谱数据（含二叉树）
void FreeFamData(FamData* data);

// ---------------- BTree（课设展示） ----------------
// 由记录数组构建家谱二叉树（数组首条记录的父亲为根）
BTree* CreatBTree(const char* root, const FamType* fam, int n);
// 后序遍历释放二叉树
void FreeTree(BTree* bt);
// 括号表示法输出二叉树，如 陈鼎元(奚秀兰(陈建国(李秀英(...))))
QString DispTreeInBracketFormat(const BTree* bt);
// 家谱文本树（基于二叉树编码渲染家族视图：夫-妻同行，子女缩进挂其下）
QString DisplayFamilyTreeText(const BTree* bt);
// 全部记录文本（父亲/母亲/儿子 三列）
QString RecordsToText(const FamData& data);

// ---------------- 查询 ----------------
const MemberInfo* FindMember(const FamData& d, const QString& name);
// 血亲祖先列表（先父后母、由近及远；始祖返回空）
QStringList FindAllAncestors(const FamData& d, const QString& name);
// 子女列表（女性返回其丈夫的子女；无子女返回空）
QStringList FindAllChildren(const FamData& d, const QString& name);
// 姓名模糊搜索（包含匹配，按代际+姓名排序）
QStringList FuzzySearch(const FamData& d, const QString& keyword);

// ---------------- 修改（成功返回空串，失败返回中文错误信息） ----------------
// 添加一条记录（父亲,母亲,儿子）：校验 非空/无逗号/父亲存在/儿子不重名/妻一致/记录不重复
QString AddMember(FamData* d, const QString& father, const QString& wife, const QString& son);
// 删除成员及其全部后代（级联）：拒删始祖、拒删配偶（需删其丈夫）
QString DeleteMember(FamData* d, const QString& name);
// 重命名成员（同步更新所有记录中的三列）
QString RenameMember(FamData* d, const QString& oldName, const QString& newName);
// 某成员的血亲后代数量（不含本人，删除确认框用）
int CountDescendants(const FamData& d, const QString& name);

// ---------------- 统计 ----------------
struct StatsSummary
{
    int totalMembers = 0;      // 总人数
    int generationCount = 0;   // 总代数
    int longestBranch = 0;     // 最长支系人数
    int maxChildren = 0;       // 单户最多子女数
    QString busiestName;       // 子女最多的成员
};
StatsSummary ComputeStats(const FamData& d);
// 每代人数（下标 = 代数，长度 = 总代数）
QVector<int> CountPerGeneration(const FamData& d);
// 子女数量分布：子女数 -> 户数（按父亲计）
QMap<int, int> ChildrenCountDistribution(const FamData& d);
// 各支系人数 Top-K（支系根 = 始祖的子女；始祖仅一子时取其子女；人数含配偶）
QVector<QPair<QString, int>> BranchSizes(const FamData& d, int topK);
// 最长支系路径（始祖 -> ... -> 最深处后代）
QStringList LongestBranchPath(const FamData& d);

#endif // FAMILYTREE_H

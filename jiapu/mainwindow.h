#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "familytree.h"

class QListWidget;
class QStackedWidget;
class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QTableWidget;
class TreeView;
class BarChartWidget;
class HBarChartWidget;

// 主窗口（界面全部由代码构建，不依赖 .ui 文件）
// 布局：顶部工具栏（搜索/返回登录/退出）｜ 左侧导航 ｜ 中央 QStackedWidget 四页 ｜ 底部状态栏
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearch();                    // 搜索框（模糊匹配 + 树图高亮定位）
    void onNavChanged(int index);       // 切换页面
    void onMemberSelected(const QString& name);   // 树图点击成员 → 详情面板
    void onAddMember();                 // 成员管理：添加
    void onDeleteMember();              // 成员管理：删除（级联）
    void onRenameMember();              // 成员管理：重命名
    void showBracketFormat();           // 二叉树结构页：括号表示法
    void showRecords();                 // 二叉树结构页：全部记录
    void showTextTree();                // 二叉树结构页：家谱文本树
    void onBackToLogin();               // 返回登录窗口

private:
    void setupUi();                     // 构建界面与样式
    void applyStyle();                  // 全局 QSS 样式
    void reloadData();                  // 重新加载数据文件
    void refreshAll();                  // 统一刷新管线：树图/表格/图表/状态栏
    bool copyResourceToFile();          // 从内置资源复制初始数据文件

    // ---- 数据 ----
    FamData* m_data = nullptr;          // 家谱数据（唯一数据源）
    QString filepath;                   // 运行时数据文件路径（<exe>/jiapu/familytree.txt）

    // ---- 导航与工具栏 ----
    QListWidget* m_nav = nullptr;
    QStackedWidget* m_stack = nullptr;
    QLineEdit* m_searchEdit = nullptr;

    // ---- 页面 1：家谱树 ----
    QWidget* m_treePage = nullptr;
    TreeView* m_treeView = nullptr;
    QPlainTextEdit* m_detailPanel = nullptr;

    // ---- 页面 2：统计分析 ----
    QWidget* m_statsPage = nullptr;
    QLabel* m_kpiTotal = nullptr;
    QLabel* m_kpiGen = nullptr;
    QLabel* m_kpiBranch = nullptr;
    QLabel* m_kpiBusiest = nullptr;
    BarChartWidget* m_chartGen = nullptr;      // 每代人数柱状图
    BarChartWidget* m_chartChildren = nullptr; // 子女数量分布柱状图
    HBarChartWidget* m_chartBranches = nullptr;// 支系人数横向条形图
    QLabel* m_longestPathLabel = nullptr;      // 最长支系路径

    // ---- 页面 3：成员管理 ----
    QWidget* m_memberPage = nullptr;
    QTableWidget* m_memberTable = nullptr;

    // ---- 页面 4：二叉树结构（课设功能保留） ----
    QWidget* m_structPage = nullptr;
    QPlainTextEdit* m_structOutput = nullptr;

    // ---- 状态栏 ----
    QLabel* m_statusTotal = nullptr;
    QLabel* m_statusGen = nullptr;
    QLabel* m_statusBranch = nullptr;
};

#endif // MAINWINDOW_H

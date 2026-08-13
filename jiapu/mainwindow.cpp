#include "mainwindow.h"
#include "login.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    // 运行时数据文件：<exe>/jiapu/familytree.txt（不存在或为空时从内置资源恢复）
    const QString appPath = QCoreApplication::applicationDirPath();
    const QString jiapuDir = appPath + "/jiapu/";
    QDir().mkpath(jiapuDir);
    filepath = jiapuDir + "familytree.txt";

    bool ok = true;
    QFile destFile(filepath);
    if (destFile.exists() && destFile.size() == 0) {
        ok = (QMessageBox::question(this, "空文件", "检测到空家谱文件，是否从内置数据恢复？")
              == QMessageBox::Yes) && copyResourceToFile();
    } else if (!destFile.exists()) {
        ok = copyResourceToFile();
    }
    if (!ok) {
        QMessageBox::critical(this, "错误", "无法初始化家谱文件！");
        return;
    }
    reloadData();
}

MainWindow::~MainWindow()
{
    FreeFamData(m_data);
}

bool MainWindow::copyResourceToFile()
{
    QFile srcFile(":/familytreedata.txt");
    QFile destFile(filepath);
    if (!srcFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "错误", "无法打开内置资源数据！");
        return false;
    }
    if (!destFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, "错误", "无法创建数据文件！");
        return false;
    }
    destFile.write(srcFile.readAll());
    srcFile.close();
    destFile.close();
    return true;
}

void MainWindow::reloadData()
{
    FreeFamData(m_data);
    m_data = nullptr;
    QString msg;
    m_data = LoadFamilyData(filepath, &msg);
    if (!m_data) {
        QMessageBox::critical(this, "错误", "加载家谱数据失败：\n" + msg);
        return;
    }
    if (!msg.isEmpty())
        QMessageBox::warning(this, "提示", "加载数据时发现以下问题：\n" + msg);
    refreshAll();
}

// ---------------- 界面构建 ----------------

void MainWindow::setupUi()
{
    setWindowTitle("家谱管理系统");
    setWindowIcon(QIcon(":/logo.png"));
    resize(1280, 800);

    // ---- 顶部工具栏 ----
    QToolBar* bar = addToolBar("main");
    bar->setMovable(false);
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("搜索成员姓名（模糊匹配）…");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setFixedWidth(280);
    QPushButton* searchBtn = new QPushButton("搜索");
    auto* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QPushButton* backBtn = new QPushButton("返回登录");
    QPushButton* quitBtn = new QPushButton("退出");
    bar->addWidget(m_searchEdit);
    bar->addWidget(searchBtn);
    bar->addWidget(spacer);
    bar->addWidget(backBtn);
    bar->addWidget(quitBtn);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::onBackToLogin);
    connect(quitBtn, &QPushButton::clicked, this, &QMainWindow::close);

    // ---- 左侧导航 ----
    m_nav = new QListWidget;
    m_nav->setFixedWidth(200);
    m_nav->addItems({QStringLiteral("家谱树"), QStringLiteral("统计分析"),
                     QStringLiteral("成员管理"), QStringLiteral("二叉树结构")});
    m_nav->setCurrentRow(0);

    // ---- 页面 1：家谱树（Phase 5 填充 TreeView） ----
    m_treePage = new QWidget;
    {
        auto* lay = new QHBoxLayout(m_treePage);
        auto* placeholder = new QLabel("家谱树可视化（建设中）");
        placeholder->setAlignment(Qt::AlignCenter);
        lay->addWidget(placeholder);
    }

    // ---- 页面 2：统计分析（Phase 6 填充图表） ----
    m_statsPage = new QWidget;
    {
        auto* lay = new QVBoxLayout(m_statsPage);
        auto* placeholder = new QLabel("统计分析（建设中）");
        placeholder->setAlignment(Qt::AlignCenter);
        lay->addWidget(placeholder);
    }

    // ---- 页面 3：成员管理（Phase 7 填充表格） ----
    m_memberPage = new QWidget;
    {
        auto* lay = new QVBoxLayout(m_memberPage);
        auto* placeholder = new QLabel("成员管理（建设中）");
        placeholder->setAlignment(Qt::AlignCenter);
        lay->addWidget(placeholder);
    }

    // ---- 页面 4：二叉树结构（课设功能保留） ----
    m_structPage = new QWidget;
    {
        auto* lay = new QVBoxLayout(m_structPage);
        auto* btnRow = new QHBoxLayout;
        auto* btnBracket = new QPushButton("括号表示法输出二叉树");
        auto* btnRecords = new QPushButton("输出全部记录");
        auto* btnTextTree = new QPushButton("家谱文本树");
        btnRow->addWidget(btnBracket);
        btnRow->addWidget(btnRecords);
        btnRow->addWidget(btnTextTree);
        btnRow->addStretch();
        m_structOutput = new QPlainTextEdit;
        m_structOutput->setReadOnly(true);
        QFont monoFont("Microsoft YaHei UI", 11);
        m_structOutput->setFont(monoFont);
        lay->addLayout(btnRow);
        lay->addWidget(m_structOutput, 1);
        connect(btnBracket, &QPushButton::clicked, this, &MainWindow::showBracketFormat);
        connect(btnRecords, &QPushButton::clicked, this, &MainWindow::showRecords);
        connect(btnTextTree, &QPushButton::clicked, this, &MainWindow::showTextTree);
    }

    m_stack = new QStackedWidget;
    m_stack->addWidget(m_treePage);
    m_stack->addWidget(m_statsPage);
    m_stack->addWidget(m_memberPage);
    m_stack->addWidget(m_structPage);

    // ---- 中央布局 ----
    auto* central = new QWidget;
    auto* mainLay = new QHBoxLayout(central);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);
    mainLay->addWidget(m_nav);
    mainLay->addWidget(m_stack, 1);
    setCentralWidget(central);
    connect(m_nav, &QListWidget::currentRowChanged, this, &MainWindow::onNavChanged);

    // ---- 状态栏 ----
    m_statusTotal = new QLabel;
    m_statusGen = new QLabel;
    m_statusBranch = new QLabel;
    statusBar()->addWidget(m_statusTotal);
    statusBar()->addWidget(m_statusGen);
    statusBar()->addWidget(m_statusBranch);

    applyStyle();
}

void MainWindow::applyStyle()
{
    setStyleSheet(QStringLiteral(R"(
        QMainWindow { background: #f9f9f7; }
        QToolBar { background: #fcfcfb; border-bottom: 1px solid #e1e0d9; spacing: 8px; padding: 4px 8px; }
        QLineEdit { border: 1px solid #c3c2b7; border-radius: 6px; padding: 5px 10px; background: white; }
        QLineEdit:focus { border: 1px solid #2a78d6; }
        QPushButton { background: #2a78d6; color: white; border: none; border-radius: 6px; padding: 6px 16px; }
        QPushButton:hover { background: #1f62b3; }
        QPushButton:pressed { background: #1a5296; }
        QListWidget { background: #f2f1ec; border: none; border-right: 1px solid #e1e0d9; font-size: 14px; }
        QListWidget::item { padding: 14px 16px; color: #52514e; }
        QListWidget::item:selected { background: #2a78d6; color: white; }
        QListWidget::item:hover:!selected { background: #e6e4db; }
        QStackedWidget > QWidget { background: #f9f9f7; }
        QStatusBar { background: #fcfcfb; border-top: 1px solid #e1e0d9; color: #52514e; }
        QPlainTextEdit { background: white; border: 1px solid #e1e0d9; border-radius: 6px; font-size: 13px; }
    )"));
}

// ---------------- 槽函数 ----------------

void MainWindow::onNavChanged(int index)
{
    m_stack->setCurrentIndex(index);
}

void MainWindow::onSearch()
{
    if (!m_data)
        return;
    const QString keyword = m_searchEdit->text().trimmed();
    if (keyword.isEmpty())
        return;
    const QStringList hits = FuzzySearch(*m_data, keyword);
    if (hits.isEmpty()) {
        QMessageBox::information(this, "搜索结果", QString("未找到包含「%1」的成员").arg(keyword));
        return;
    }
    // Phase 5 起改为在树图中高亮并定位；此处先以列表展示
    QString text = QString("找到 %1 名成员：\n").arg(hits.size());
    for (const QString& n : hits)
        text += "· " + n + "\n";
    QMessageBox::information(this, "搜索结果", text);
}

void MainWindow::showBracketFormat()
{
    if (!m_data) return;
    m_structOutput->setPlainText(DispTreeInBracketFormat(m_data->tree));
}

void MainWindow::showRecords()
{
    if (!m_data) return;
    m_structOutput->setPlainText(RecordsToText(*m_data));
}

void MainWindow::showTextTree()
{
    if (!m_data) return;
    m_structOutput->setPlainText(DisplayFamilyTreeText(m_data->tree));
}

void MainWindow::onBackToLogin()
{
    auto* l = new login;
    l->setAttribute(Qt::WA_DeleteOnClose);
    l->show();
    close();   // 本窗口由登录窗口以 WA_DeleteOnClose 创建，关闭即销毁
}

void MainWindow::onMemberSelected(const QString& name)
{
    Q_UNUSED(name)   // Phase 5 接入树图后填充详情面板
}

void MainWindow::onAddMember()
{
    Q_UNUSED(0)      // Phase 7 实现
}

void MainWindow::onDeleteMember()
{
    Q_UNUSED(0)      // Phase 7 实现
}

void MainWindow::onRenameMember()
{
    Q_UNUSED(0)      // Phase 7 实现
}

// ---------------- 刷新管线 ----------------

void MainWindow::refreshAll()
{
    if (!m_data)
        return;
    const StatsSummary s = ComputeStats(*m_data);
    m_statusTotal->setText(QString("  总人数：%1 ｜ ").arg(s.totalMembers));
    m_statusGen->setText(QString("代数：%1 ｜ ").arg(s.generationCount));
    m_statusBranch->setText(QString("最长支系：%1 人 ｜ ").arg(s.longestBranch));
    // Phase 5+：树图 / 成员表格 / 统计图表随数据刷新
}

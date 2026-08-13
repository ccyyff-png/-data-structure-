#include "mainwindow.h"
#include "login.h"
#include "treeview.h"
#include "chartwidget.h"
#include "memberdialog.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QInputDialog>
#include <algorithm>
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

    // ---- 页面 1：家谱树（QGraphicsView 自绘树形图 + 详情面板） ----
    m_treePage = new QWidget;
    {
        auto* lay = new QHBoxLayout(m_treePage);
        m_treeView = new TreeView;
        m_detailPanel = new QPlainTextEdit;
        m_detailPanel->setReadOnly(true);
        m_detailPanel->setFixedWidth(300);
        QFont detailFont("Microsoft YaHei UI", 10);
        m_detailPanel->setFont(detailFont);
        lay->addWidget(m_treeView, 1);
        lay->addWidget(m_detailPanel);
        connect(m_treeView, &TreeView::memberSelected, this, &MainWindow::onMemberSelected);
        connect(m_treeView, &TreeView::memberDoubleClicked, this, &MainWindow::onMemberSelected);
    }

    // ---- 页面 2：统计分析（KPI 卡片 + 自绘图表） ----
    m_statsPage = new QWidget;
    {
        auto* lay = new QVBoxLayout(m_statsPage);
        // KPI 卡片行
        auto* kpiRow = new QHBoxLayout;
        const auto makeCard = [](QLabel*& value, const QString& caption) {
            auto* card = new QFrame;
            card->setObjectName("kpiCard");
            auto* v = new QVBoxLayout(card);
            value = new QLabel("—");
            value->setObjectName("kpiValue");
            value->setAlignment(Qt::AlignCenter);
            auto* cap = new QLabel(caption);
            cap->setObjectName("kpiCaption");
            cap->setAlignment(Qt::AlignCenter);
            v->addWidget(value);
            v->addWidget(cap);
            return card;
        };
        kpiRow->addWidget(makeCard(m_kpiTotal, "总人数"));
        kpiRow->addWidget(makeCard(m_kpiGen, "总代数"));
        kpiRow->addWidget(makeCard(m_kpiBranch, "最长支系"));
        kpiRow->addWidget(makeCard(m_kpiBusiest, "子女最多"));
        // 图表行
        auto* chartRow = new QHBoxLayout;
        m_chartGen = new BarChartWidget;
        m_chartChildren = new BarChartWidget;
        m_chartBranches = new HBarChartWidget;
        chartRow->addWidget(m_chartGen, 1);
        chartRow->addWidget(m_chartChildren, 1);
        chartRow->addWidget(m_chartBranches, 1);
        // 最长支系路径条
        m_longestPathLabel = new QLabel;
        m_longestPathLabel->setObjectName("pathLabel");
        m_longestPathLabel->setWordWrap(true);
        lay->addLayout(kpiRow);
        lay->addLayout(chartRow, 1);
        lay->addWidget(m_longestPathLabel);
    }

    // ---- 页面 3：成员管理（表格 + 增/删/改名） ----
    m_memberPage = new QWidget;
    {
        auto* lay = new QVBoxLayout(m_memberPage);
        auto* btnRow = new QHBoxLayout;
        auto* btnAdd = new QPushButton("添加成员");
        auto* btnDelete = new QPushButton("删除成员");
        auto* btnRename = new QPushButton("重命名成员");
        btnRow->addWidget(btnAdd);
        btnRow->addWidget(btnDelete);
        btnRow->addWidget(btnRename);
        btnRow->addStretch();
        m_memberTable = new QTableWidget;
        m_memberTable->setColumnCount(7);
        m_memberTable->setHorizontalHeaderLabels(
            {QStringLiteral("姓名"), QStringLiteral("性别"), QStringLiteral("代数"),
             QStringLiteral("父亲"), QStringLiteral("母亲"), QStringLiteral("配偶"),
             QStringLiteral("子女数")});
        m_memberTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_memberTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_memberTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_memberTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_memberTable->setSelectionMode(QAbstractItemView::SingleSelection);
        m_memberTable->verticalHeader()->setVisible(false);
        m_memberTable->setAlternatingRowColors(true);
        lay->addLayout(btnRow);
        lay->addWidget(m_memberTable, 1);
        connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddMember);
        connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteMember);
        connect(btnRename, &QPushButton::clicked, this, &MainWindow::onRenameMember);
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
        QFrame#kpiCard { background: white; border: 1px solid #e1e0d9; border-radius: 8px; }
        QLabel#kpiValue { font-size: 26px; font-weight: bold; color: #0b0b0b; }
        QLabel#kpiCaption { color: #898781; font-size: 12px; }
        QLabel#pathLabel { background: white; border: 1px solid #e1e0d9; border-radius: 6px;
                           padding: 8px 12px; color: #52514e; font-size: 12px; }
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
    // 切到家谱树页：高亮全部命中并居中第一个
    m_nav->setCurrentRow(0);
    m_treeView->highlightMatches(keyword);
    statusBar()->showMessage(QString("找到 %1 名成员，已在树图中高亮").arg(hits.size()), 4000);
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
    if (!m_data)
        return;
    const MemberInfo* m = FindMember(*m_data, name);
    if (!m)
        return;
    QStringList lines;
    lines << QString("【%1】").arg(m->name);
    lines << QString("性别：%1      第 %2 代").arg(m->male ? "男" : "女").arg(m->generation + 1);
    if (m->isRoot)
        lines << "身份：家谱始祖";
    if (!m->father.isEmpty())
        lines << QString("父亲：%1").arg(m->father);
    if (!m->mother.isEmpty())
        lines << QString("母亲：%1").arg(m->mother);
    lines << QString("配偶：%1").arg(m->spouse.isEmpty() ? "无" : m->spouse);
    lines << QString("子女：%1").arg(m->children.isEmpty() ? "无" : m->children.join("、"));
    const QStringList ancestors = FindAllAncestors(*m_data, name);
    if (ancestors.isEmpty()) {
        lines << "血亲祖先：无（始祖）";
    } else {
        QStringList blood;   // 祖先列表为 父,母,祖父,祖母...，偶数下标为血亲
        for (int i = 0; i < ancestors.size(); i += 2)
            blood << ancestors[i];
        lines << QString("血亲祖先：%1").arg(blood.join(" → "));
    }
    lines << QString("后代人数：%1 人").arg(CountDescendants(*m_data, name));
    m_detailPanel->setPlainText(lines.join("\n"));
}

void MainWindow::onAddMember()
{
    if (!m_data)
        return;
    MemberDialog dlg(this);
    while (dlg.exec() == QDialog::Accepted) {   // 失败时重新弹出，输入得以保留
        const QString err = AddMember(m_data, dlg.father(), dlg.wife(), dlg.son());
        if (err.isEmpty()) {
            QMessageBox::information(this, "提示", "记录已添加！");
            persistAndRefresh();
            return;
        }
        QMessageBox::warning(this, "提示", err);
    }
}

void MainWindow::onDeleteMember()
{
    if (!m_data)
        return;
    const int row = m_memberTable->currentRow();
    if (row < 0 || !m_memberTable->item(row, 0)) {
        QMessageBox::warning(this, "提示", "请先在表格中选择一名成员");
        return;
    }
    const QString name = m_memberTable->item(row, 0)->text();
    const int n = CountDescendants(*m_data, name);
    const QString scope = n > 0 ? QString("及其后代共 %1 人，").arg(n + 1) : QString();
    const auto res = QMessageBox::warning(this, "删除确认",
        QString("将删除「%1」%2全部相关家谱记录，且不可恢复。确定？").arg(name, scope),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (res != QMessageBox::Yes)
        return;
    const QString result = DeleteMember(m_data, name);
    if (result.startsWith("已删除")) {
        QMessageBox::information(this, "提示", result);
        persistAndRefresh();
    } else {
        QMessageBox::warning(this, "提示", result);
    }
}

void MainWindow::onRenameMember()
{
    if (!m_data)
        return;
    const int row = m_memberTable->currentRow();
    if (row < 0 || !m_memberTable->item(row, 0)) {
        QMessageBox::warning(this, "提示", "请先在表格中选择一名成员");
        return;
    }
    const QString oldName = m_memberTable->item(row, 0)->text();
    bool ok = false;
    const QString newName = QInputDialog::getText(this, "重命名成员",
        QString("将「%1」重命名为：").arg(oldName), QLineEdit::Normal, oldName, &ok);
    if (!ok)
        return;
    const QString err = RenameMember(m_data, oldName, newName);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, "提示", err);
        return;
    }
    QMessageBox::information(this, "提示", QString("已将「%1」重命名为「%2」").arg(oldName, newName));
    persistAndRefresh();
}

// ---------------- 刷新管线 ----------------

void MainWindow::persistAndRefresh()
{
    if (!SaveFamilyData(filepath, *m_data)) {
        QMessageBox::critical(this, "错误", "保存数据文件失败！");
        return;
    }
    reloadData();
}

void MainWindow::refreshAll()
{
    if (!m_data)
        return;
    const StatsSummary s = ComputeStats(*m_data);
    m_statusTotal->setText(QString("  总人数：%1 ｜ ").arg(s.totalMembers));
    m_statusGen->setText(QString("代数：%1 ｜ ").arg(s.generationCount));
    m_statusBranch->setText(QString("最长支系：%1 人 ｜ ").arg(s.longestBranch));
    // 家谱树图随数据重建
    if (m_treeView)
        m_treeView->setData(m_data);

    // KPI 卡片
    if (m_kpiTotal) {
        m_kpiTotal->setText(QString::number(s.totalMembers));
        m_kpiGen->setText(QString::number(s.generationCount));
        m_kpiBranch->setText(QString::number(s.longestBranch));
        m_kpiBusiest->setText(QString("%1 ×%2").arg(s.busiestName).arg(s.maxChildren));
    }
    // 图表
    if (m_chartGen) {
        const QVector<int> perGen = CountPerGeneration(*m_data);
        QStringList genLabels;
        QVector<double> genVals;
        for (int i = 0; i < perGen.size(); ++i) {
            genLabels << QString("第%1代").arg(i + 1);
            genVals << perGen[i];
        }
        m_chartGen->setData("每代人数", genLabels, genVals, QColor(0x2a, 0x78, 0xd6));

        const QMap<int, int> dist = ChildrenCountDistribution(*m_data);
        QStringList distLabels;
        QVector<double> distVals;
        for (auto it = dist.begin(); it != dist.end(); ++it) {
            distLabels << QString("%1 个子女").arg(it.key());
            distVals << it.value();
        }
        m_chartChildren->setData("子女数量分布（户）", distLabels, distVals, QColor(0x1b, 0xaf, 0x7a));

        const auto branches = BranchSizes(*m_data, 5);
        QStringList brNames;
        QVector<double> brVals;
        for (const auto& b : branches) {
            brNames << b.first;
            brVals << b.second;
        }
        m_chartBranches->setData("支系人数 Top5", brNames, brVals);

        const QStringList path = LongestBranchPath(*m_data);
        m_longestPathLabel->setText(QString("最长支系（%1 人）：%2")
                                        .arg(path.size())
                                        .arg(path.join(" → ")));
    }
    // 成员表格（按代际+姓名排序）
    if (m_memberTable) {
        QVector<const MemberInfo*> list;
        for (const auto& [name, m] : m_data->members)
            list.append(&m);
        std::sort(list.begin(), list.end(), [](const MemberInfo* a, const MemberInfo* b) {
            return a->generation != b->generation ? a->generation < b->generation
                                                  : a->name < b->name;
        });
        m_memberTable->setRowCount(list.size());
        for (int row = 0; row < list.size(); ++row) {
            const MemberInfo* m = list[row];
            const QStringList cells = {
                m->name,
                m->male ? "男" : "女",
                QString("第 %1 代").arg(m->generation + 1),
                m->father,
                m->mother,
                m->spouse,
                QString::number(m->children.size())};
            for (int col = 0; col < cells.size(); ++col) {
                auto* item = new QTableWidgetItem(cells[col]);
                if (col != 0)
                    item->setTextAlignment(Qt::AlignCenter);
                m_memberTable->setItem(row, col, item);
            }
        }
    }
}

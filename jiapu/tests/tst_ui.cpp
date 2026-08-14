#include <QtTest>
#include <QApplication>
#include <QDir>
#include <QStackedWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QImage>
#include "login.h"
#include "mainwindow.h"
#include "treeview.h"

// UI 冒烟测试：离屏渲染（QT_QPA_PLATFORM=offscreen）各页面，截图保存到 build/screenshots，
// 并通过像素颜色断言验证关键渲染结果（树图节点配色/图表柱体/搜索高亮）。
class TestUi : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void loginWindow();
    void mainWindowPages();
    void searchAndSelect();
    void memberOps();

private:
    // 统计图片中指定颜色（容差内）的像素数
    static int countColor(const QImage& img, const QColor& target, int step = 4, int tol = 12);
};

void TestUi::initTestCase()
{
    QDir().mkpath(QDir::currentPath() + "/screenshots");
}

int TestUi::countColor(const QImage& img, const QColor& target, int step, int tol)
{
    int count = 0;
    for (int y = 0; y < img.height(); y += step) {
        for (int x = 0; x < img.width(); x += step) {
            const QColor c = img.pixelColor(x, y);
            if (std::abs(c.red() - target.red()) <= tol
                && std::abs(c.green() - target.green()) <= tol
                && std::abs(c.blue() - target.blue()) <= tol)
                ++count;
        }
    }
    return count;
}

void TestUi::loginWindow()
{
    // 仅验证登录窗口可渲染（错误密码路径会弹模态框，离屏模式下不可自动化）
    login w;
    w.show();
    QTest::qWait(300);
    QVERIFY(!w.grab().isNull());

    // 回归：窗口缩放时背景图与表单应同步（背景随布局拉伸、表单重新居中）
    auto* bg = w.findChild<QLabel*>("label_background");
    auto* loginBtn = w.findChild<QPushButton*>("pushButton");
    QVERIFY(bg && loginBtn);
    const int bgW = bg->width();
    const int btnX = loginBtn->x();
    w.resize(1500, 820);
    QTest::qWait(200);
    QVERIFY2(bg->width() > bgW, "窗口放大后背景未同步拉伸");
    QVERIFY2(loginBtn->x() > btnX, "窗口放大后表单未同步右移居中");
    w.grab().save(QDir::currentPath() + "/screenshots/login.png");
}

void TestUi::mainWindowPages()
{
    MainWindow w;
    w.show();
    QTest::qWait(600);

    auto* stack = w.findChild<QStackedWidget*>();
    QVERIFY(stack);
    QCOMPARE(stack->count(), 4);

    const QString dir = QDir::currentPath() + "/screenshots";
    for (int i = 0; i < stack->count(); ++i) {
        stack->setCurrentIndex(i);
        QTest::qWait(200);
        const QImage shot = w.grab().toImage();
        QVERIFY(!shot.isNull());
        shot.save(dir + QString("/page%1.png").arg(i));
        if (i == 0) {
            // 家谱树页：必须渲染出蓝色男节点与粉色女节点卡片
            QVERIFY2(countColor(shot, QColor(0x2a, 0x78, 0xd6)) > 200, "树图男性节点未渲染");
            QVERIFY2(countColor(shot, QColor(0xe8, 0x7b, 0xa4)) > 200, "树图女性节点未渲染");
            // 回归：同行卡片不得相互遮挡
            const auto* treeView = w.findChild<TreeView*>();
            QVERIFY(treeView);
            const QStringList overlaps = treeView->overlappingPairs();
            QVERIFY2(overlaps.isEmpty(),
                     qPrintable("树图存在重叠卡片: " + overlaps.join(", ")));
        }
        if (i == 1) {
            // 统计页：蓝色柱体（每代人数）+ 绿色柱体（子女分布）
            QVERIFY2(countColor(shot, QColor(0x2a, 0x78, 0xd6)) > 200, "统计图蓝色柱体未渲染");
            QVERIFY2(countColor(shot, QColor(0x1b, 0xaf, 0x7a)) > 100, "统计图绿色柱体未渲染");
        }
    }
}

void TestUi::searchAndSelect()
{
    MainWindow w;
    w.show();
    QTest::qWait(600);

    auto* edit = w.findChild<QLineEdit*>();
    QVERIFY(edit);
    edit->setText("俊");
    QTest::keyClick(edit, Qt::Key_Return);   // 触发 onSearch → 高亮
    QTest::qWait(300);
    const QImage shot = w.grab().toImage();
    QVERIFY(!shot.isNull());
    shot.save(QDir::currentPath() + "/screenshots/search_jun.png");
    // 搜索命中节点应出现橙色高亮描边（描边细且抗锯齿，放宽容差统计）
    QVERIFY2(countColor(shot, QColor(0xeb, 0x68, 0x34), 2, 45) > 20, "搜索高亮未渲染");
}

void TestUi::memberOps()
{
    MainWindow w;
    w.show();
    QTest::qWait(600);

    // 切到成员管理页
    auto* stack = w.findChild<QStackedWidget*>();
    auto* nav = w.findChild<QListWidget*>();
    QVERIFY(stack && nav);
    nav->setCurrentRow(2);
    QTest::qWait(200);
    const QImage shot = w.grab().toImage();
    QVERIFY(!shot.isNull());
    shot.save(QDir::currentPath() + "/screenshots/page2_members.png");
    // 表格应有内容（白色表体区域大量存在）
    QVERIFY2(countColor(shot, Qt::white) > 2000, "成员表格未渲染");
}

QTEST_MAIN(TestUi)
#include "tst_ui.moc"

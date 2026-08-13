#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 最后一个窗口关闭即退出应用，避免后台残留进程（修复原 hide() 设计缺陷）
    a.setQuitOnLastWindowClosed(true);
    login w;
    w.show();
    return a.exec();
}

#include "login.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <windows.h>

// 全局崩溃处理器：记录异常码与指令地址（RVA，配合调试符号可用 addr2line 定位）
static LONG WINAPI crashHandler(EXCEPTION_POINTERS* ep)
{
    const DWORD code = ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionCode : 0;
    const quintptr fault = ep->ExceptionRecord
        ? reinterpret_cast<quintptr>(ep->ExceptionRecord->ExceptionAddress) : 0;
    const quintptr base = reinterpret_cast<quintptr>(GetModuleHandleW(nullptr));
    const QString logDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    QFile log(logDir + "/crash.log");
    if (log.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&log);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
            << "] 崩溃: 异常码 0x" << QString::number(code, 16)
            << " 指令地址 0x" << QString::number(fault, 16)
            << " (RVA 0x" << QString::number(fault - base, 16) << ")\n";
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 应用标识：决定用户数据目录（%APPDATA%/jiapu），数据不依赖程序所在路径
    QCoreApplication::setOrganizationName("jiapu");
    QCoreApplication::setApplicationName("jiapu");
    // 最后一个窗口关闭即退出应用，避免后台残留进程（修复原 hide() 设计缺陷）
    a.setQuitOnLastWindowClosed(true);
    SetUnhandledExceptionFilter(crashHandler);
    login w;
    w.show();
    return a.exec();
}

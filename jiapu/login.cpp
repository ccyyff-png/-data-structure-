#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QSettings>
#include <QCoreApplication>

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
    , bgTimer(new QTimer(this))   // 父对象 = this，由 Qt 自动释放（修复：原双重释放崩溃）
    , currentPicIndex(0)
{
    ui->setupUi(this);
    setWindowTitle("家谱管理系统");
    setWindowIcon(QIcon(":/logo.png"));   // 修复：原 "../jiapu/fa.png" 不存在

    ui->label_background->setScaledContents(true);
    // 修复：原资源路径缺少 .jpg 扩展名导致背景图加载失败（一直空白）
    bgPics << ":/bgp5.jpg" << ":/bgp4.jpg" << ":/bgp3.jpg";
    setNextPixmap();

    connect(bgTimer, &QTimer::timeout, this, &login::updatePixmap);
    bgTimer->start(3000);   // 每 3 秒切换背景

    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    // 回车即可登录
    ui->pushButton->setDefault(true);
    connect(ui->lineEdit_user, &QLineEdit::returnPressed, this, &login::on_pushButton_clicked);
    connect(ui->lineEdit_password, &QLineEdit::returnPressed, this, &login::on_pushButton_clicked);
}

login::~login()
{
    delete ui;   // bgTimer 是 QObject 子对象，随本窗口自动释放，不再手动 delete（修复双重释放）
}

void login::updatePixmap()
{
    setNextPixmap();
}

void login::setNextPixmap()
{
    if (!bgPics.isEmpty()) {
        currentPicIndex = (currentPicIndex + 1) % bgPics.size();
        ui->label_background->setPixmap(QPixmap(bgPics[currentPicIndex]));
    }
}

bool login::verifyPassword(const QString& user, const QString& pwd)
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    if (!settings.contains("auth/user") || !settings.contains("auth/passhash")) {
        // 首次运行：播种默认账号 admin / 12345，仅保存 SHA-256 哈希
        settings.setValue("auth/user", "admin");
        settings.setValue("auth/passhash",
            QString::fromLatin1(QCryptographicHash::hash("12345", QCryptographicHash::Sha256).toHex()));
        settings.sync();
    }
    const QByteArray inputHash =
        QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex();
    return user == settings.value("auth/user").toString()
        && QString::fromLatin1(inputHash) == settings.value("auth/passhash").toString();
}

void login::on_pushButton_clicked()
{
    const QString userName = ui->lineEdit_user->text().trimmed();
    const QString pwd = ui->lineEdit_password->text();
    if (!verifyPassword(userName, pwd)) {
        QMessageBox::critical(this, "提示", "登录信息错误！");
        ui->lineEdit_password->clear();
        ui->lineEdit_password->setFocus();
        return;
    }

    MainWindow *mainWindow = new MainWindow();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);   // 关闭主窗口即销毁，应用随之退出
    mainWindow->setWindowTitle("家谱管理系统");
    mainWindow->setWindowIcon(QIcon(":/logo.png"));
    mainWindow->show();
    this->close();   // 修复：关闭登录窗口（原 hide() 导致主窗口关闭后进程残留后台）
}

void login::on_pushButton_2_clicked()
{
    const int res = QMessageBox::question(this, "提示", "是否要关闭程序？");
    if (res == QMessageBox::Yes)
        this->close();   // 修复：正常关闭退出事件循环（原 exit(0) 绕过析构）
}

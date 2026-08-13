#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QStringList>
#include <QTimer>

namespace Ui {
class login;
}

// 登录窗口（界面见 login.ui）
class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_pushButton_clicked();   // 登录按钮（也响应输入框回车）
    void on_pushButton_2_clicked(); // 退出按钮
    void updatePixmap();            // 背景图片轮播

private:
    Ui::login *ui;
    QTimer* bgTimer;            // 背景轮播定时器（父对象管理，Qt 自动释放，勿手动 delete）
    QStringList bgPics;
    int currentPicIndex;
    void setNextPixmap();
    // 校验账号密码（SHA-256 哈希存储于 config.ini，首次运行自动播种 admin/12345）
    static bool verifyPassword(const QString& user, const QString& pwd);
};

#endif // LOGIN_H

#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <QPair>

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

protected:
    void resizeEvent(QResizeEvent* event) override;   // 表单随窗口缩放重新居中

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
    // 表单组件的原始 X 坐标（.ui 按 1291 宽设计），缩放时按中心偏移重定位
    QVector<QPair<QWidget*, int>> m_formOrigX;
};

#endif // LOGIN_H

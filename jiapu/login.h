#ifndef LOGIN_H
#define LOGIN_H
#include <QWidget>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void updatePixmap();  // 新增：定时器触发的图片更新函数

private:
    Ui::login *ui;
    QTimer* bgTimer;  // 新增：定时器
    QStringList bgPics;  // 新增：背景图片列表
    int currentPicIndex;  // 新增：当前图片索引
    void setNextPixmap();  // 新增：设置下一张图片
};
#endif // LOGIN_H
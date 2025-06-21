#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include <time.h>
#include <QMessageBox>
#include <QTimer>  // 新增：包含定时器头文件

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login),
    bgTimer(new QTimer(this)),  // 新增：初始化定时器
    currentPicIndex(0)  // 新增：初始化图片索引
{
    ui->setupUi(this);
    this->setWindowTitle("家谱管理系统");
    //    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //    this->ui->pushButton->setAttribute(Qt::WA_TranslucentBackground, true);
    //    this->ui->pushButton_2->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->label_background->setScaledContents(true);
    
    // 新增：初始化背景图片列表
    bgPics << ":/bgp5" << ":/bgp4" << ":/bgp3";
    
    // 新增：首次加载图片
    setNextPixmap();
    
    // 新增：连接定时器超时信号到更新槽函数（5秒切换一次）
    connect(bgTimer, &QTimer::timeout, this, &login::updatePixmap);
    bgTimer->start(3000);  // 5000毫秒 = 5秒
    
    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
}

login::~login()
{
    delete ui;
    // 新增：停止并删除定时器
    bgTimer->stop();
    delete bgTimer;
}

// 新增：更新图片的槽函数
void login::updatePixmap() {
    setNextPixmap();
}

// 新增：设置下一张图片
void login::setNextPixmap() {
    if (!bgPics.isEmpty()) {
        currentPicIndex = (currentPicIndex + 1) % bgPics.size();
        QPixmap pic(bgPics[currentPicIndex]);
        ui->label_background->setPixmap(pic);
    }
}

// 登录按钮
void login::on_pushButton_clicked()
{
    QString userName=this->ui->lineEdit_user->text();
    QString pwd=this->ui->lineEdit_password->text();
    if(userName=="admin"&&pwd=="12345")
    {
        qDebug()<<"登录成功";
         QMessageBox::information(this,"提示","登录成功");
    }
    else
    {
        QMessageBox::critical(this,"提示","登录信息错误！");
        return;
    }
    MainWindow *mainWindow = new MainWindow();
    mainWindow->setWindowTitle("家谱管理系统");
    mainWindow->setWindowIcon(QIcon("../jiapu/fa.png"));  //更改页面的图标
    mainWindow->show();
    this->hide();
}

// 退出按钮
void login::on_pushButton_2_clicked()
{
    int res = QMessageBox::question(this,"提示","是否要关闭程序？");  //弹出对话框进行确认是否退出
    if (res == QMessageBox::Yes){
        this->close();
        exit(0);
    }
    else {
        this->show();
    }
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "familytree.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <string>
#include <QTextEdit>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 使用程序所在目录
    QString appPath = QCoreApplication::applicationDirPath();
    QString jiapuPath = appPath + "/jiapu/";
    QDir().mkpath(jiapuPath);  // 确保目录存在
    filepath = jiapuPath + "familytree.txt";
    

    // 如果家谱文件不存在或为空，则从资源复制
    QFile destFile(filepath);
    
    // 新增统一文件验证逻辑
    bool fileValid = false;
    if (destFile.exists()) {
        if (destFile.size() > 0) {
            fileValid = true;
        } else {
            // 文件为空时提示用户
            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "空文件", "检测到空家谱文件，是否从资源恢复数据？",
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                fileValid = copyResourceToFile();
            }
        }
    } else {
        // 文件不存在时直接复制
        fileValid = copyResourceToFile();
    }

    if (!fileValid) {
        QMessageBox::critical(this, "错误", "无法初始化家谱文件！");
        root1 = NULL;  // 确保文件无效时根节点为空
        ui->textEdit->setText("家谱文件初始化失败");
        return;
    }

    root1 = ReadTxtAndCreateTree(filepath); // 使用可写路径读取家谱
    ui->label_bgp->setScaledContents(true);
    QPixmap pic(":/bgp2");
    ui->label_bgp->setPixmap(pic);
    // 初始显示家谱文件内容
    ui->textEdit->setText(OutputFamilyRecords(filepath));
}

// 新增资源文件复制函数
bool MainWindow::copyResourceToFile() {
    QFile srcFile(":/familytreedata.txt");
    QFile destFile(filepath);
    
    if (!srcFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "错误", "无法打开资源文件！");
        return false;
    }
    
    if (!destFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "错误", "无法创建目标文件！");
        return false;
    }
    
    if (!destFile.write(srcFile.readAll())) {
        QMessageBox::critical(this, "错误", "文件写入失败！");
        return false;
    }
    
    srcFile.close();
    destFile.close();
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    // 返回按钮，可以在此处实现返回功能，例如重置显示内容
    ui->textEdit->setText(OutputFamilyRecords(filepath));
}

void MainWindow::on_pushButton_3_clicked()
{
    // 退出按钮
    this->close();
}

void MainWindow::on_pushButton_4_clicked()
{
    // 查找某人所有祖先
    QString name = QInputDialog::getText(this, "输入姓名", "请输入姓名:");
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "姓名不能为空！");
        return;
    }
    QString result = FindAllAncestors(root1, name);
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_5_clicked()
{
    // 用括号表示法输出二叉树
    QString result = DispTreeInBracketFormat(root1);
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_6_clicked()
{
    // 输出家谱全部记录
    QString result = OutputFamilyRecords(filepath);
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_7_clicked()
{
    // 清除家谱全部记录
    QString result = ClearFamilyRecords(filepath);
    if (result == "家谱文件记录已清除") {
        QMessageBox::information(this, "提示", "家谱文件记录已清除！");
        root1 = NULL;  // 重置树结构
    } else {
        QMessageBox::warning(this, "警告", "无法清除家谱文件记录！");
    }
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_8_clicked()
{
    // 添加一个记录
    QString father = QInputDialog::getText(this, "输入父亲姓名", "请输入父亲姓名:");
    QString wife = QInputDialog::getText(this, "输入母亲姓名", "请输入母亲姓名:");
    QString son = QInputDialog::getText(this, "输入儿子姓名", "请输入儿子姓名:");
    if (father.isEmpty() || wife.isEmpty() || son.isEmpty()) {
        QMessageBox::warning(this, "警告", "姓名不能为空！");
        return;
    }
    QString result = AddRecordToFile(filepath, father, wife, son);
    if (result == "记录已添加") {
        QMessageBox::information(this, "提示", "记录已添加！");
        root1 = ReadTxtAndCreateTree(filepath); // 重新加载树
        ui->textEdit->setText(OutputFamilyRecords(filepath));
    } else {
        QMessageBox::warning(this, "警告", "无法添加记录到家谱文件！");
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    // 查找成员
    QString name = QInputDialog::getText(this, "输入姓名", "请输入姓名:");
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "姓名不能为空！");
        return;
    }
    QString result = FindMemberInfo(root1, name);
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_1_clicked()
{
    // 查找某人所有儿子
    QString name = QInputDialog::getText(this, "输入姓名", "请输入姓名:");
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "姓名不能为空！");
        return;
    }
    QString result = FindAllSons(root1, name);
    ui->textEdit->setText(result);
}

void MainWindow::on_pushButton_19_clicked()
{
    // 家谱树二叉树展示
    QString result = DisplayFamilyTree(root1);
    ui->textEdit->setText(result);
}

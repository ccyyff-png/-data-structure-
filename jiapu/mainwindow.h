#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "familytree.h"  // 包含 familytree 的定义
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    BTree* root1 = nullptr;  // 保存家谱二叉树的根节点
    QString filepath = ":/familytreedata.txt";  // 家谱文件路径

private slots:
    void on_pushButton_1_clicked();  // 查找某人所有儿子
    void on_pushButton_2_clicked();  // 查找某人所有祖先
    void on_pushButton_3_clicked();  // 用括号表示法输出二叉树
    void on_pushButton_4_clicked();  // 输出家谱全部记录
    void on_pushButton_5_clicked();  // 清除家谱全部记录
    void on_pushButton_6_clicked();  // 添加一个记录
    void on_pushButton_7_clicked();  // 查找成员
    void on_pushButton_8_clicked();  // 返回按钮
    void on_pushButton_9_clicked();  // 退出按钮
    void on_pushButton_19_clicked();  // 家谱树二叉树展示

private:
    bool copyResourceToFile();  // 新增资源文件复制函数声明
};

#endif // MAINWINDOW_H

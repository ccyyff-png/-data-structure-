/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label_bgp;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QTextEdit *textEdit;
    QPushButton *pushButton_1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_19;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1291, 701);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_bgp = new QLabel(centralwidget);
        label_bgp->setObjectName(QString::fromUtf8("label_bgp"));
        label_bgp->setGeometry(QRect(0, -30, 1291, 711));
        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(70, 10, 471, 631));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 469, 629));
        textEdit = new QTextEdit(scrollAreaWidgetContents);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 451, 611));
        QPalette palette;
        QBrush brush(QColor(213, 213, 213, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush);
        textEdit->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(11);
        textEdit->setFont(font);
        scrollArea->setWidget(scrollAreaWidgetContents);
        pushButton_1 = new QPushButton(centralwidget);
        pushButton_1->setObjectName(QString::fromUtf8("pushButton_1"));
        pushButton_1->setGeometry(QRect(660, 20, 421, 61));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\260\217\346\240\207\345\256\213\347\256\200\344\275\223"));
        pushButton_1->setFont(font1);
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(650, 590, 151, 51));
        pushButton_2->setFont(font1);
        pushButton_3 = new QPushButton(centralwidget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(940, 590, 151, 51));
        pushButton_3->setFont(font1);
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(660, 90, 421, 61));
        pushButton_4->setFont(font1);
        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(660, 160, 421, 61));
        pushButton_5->setFont(font1);
        pushButton_6 = new QPushButton(centralwidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(660, 230, 421, 61));
        pushButton_6->setFont(font1);
        pushButton_7 = new QPushButton(centralwidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(660, 300, 421, 61));
        pushButton_7->setFont(font1);
        pushButton_8 = new QPushButton(centralwidget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(660, 370, 421, 61));
        pushButton_8->setFont(font1);
        pushButton_9 = new QPushButton(centralwidget);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(660, 440, 421, 61));
        pushButton_9->setFont(font1);
        pushButton_19 = new QPushButton(centralwidget);
        pushButton_19->setObjectName(QString::fromUtf8("pushButton_19"));
        pushButton_19->setGeometry(QRect(660, 510, 421, 61));
        pushButton_19->setFont(font1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1291, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_bgp->setText(QString());
        pushButton_1->setText(QCoreApplication::translate("MainWindow", "\346\237\245\346\211\276\346\237\220\344\272\272\346\211\200\346\234\211\345\255\251\345\255\220", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\346\237\245\346\211\276\346\237\220\344\272\272\346\211\200\346\234\211\347\245\226\345\205\210", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "\347\224\250\346\213\254\345\217\267\350\241\250\347\244\272\346\263\225\350\276\223\345\207\272\344\272\214\345\217\211\346\240\221", nullptr));
        pushButton_6->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\207\272\345\256\266\350\260\261\345\205\250\351\203\250\350\256\260\345\275\225", nullptr));
        pushButton_7->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\345\256\266\350\260\261\345\205\250\351\203\250\350\256\260\345\275\225", nullptr));
        pushButton_8->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\344\270\200\344\270\252\350\256\260\345\275\225", nullptr));
        pushButton_9->setText(QCoreApplication::translate("MainWindow", "\346\237\245\346\211\276\346\210\220\345\221\230", nullptr));
        pushButton_19->setText(QCoreApplication::translate("MainWindow", "\345\256\266\350\260\261\346\240\221\345\261\225\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

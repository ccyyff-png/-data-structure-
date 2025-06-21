/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_login
{
public:
    QLabel *user_text;
    QLabel *password_text;
    QLineEdit *lineEdit_user;
    QLineEdit *lineEdit_password;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLabel *label_background;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *user_icon;
    QLabel *password_icon;
    QLabel *label_7;
    QLabel *button_label;

    void setupUi(QWidget *login)
    {
        if (login->objectName().isEmpty())
            login->setObjectName(QString::fromUtf8("login"));
        login->resize(1900, 950);
        user_text = new QLabel(login);
        user_text->setObjectName(QString::fromUtf8("user_text"));
        user_text->setGeometry(QRect(820, 170, 111, 61));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        user_text->setPalette(palette);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\260\217\346\240\207\345\256\213\347\256\200\344\275\223"));
        font.setPointSize(12);
        user_text->setFont(font);
        password_text = new QLabel(login);
        password_text->setObjectName(QString::fromUtf8("password_text"));
        password_text->setGeometry(QRect(820, 310, 120, 31));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        password_text->setPalette(palette1);
        password_text->setFont(font);
        lineEdit_user = new QLineEdit(login);
        lineEdit_user->setObjectName(QString::fromUtf8("lineEdit_user"));
        lineEdit_user->setGeometry(QRect(950, 170, 201, 51));
        lineEdit_password = new QLineEdit(login);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(950, 300, 201, 51));
        pushButton = new QPushButton(login);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(770, 490, 141, 61));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush2(QColor(0, 61, 134, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush2);
        QBrush brush3(QColor(255, 255, 255, 128));
        brush3.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::Active, QPalette::PlaceholderText, brush3);
#endif
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        QBrush brush4(QColor(255, 255, 255, 128));
        brush4.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush4);
#endif
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        QBrush brush5(QColor(255, 255, 255, 128));
        brush5.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette2.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush5);
#endif
        pushButton->setPalette(palette2);
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\260\217\346\240\207\345\256\213\347\256\200\344\275\223"));
        font1.setPointSize(10);
        pushButton->setFont(font1);
        pushButton->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 61, 134);\n"
"color: rgb(255, 255, 255);"));
        pushButton->setAutoDefault(false);
        pushButton_2 = new QPushButton(login);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(1020, 490, 141, 61));
        pushButton_2->setFont(font1);
        pushButton_2->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"background-color: rgb(0, 61, 134);"));
        label_background = new QLabel(login);
        label_background->setObjectName(QString::fromUtf8("label_background"));
        label_background->setGeometry(QRect(-20, -60, 1411, 761));
        QFont font2;
        font2.setPointSize(6);
        label_background->setFont(font2);
        label_3 = new QLabel(login);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(830, 80, 291, 41));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_3->setPalette(palette3);
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\226\271\346\255\243\345\260\217\346\240\207\345\256\213\347\256\200\344\275\223"));
        font3.setPointSize(17);
        label_3->setFont(font3);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_4 = new QLabel(login);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 10, 321, 101));
        user_icon = new QLabel(login);
        user_icon->setObjectName(QString::fromUtf8("user_icon"));
        user_icon->setGeometry(QRect(760, 180, 41, 41));
        user_icon->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        password_icon = new QLabel(login);
        password_icon->setObjectName(QString::fromUtf8("password_icon"));
        password_icon->setGeometry(QRect(760, 300, 41, 51));
        password_icon->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        label_7 = new QLabel(login);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(720, 50, 491, 591));
        label_7->setStyleSheet(QString::fromUtf8("\n"
"background-color: rgb(0, 0, 0,90);"));
        button_label = new QLabel(login);
        button_label->setObjectName(QString::fromUtf8("button_label"));
        button_label->setGeometry(QRect(560, 680, 171, 20));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush6(QColor(155, 155, 155, 90));
        brush6.setStyle(Qt::SolidPattern);
        palette4.setBrush(QPalette::Active, QPalette::Button, brush6);
        palette4.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush6);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Button, brush6);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush6);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        palette4.setBrush(QPalette::Disabled, QPalette::Button, brush6);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush6);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush6);
        button_label->setPalette(palette4);
        button_label->setFont(font1);
        button_label->setStyleSheet(QString::fromUtf8("background-color: rgb(155, 155, 155,90);"));
        label_background->raise();
        label_7->raise();
        user_text->raise();
        password_text->raise();
        lineEdit_user->raise();
        lineEdit_password->raise();
        pushButton->raise();
        pushButton_2->raise();
        label_3->raise();
        label_4->raise();
        user_icon->raise();
        password_icon->raise();
        button_label->raise();

        retranslateUi(login);

        QMetaObject::connectSlotsByName(login);
    } // setupUi

    void retranslateUi(QWidget *login)
    {
        login->setWindowTitle(QCoreApplication::translate("login", "Dialog", nullptr));
        user_text->setText(QCoreApplication::translate("login", "\347\256\241\347\220\206\345\221\230\350\264\246\345\217\267\357\274\232", nullptr));
        password_text->setText(QCoreApplication::translate("login", "\347\256\241\347\220\206\345\221\230\345\257\206\347\240\201\357\274\232", nullptr));
        pushButton->setText(QCoreApplication::translate("login", "\347\231\273\345\275\225", nullptr));
        pushButton_2->setText(QCoreApplication::translate("login", "\351\200\200\345\207\272", nullptr));
        label_background->setText(QString());
        label_3->setText(QCoreApplication::translate("login", "\346\254\242\350\277\216\344\275\277\347\224\250\345\256\266\350\260\261\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        label_4->setText(QCoreApplication::translate("login", "<html><head/><body><p><img src=\":/logo.png\"/></p></body></html>", nullptr));
        user_icon->setText(QCoreApplication::translate("login", "<html><head/><body><p><img src=\":/user1.png\"/></p></body></html>", nullptr));
        password_icon->setText(QCoreApplication::translate("login", "<html><head/><body><p><img src=\":/pass1.png\"/></p></body></html>", nullptr));
        label_7->setText(QString());
        button_label->setText(QCoreApplication::translate("login", " Copyright \302\251 \345\216\246\351\227\250\345\244\247\345\255\246", nullptr));
    } // retranslateUi

};

namespace Ui {
    class login: public Ui_login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H

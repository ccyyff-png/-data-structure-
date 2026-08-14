#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>

class QLineEdit;

// 账号设置对话框：验证当前密码后可修改用户名与密码
class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDialog(const QString& currentUser, QWidget* parent = nullptr);

    QString currentPassword() const;
    QString newUser() const;
    QString newPassword() const;
    QString confirmPassword() const;

private:
    QLineEdit* m_currentPwd = nullptr;
    QLineEdit* m_newUser = nullptr;
    QLineEdit* m_newPwd = nullptr;
    QLineEdit* m_confirmPwd = nullptr;
};

#endif // ACCOUNTDIALOG_H

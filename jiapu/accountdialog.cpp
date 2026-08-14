#include "accountdialog.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

AccountDialog::AccountDialog(const QString& currentUser, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("账号设置");

    auto* lay = new QVBoxLayout(this);
    auto* form = new QFormLayout;

    auto* currentLabel = new QLabel(currentUser);
    currentLabel->setStyleSheet("color: #898781;");
    form->addRow("当前用户名：", currentLabel);

    m_currentPwd = new QLineEdit;
    m_currentPwd->setEchoMode(QLineEdit::Password);
    m_currentPwd->setPlaceholderText("修改前需验证当前密码");
    form->addRow("当前密码：", m_currentPwd);

    m_newUser = new QLineEdit(currentUser);
    form->addRow("新用户名：", m_newUser);

    m_newPwd = new QLineEdit;
    m_newPwd->setEchoMode(QLineEdit::Password);
    form->addRow("新密码：", m_newPwd);

    m_confirmPwd = new QLineEdit;
    m_confirmPwd->setEchoMode(QLineEdit::Password);
    form->addRow("确认新密码：", m_confirmPwd);
    lay->addLayout(form);

    auto* tip = new QLabel("提示：密码仅以 SHA-256 哈希保存在程序目录 config.ini 中。");
    tip->setWordWrap(true);
    tip->setStyleSheet("color: #898781; font-size: 12px;");
    lay->addWidget(tip);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("保存");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    lay->addWidget(buttons);

    resize(400, 240);
}

QString AccountDialog::currentPassword() const { return m_currentPwd->text(); }
QString AccountDialog::newUser() const         { return m_newUser->text().trimmed(); }
QString AccountDialog::newPassword() const     { return m_newPwd->text(); }
QString AccountDialog::confirmPassword() const { return m_confirmPwd->text(); }

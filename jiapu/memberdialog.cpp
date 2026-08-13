#include "memberdialog.h"
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

MemberDialog::MemberDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("添加成员");

    auto* lay = new QVBoxLayout(this);
    auto* form = new QFormLayout;
    m_fatherEdit = new QLineEdit;
    m_fatherEdit->setPlaceholderText("必须是家谱中已有的成员");
    m_wifeEdit = new QLineEdit;
    m_sonEdit = new QLineEdit;
    form->addRow("父亲姓名：", m_fatherEdit);
    form->addRow("母亲姓名：", m_wifeEdit);
    form->addRow("儿子姓名：", m_sonEdit);
    lay->addLayout(form);

    auto* tip = new QLabel("提示：父亲必须是家谱中已有成员；儿子重名、姓名含逗号、母亲与现有记录不一致时会被拒绝。");
    tip->setWordWrap(true);
    tip->setStyleSheet("color: #898781; font-size: 12px;");
    lay->addWidget(tip);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("添加");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    lay->addWidget(buttons);

    resize(380, 200);
}

QString MemberDialog::father() const { return m_fatherEdit->text().trimmed(); }
QString MemberDialog::wife()   const { return m_wifeEdit->text().trimmed(); }
QString MemberDialog::son()    const { return m_sonEdit->text().trimmed(); }

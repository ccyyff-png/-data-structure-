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

// ---------------- MemberEditDialog ----------------

MemberEditDialog::MemberEditDialog(const MemberInfo& info, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QString("编辑成员「%1」").arg(info.name));

    auto* lay = new QVBoxLayout(this);
    auto* form = new QFormLayout;

    m_nameEdit = new QLineEdit(info.name);
    form->addRow("姓名：", m_nameEdit);

    m_spouseEdit = new QLineEdit(info.spouse);
    m_spouseEdit->setPlaceholderText("无");
    form->addRow(info.male ? "妻子姓名：" : "丈夫姓名：", m_spouseEdit);

    m_fatherEdit = new QLineEdit(info.father);
    m_fatherEdit->setPlaceholderText(info.isRoot ? "家谱始祖" : "必须是家谱中已有成员");
    form->addRow("父亲姓名：", m_fatherEdit);
    lay->addLayout(form);

    auto* tip = new QLabel(QString("第 %1 代　·　%2。修改姓名/配偶为直接重命名；修改父亲将重新挂接亲子关系。")
                               .arg(info.generation + 1)
                               .arg(info.male ? "男" : "女"));
    tip->setWordWrap(true);
    tip->setStyleSheet("color: #898781; font-size: 12px;");
    lay->addWidget(tip);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("保存");
    buttons->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    lay->addWidget(buttons);

    resize(400, 230);
}

QString MemberEditDialog::newName() const    { return m_nameEdit->text().trimmed(); }
QString MemberEditDialog::newSpouse() const  { return m_spouseEdit->text().trimmed(); }
QString MemberEditDialog::newFather() const  { return m_fatherEdit->text().trimmed(); }

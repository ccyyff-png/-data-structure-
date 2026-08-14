#ifndef MEMBERDIALOG_H
#define MEMBERDIALOG_H

#include <QDialog>
#include "familytree.h"

class QLineEdit;

// 添加成员对话框（表单：父亲 / 母亲 / 儿子）
// 校验失败时由调用方再次 exec()，输入内容得以保留
class MemberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemberDialog(QWidget* parent = nullptr);

    QString father() const;
    QString wife() const;
    QString son() const;

private:
    QLineEdit* m_fatherEdit = nullptr;
    QLineEdit* m_wifeEdit = nullptr;
    QLineEdit* m_sonEdit = nullptr;
};

// 编辑成员对话框（直观修改：姓名 / 配偶姓名 / 父亲）
class MemberEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemberEditDialog(const MemberInfo& info, QWidget* parent = nullptr);

    QString newName() const;    // 新姓名（重命名本人）
    QString newSpouse() const;  // 新配偶姓名（重命名配偶）
    QString newFather() const;  // 新父亲（改挂关系）

private:
    QLineEdit* m_nameEdit = nullptr;
    QLineEdit* m_spouseEdit = nullptr;
    QLineEdit* m_fatherEdit = nullptr;
};

#endif // MEMBERDIALOG_H

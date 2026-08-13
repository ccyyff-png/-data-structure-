#ifndef MEMBERDIALOG_H
#define MEMBERDIALOG_H

#include <QDialog>

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

#endif // MEMBERDIALOG_H

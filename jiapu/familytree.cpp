#include "familytree.h"

// 查找家谱数组中指定父亲节点的位置
int FindFamNode(FamType fam[], int n, char *name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(fam[i].father, name) == 0) {
            return i;
        }
    }
    return -1;
}

// 创建家谱二叉树
BTree* CreatBTree(char *root, FamType fam[], int n) {
    if (root == NULL || strlen(root) == 0) {
        return NULL; // 空节点处理
    }

    BTree *bt = (BTree*)malloc(sizeof(BTree)); // 创建当前节点
    strcpy(bt->name, root);
    bt->lchild = bt->rchild = NULL;

    int i = FindFamNode(fam, n, root); // 查找当前节点在fam中的位置
    if (i != -1) {
        // 创建母亲节点
        if (strlen(fam[i].wife) > 0) {
            BTree *p = (BTree*)malloc(sizeof(BTree));
            strcpy(p->name, fam[i].wife);
            p->lchild = p->rchild = NULL;
            bt->lchild = p;


            for (int j = 0; j < n; j++) {
                if (strcmp(fam[j].father, root) == 0 && strlen(fam[j].son) > 0) {
                    p->rchild = CreatBTree(fam[j].son, fam, n);
                    p = p->rchild;
                }
            }
        }
    }
    return bt;
}

// 读取txt文档的家谱信息并创建二叉树
BTree* ReadTxtAndCreateTree(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filePath;
        return nullptr;
    }

    QTextStream in(&file);
    FamType fam[MaxSize];
    int n = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() == 3) {
            strncpy(fam[n].father, parts[0].toStdString().c_str(), MaxSize);
            strncpy(fam[n].wife, parts[1].toStdString().c_str(), MaxSize);
            strncpy(fam[n].son, parts[2].toStdString().c_str(), MaxSize);
            n++;
        }
    }
    file.close();

    if (n > 0) {
        return CreatBTree(fam[0].father, fam, n); // 调用CreatBTree函数
    }
    return nullptr;
}

// 查找指定节点
BTree* FindNode(BTree *bt, const char *name) {
    if (bt == NULL) {
        return NULL;
    } else if (strcmp(bt->name, name) == 0) {
        return bt;
    } else {
        BTree *p = FindNode(bt->lchild, name);
        if (p != NULL) {
            return p;
        } else {
            return FindNode(bt->rchild, name);
        }
    }
}

// 查找某人所有祖先（返回QString类型）
QString FindAllAncestors(BTree* bt, const QString& name) {
    BTree* node = FindNode(bt, name.toStdString().c_str());
    if (!node) {
        return "查无此人";
    }

    QString result = "所有祖先: ";
    BTree* St[MaxSize];
    int top = -1;
    BTree* current = bt;

    // 遍历二叉树，记录路径
    while (current || top != -1) {
        while (current) {
            St[++top] = current; // 入栈
            current = current->lchild; // 向左（母亲）遍历
        }
        if (top != -1) {
            current = St[top--]; // 出栈
            if (current == node) {
                // 找到目标节点，输出栈中路径
                for (int i = 0; i <= top; i++) {
                    result += QString(St[i]->name) + " ";
                }
                return result;
            }
            current = current->rchild; // 向右（儿子）遍历
        }
    }
    return "无祖先信息";
}

// 查找某人所有儿子（返回QString类型）
QString FindAllSons(BTree* bt, const QString& name) {
    BTree* node = FindNode(bt, name.toStdString().c_str());
    if (!node) {
        return "查无此人";
    }

    QString result = "所有儿子: ";
    // 直接通过母亲节点获取儿子
    BTree* p = node->lchild ? node->lchild->rchild : node->rchild;
    if (p) {
        while (p) {
            result += QString(p->name) + " ";
            p = p->rchild;
        }
    } else {
        result += "无";
    }
    return result;
}

// 清除家谱文件全部记录
QString ClearFamilyRecords(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return "无法清除家谱文件记录";
    }
    file.close();
    return "家谱文件记录已清除";
}

// 输出家谱文件全部记录（返回QString类型）
QString OutputFamilyRecords(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "无法读取家谱文件记录";
    }

    QTextStream in(&file);
    QString result = "父亲   母亲   儿子\n------------------------------\n";
    while (!in.atEnd()) {//判断是否达到了文件末尾
        QString line = in.readLine();
        result += line + "\n";
    }
    file.close();
    result += "------------------------------";
    return result;
}

// 用括号表示法输出二叉树（返回QString类型）
QString DispTreeInBracketFormat(BTree* bt) {
    QString result;
    QTextStream stream(&result);
    stream.setCodec("UTF-8"); // 设置编码为UTF-8

    std::function<void(BTree*)> helper = [&](BTree* b) {
        if (b) {
            stream << QString::fromUtf8(b->name); // 使用fromUtf8确保正确编码
            if (b->lchild || b->rchild) {
                stream << "(";
                helper(b->lchild);
                if (b->rchild && b->lchild) {
                    stream << ","; // 仅当左右子树都存在时才输出逗号
                }
                helper(b->rchild);
                stream << ")";
            }
        }
    };

    helper(bt);
    return result;
}
//家谱树展示
QString DisplayFamilyTree(BTree* bt)
{
    QString result;
    QTextStream stream(&result);
    stream.setCodec("UTF-8");

    std::function<void(BTree*, int, bool, bool)> helper = [&](BTree* node, int depth, bool isLast, bool isWife) {
        if (node == nullptr) return;

        // 生成缩进
        for (int i = 0; i < depth; ++i) {
            stream << "  ";
        }

        // 添加箭头
        if (depth > 0) {
            if (isWife) { // 母亲节点
                if (isLast) {
                    stream << QString::fromUtf8("└← "); // 母亲节点且为最后一个子节点
                } else {
                    stream << QString::fromUtf8("├← "); // 母亲节点且非最后一个子节点
                }
            } else { // 父亲节点
                if (isLast) {
                    stream << QString::fromUtf8("└→ "); // 父亲节点且为最后一个子节点
                } else {
                    stream << QString::fromUtf8("├→ "); // 父亲节点且非最后一个子节点
                }
            }
        }

        // 确保正确转换和输出节点名称
        QString nodeName = QString::fromUtf8(node->name);
        stream << nodeName << "\n";

        // 递归处理子节点
        BTree* child = node->lchild;
        int count = 0;
        while (child != nullptr) {
            bool isLastChild = (child->rchild == nullptr);
            helper(child, depth + 1, isLastChild, count == 0); // 第一个子节点视为母亲节点
            child = child->rchild;
            count++;
        }
    };

    helper(bt, 0, true, false);
    return result;
}

// 添加一个记录
QString AddRecordToFile(const QString& filePath, const QString& father, const QString& wife, const QString& son) {
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return "无法添加记录到家谱文件";
    }

    QTextStream out(&file);
    // 确保在写入新记录前添加换行符（如果文件非空）
    if (file.size() > 0) {
        out << Qt::endl; // 在新记录前添加换行符
    }
    out << father << "," << wife << "," << son << Qt::endl; // 使用 Qt::endl 确保换行和刷新
    file.close();
    return "记录已添加";
}

// 按照姓名查找成员信息，如果找到，返回有，没有返回无（返回QString类型）
QString FindMemberInfo(BTree* bt, const QString& name) {
    BTree* node = FindNode(bt, name.toStdString().c_str());
    return node ? "存在此人" : "不存在此人";
}

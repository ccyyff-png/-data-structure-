#ifndef FAMILYTREE_H
#define FAMILYTREE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include<iostream>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QTextStream>

#include <QInputDialog>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <QTextStream>
#include <QDebug>

#define MaxSize 100	//代表了姓名字符、最大场宽、数组元素树
using namespace std;
typedef struct fnode
{
    char father[MaxSize];	//父
    char wife[MaxSize];	//母
    char son[MaxSize];	//子
} FamType;
typedef struct tnode//定义二叉树节点结构，代表家谱中的每个成员
{
    char name[MaxSize];
    struct tnode *lchild,*rchild;
} BTree;

Q_DECLARE_METATYPE(BTree*);
Q_DECLARE_METATYPE(FamType);

int FindFamNode(FamType fam[], int n, char *name);
BTree* CreatBTree(char *root, FamType fam[], int n);
BTree* ReadTxtAndCreateTree(const QString& filePath);
BTree* FindNode(BTree *bt, const char *name);
QString FindAllAncestors(BTree* bt, const QString& name);
QString FindAllSons(BTree* bt, const QString& name);
QString ClearFamilyRecords(const QString& filePath);
QString OutputFamilyRecords(const QString& filePath);
QString DispTreeInBracketFormat(BTree* bt);
QString AddRecordToFile(const QString& filePath, const QString& father, const QString& wife, const QString& son);
QString FindMemberInfo(BTree* bt, const QString& name);
QString DisplayFamilyTree(BTree* bt);









#endif // FAMILYTREE_H

#ifndef AUTH_H
#define AUTH_H

#include <QString>

/**
 * 账号认证模块：凭据存于 ini 文件（仅保存 SHA-256 哈希），
 * 首次运行自动播种默认账号 admin / 12345。
 * 所有函数接受 ini 路径参数，便于单元测试。
 */

// 默认凭据文件路径（程序目录 config.ini）
QString authFilePath();
// 当前用户名
QString currentUser(const QString& iniPath);
// 校验用户名+密码（首次运行自动播种默认凭据）
bool verifyPassword(const QString& iniPath, const QString& user, const QString& pwd);
// 修改用户名与密码（需先验证当前密码）；成功返回空串，失败返回中文错误信息
QString changeCredentials(const QString& iniPath, const QString& user, const QString& pwd,
                          const QString& newUser, const QString& newPwd);

#endif // AUTH_H

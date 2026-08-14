#include "auth.h"
#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>

static QString hashPassword(const QString& pwd)
{
    return QString::fromLatin1(
        QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256).toHex());
}

static void seedIfNeeded(QSettings& settings)
{
    if (!settings.contains("auth/user") || !settings.contains("auth/passhash")) {
        settings.setValue("auth/user", "admin");
        settings.setValue("auth/passhash", hashPassword("12345"));
        settings.sync();
    }
}

QString authFilePath()
{
    // 用户数据目录（%APPDATA%/jiapu），不依赖程序所在路径可写
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    // 旧版本凭据文件（程序目录 config.ini）迁移
    const QString legacy = QCoreApplication::applicationDirPath() + "/config.ini";
    const QString current = dir + "/config.ini";
    if (!QFile::exists(current) && QFile::exists(legacy))
        QFile::copy(legacy, current);
    return current;
}

QString currentUser(const QString& iniPath)
{
    QSettings settings(iniPath, QSettings::IniFormat);
    seedIfNeeded(settings);
    return settings.value("auth/user").toString();
}

bool verifyPassword(const QString& iniPath, const QString& user, const QString& pwd)
{
    QSettings settings(iniPath, QSettings::IniFormat);
    seedIfNeeded(settings);
    return user == settings.value("auth/user").toString()
        && hashPassword(pwd) == settings.value("auth/passhash").toString();
}

QString changeCredentials(const QString& iniPath, const QString& user, const QString& pwd,
                          const QString& newUser, const QString& newPwd)
{
    if (!verifyPassword(iniPath, user, pwd))
        return "当前密码错误";
    const QString nu = newUser.trimmed();
    if (nu.isEmpty())
        return "新用户名不能为空";
    if (nu.contains(',') || nu.contains('\n') || nu.contains('\r'))
        return "用户名不能包含逗号或换行";
    if (newPwd.isEmpty())
        return "新密码不能为空";

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.setValue("auth/user", nu);
    settings.setValue("auth/passhash", hashPassword(newPwd));
    settings.sync();
    return {};
}

#ifndef USERROLEHELPER_H
#define USERROLEHELPER_H
#include <QString>
#include <QMap>

enum class UserRole
{
    Unknown,
    Administrator,
    Seller,
    Buyer,
    Courier
};

class UserRoleHelper
{
public:
    static const QMap<UserRole, QString>& getRoleNames();
    static UserRole fromString(const QString &roleStr);
    static QString toString(UserRole role);
};

#endif // USERROLEHELPER_H

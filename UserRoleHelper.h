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
};

#endif // USERROLEHELPER_H

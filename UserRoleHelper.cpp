#include "UserRoleHelper.h"

const QMap<UserRole, QString>& UserRoleHelper::getRoleNames()
{
    static const QMap<UserRole, QString> roleNames = {
        {UserRole::Unknown, "Посетитель"},
        {UserRole::Administrator, "Администратор"},
        {UserRole::Seller, "Продавец"},
        {UserRole::Buyer, "Покупатель"},
        {UserRole::Courier, "Курьер"}
    };
    return roleNames;
}

const UserRole UserRoleHelper::fromString(const QString &roleStr)
{
    static const QMap<QString, UserRole> roleMap = {
        {"Посетитель", UserRole::Unknown},
        {"Администратор", UserRole::Administrator},
        {"Продавец", UserRole::Seller},
        {"Покупатель", UserRole::Buyer},
        {"Курьер", UserRole::Courier}
    };
    return roleMap.value(roleStr, UserRole::Unknown);
}

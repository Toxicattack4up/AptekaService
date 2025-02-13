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

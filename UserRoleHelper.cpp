#include "UserRoleHelper.h"

const QMap<UserRole, QString>& UserRoleHelper::getRoleNames() {
    static const QMap<UserRole, QString> roleNames = {
        {UserRole::Unknown, "Посетитель"},
        {UserRole::Administrator, "Администратор"},
        {UserRole::Seller, "Продавец"},
        {UserRole::Buyer, "Покупатель"},
        {UserRole::Courier, "Курьер"}
    };
    return roleNames;
}

UserRole UserRoleHelper::fromString(const QString& roleStr) {
    static const QMap<QString, UserRole> roleMap = {
        {"Посетитель", UserRole::Unknown},
        {"Администратор", UserRole::Administrator},
        {"Продавец", UserRole::Seller},
        {"Покупатель", UserRole::Buyer},
        {"Курьер", UserRole::Courier}
    };
    return roleMap.value(roleStr, UserRole::Unknown); // По умолчанию Unknown
}

QString UserRoleHelper::toString(UserRole role) {
    switch (role) {
    case UserRole::Administrator: return "Администратор";
    case UserRole::Seller: return "Продавец";
    case UserRole::Buyer: return "Покупатель"; // Исправлено с "Посетитель"
    case UserRole::Courier: return "Курьер";
    default: return "Посетитель"; // Для Unknown и других случаев
    }
}

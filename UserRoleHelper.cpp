#include "UserRoleHelper.h"

const QMap<UserRole, QString>& UserRoleHelper::getRoleNames() {
    static const QMap<UserRole, QString> roleNames = {
        {UserRole::Unknown, "Неизвестно"},
        {UserRole::Administrator, "Администратор"},
        {UserRole::Seller, "Продавец"},
        {UserRole::Courier, "Курьер"},
        {UserRole::Buyer, "Покупатель"}
    };
    return roleNames;
}

UserRole UserRoleHelper::fromString(const QString& roleStr) {
    const QMap<UserRole, QString>& roleNames = getRoleNames();
    for (auto it = roleNames.constBegin(); it != roleNames.constEnd(); ++it) {
        if (it.value() == roleStr) {
            return it.key();
        }
    }
    return UserRole::Unknown;
}

QString UserRoleHelper::toString(UserRole role) {
    const QMap<UserRole, QString>& roleNames = getRoleNames();
    return roleNames.value(role, "Неизвестно");
}

#include "UserRoleHelper.h"

// Получение карты ролей: возвращает статическую карту соответствия ролей и их названий
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

// Преобразование строки в роль: конвертирует строковое название в UserRole
UserRole UserRoleHelper::fromString(const QString& roleStr) {
    const QMap<UserRole, QString>& roleNames = getRoleNames();
    for (auto it = roleNames.constBegin(); it != roleNames.constEnd(); ++it) {
        if (it.value() == roleStr) {
            return it.key();
        }
    }
    return UserRole::Unknown;
}

// Преобразование роли в строку: конвертирует UserRole в строковое название
QString UserRoleHelper::toString(UserRole role) {
    const QMap<UserRole, QString>& roleNames = getRoleNames();
    return roleNames.value(role, "Неизвестно");
}

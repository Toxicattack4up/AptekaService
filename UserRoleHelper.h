#ifndef USERROLEHELPER_H
#define USERROLEHELPER_H

#include <QString>
#include <QMap>

enum class UserRole {
    Unknown,       // Неизвестная роль (гость)
    Administrator, // Администратор
    Seller,        // Продавец
    Buyer,         // Покупатель
    Courier        // Курьер
};

class UserRoleHelper {
public:
    // Возвращает карту соответствия ролей и их строковых названий
    static const QMap<UserRole, QString>& getRoleNames();

    // Преобразует строковое название роли в значение UserRole
    static UserRole fromString(const QString& roleStr);

    // Преобразует значение UserRole в строковое название
    static QString toString(UserRole role);
};

#endif // USERROLEHELPER_H

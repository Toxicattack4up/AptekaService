#ifndef USER_H
#define USER_H

#include "UserRoleHelper.h"
#include <QDateTime>
#include <QString>

class User {
protected:
    UserRole Role;         // Роль пользователя
    QString Login;         // Логин пользователя
    QString Password;      // Пароль пользователя
    QString FullName;      // Полное имя пользователя
    QString Email;         // Электронная почта пользователя
    QDateTime RegistrationDate; // Дата регистрации

public:
    // Конструктор по умолчанию
    User();

    // Конструктор с параметрами
    User(UserRole Role, const QString& Login, const QString& Password,
         const QString& FullName, const QString& Email);

    virtual ~User() = default; // Виртуальный деструктор для наследования

    // Геттеры: возвращают значения полей
    UserRole getRole() const;
    QString getLogin() const;
    QString getPassword() const;
    QString getFullName() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;

    // Сеттеры: устанавливают новые значения с проверкой корректности
    void setRole(UserRole _role);
    void setLogin(const QString& _login);
    void setPassword(const QString& _Password);
    void setFullName(const QString& _fullName);
    void setEmail(const QString& _email);
    void setRegistrationDate(const QDateTime& _registrationDate); // Исправлена опечатка
};

#endif // USER_H

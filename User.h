#ifndef USER_H
#define USER_H

#include "UserRoleHelper.h"
#include <QDateTime>
#include <QString>

class User {
protected:
    UserRole Role;
    QString Login;
    QString PasswordHash;
    QString FullName;
    QString Email;
    QDateTime RegistrationDate;

public:
    User();
    User(UserRole Role,
         const QString& Login,
         const QString& PasswordHash,
         const QString& FullName,
         const QString& Email);

    virtual ~User() = default; // Можно оставить виртуальный деструктор для корректного удаления

    // Геттеры
    UserRole getRole() const;
    QString getLogin() const;
    QString getPasswordHash() const;
    QString getFullName() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;

    // Сеттеры
    void setRole(UserRole _role);
    void setLogin(const QString& _login);
    void setPasswordHash(const QString& _passwordHash);
    void setFullName(const QString& _fullName);
    void setEmail(const QString& _email);
    void setRegistrationDate(const QDateTime& _registationDate);
};

#endif // USER_H

#ifndef USER_H
#define USER_H

#include "UserRoleHelper.h"
#include <QDateTime>


class User : public UserRoleHelper
{
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

    virtual ~User() = default;

    // Геттеры
    UserRole getRole() const;
    QString getLogin() const;
    QString getPasswordHash() const;
    QString getFullName() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;

    // Сеттеры
    void setRole(UserRole Role);
    void setLogin(const QString& Login);
    void setPasswordHash(const QString& PasswordHash);
    void setFullName(const QString& FullName);
    void setEmail(const QString& Email);
    void setRegistrationDate(const QDateTime& RegistationDate);
};

#endif // USER_H

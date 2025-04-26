#ifndef USER_H
#define USER_H

#include <QString>
#include <QDate>
#include "UserRoleHelper.h"

class User {
public:
    User();
    User(UserRole role, const QString& login, const QString& password,
         const QString& fullName, const QString& email);
    UserRole getRole() const;
    QString getLogin() const;
    QString getPassword() const;
    QString getFullName() const;
    QString getEmail() const;
    QDate getRegistrationDate() const;
    void setRole(UserRole _role);
    void setLogin(const QString& _login);
    void setPassword(const QString& _password);
    void setFullName(const QString& _fullName);
    void setEmail(const QString& _email);
    void setRegistrationDate(const QDate& _registrationDate);

private:
    UserRole Role;
    QString Login;
    QString Password;
    QString FullName;
    QString Email;
    QDate RegistrationDate;
};

#endif // USER_H

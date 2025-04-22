#ifndef USER_H
#define USER_H

#include "UserRoleHelper.h"
#include <QString>
#include <QDateTime>
#include <QDebug>

class User : public UserRoleHelper {
public:
    User();
    User(UserRole Role, const QString& Login, const QString& Password,
         const QString& FullName, const QString& Email);
    UserRole getRole() const;
    QString getLogin() const;
    QString getPassword() const;
    QString getFullName() const;
    QString getEmail() const;
    QDateTime getRegistrationDate() const;
    void setRole(UserRole _role);
    void setLogin(const QString& _login);
    void setPassword(const QString& _Password);
    void setFullName(const QString& _fullName);
    void setEmail(const QString& _email);
    void setRegistrationDate(const QDateTime& _registrationDate);

protected:
    UserRole Role;
    QString Login;
    QString Password;
    QString FullName;
    QString Email;
    QDateTime RegistrationDate;
};

#endif // USER_H

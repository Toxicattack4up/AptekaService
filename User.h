#ifndef USER_H
#define USER_H

#include <QString>
#include <QDate>
#include "UserRoleHelper.h"

class User {
public:
    User();
    User(UserRole Role, const QString& Login, const QString& Password,
         const QString& FullName, const QString& Email);

    UserRole getRole() const;
    QString getLogin() const;
    QString getPassword() const;
    QString getFullName() const;
    QString getEmail() const;
    QDate getRegistrationDate() const;
    int getPharmacyId() const;

    void setRole(UserRole _role);
    void setLogin(const QString& _login);
    void setPassword(const QString& _password);
    void setFullName(const QString& _fullName);
    void setEmail(const QString& _email);
    void setRegistrationDate(const QDate& _registrationDate);
    void setPharmacyId(int id);

private:
    UserRole Role;
    QString Login;
    QString Password;
    QString FullName;
    QString Email;
    QDate RegistrationDate;
    int pharmacyId = 0;
};

#endif // USER_H

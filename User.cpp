#include "User.h"

#include <QDebug>

User::User()
    : Role(UserRole::Unknown),
    Login(""),
    Password(""),
    FullName(""),
    Email(""),
    RegistrationDate(QDateTime::currentDateTime()) {}

User::User(UserRole Role, const QString& Login, const QString& Password,
           const QString& FullName, const QString& Email)
    : Role(Role),
    Login(Login),
    Password(Password),
    FullName(FullName),
    Email(Email),
    RegistrationDate(QDateTime::currentDateTime()) {}

UserRole User::getRole() const {
    return Role;
}

QString User::getLogin() const {
    return Login;
}

QString User::getPassword() const {
    return Password;
}

QString User::getFullName() const {
    return FullName;
}

QString User::getEmail() const {
    return Email;
}

QDateTime User::getRegistrationDate() const {
    return RegistrationDate;
}

void User::setRole(UserRole _role) {
    Role = _role;
}

void User::setLogin(const QString& _login) {
    if (_login.isEmpty()) {
        qDebug() << "Ошибка: Логин не может быть пустым!";
        return;
    }
    Login = _login;
}

void User::setPassword(const QString& _Password) {
    if (_Password.length() < 8) {
        qDebug() << "Ошибка: Пароль должен быть не менее 8 символов!";
        return;
    }
    Password = _Password;
}

void User::setFullName(const QString& _fullName) {
    if (_fullName.isEmpty()) {
        qDebug() << "Ошибка: Полное имя не может быть пустым!";
        return;
    }
    FullName = _fullName;
}

void User::setEmail(const QString& _email) {
    if (_email.isEmpty()) {
        qDebug() << "Ошибка: Почта не может быть пустой!";
        return;
    }
    Email = _email;
}

void User::setRegistrationDate(const QDateTime& _registrationDate) {
    RegistrationDate = _registrationDate;
}

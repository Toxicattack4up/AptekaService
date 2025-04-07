#include "User.h"
#include <QDebug>

// Конструктор по умолчанию: инициализирует поля значениями по умолчанию
User::User()
    : Role(UserRole::Unknown),
    Login(""),
    Password(""),
    FullName(""),
    Email(""),
    RegistrationDate(QDateTime::currentDateTime()) {}

// Конструктор с параметрами: инициализирует поля заданными значениями
User::User(UserRole Role, const QString& Login, const QString& Password,
           const QString& FullName, const QString& Email)
    : Role(Role),
    Login(Login),
    Password(Password),
    FullName(FullName),
    Email(Email),
    RegistrationDate(QDateTime::currentDateTime()) {}

// Геттеры
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

// Сеттеры с проверками
void User::setRole(UserRole _role) {
    Role = _role;
}

void User::setLogin(const QString& _login) {
    if (_login.isEmpty()) {
        qDebug() << "Ошибка: Логин не может быть пустым!";
        return; // Не устанавливаем пустой логин
    }
    Login = _login;
}

void User::setPassword(const QString& _Password) {
    if (_Password.length() < 8) {
        qDebug() << "Ошибка: Пароль должен быть не менее 8 символов!";
        return; // Не устанавливаем короткий пароль
    }
    Password = _Password;
}

void User::setFullName(const QString& _fullName) {
    if (_fullName.isEmpty()) {
        qDebug() << "Ошибка: Полное имя не может быть пустым!";
        return; // Не устанавливаем пустое имя
    }
    FullName = _fullName;
}

void User::setEmail(const QString& _email) {
    if (_email.isEmpty()) {
        qDebug() << "Ошибка: Почта не может быть пустой!";
        return; // Не устанавливаем пустую почту
    }
    Email = _email;
}

void User::setRegistrationDate(const QDateTime& _registrationDate) {
    RegistrationDate = _registrationDate;
}

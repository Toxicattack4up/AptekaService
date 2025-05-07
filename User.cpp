#include "User.h"
#include <QDebug>
#include <QDate>

// Конструктор по умолчанию: создает пользователя с пустыми данными
User::User()
    : Role(UserRole::Unknown),
    Login(""),
    Password(""),
    FullName(""),
    Email(""),
    RegistrationDate(QDate::currentDate()),
    pharmacyId(0) {}

// Конструктор с параметрами: инициализирует пользователя с заданными данными
User::User(UserRole Role, const QString& Login, const QString& Password,
           const QString& FullName, const QString& Email)
    : Role(Role),
    Login(Login),
    Password(Password),
    FullName(FullName),
    Email(Email),
    RegistrationDate(QDate::currentDate()),
    pharmacyId(0) {}

// Получение роли: возвращает роль пользователя
UserRole User::getRole() const {
    return Role;
}

// Получение логина: возвращает логин пользователя
QString User::getLogin() const {
    return Login;
}

// Получение пароля: возвращает пароль пользователя
QString User::getPassword() const {
    return Password;
}

// Получение полного имени: возвращает ФИО пользователя
QString User::getFullName() const {
    return FullName;
}

// Получение email: возвращает адрес электронной почты
QString User::getEmail() const {
    return Email;
}

// Получение даты регистрации: возвращает дату регистрации пользователя
QDate User::getRegistrationDate() const {
    return RegistrationDate;
}

// Получение ID аптеки: возвращает идентификатор аптеки пользователя
int User::getPharmacyId() const {
    return pharmacyId;
}

// Установка роли: изменяет роль пользователя
void User::setRole(UserRole _role) {
    Role = _role;
}

// Установка логина: изменяет логин с проверкой
void User::setLogin(const QString& _login) {
    if (_login.isEmpty()) {
        qDebug() << "Ошибка: Логин не может быть пустым!";
        return;
    }
    Login = _login;
}

// Установка пароля: изменяет пароль с проверкой длины
void User::setPassword(const QString& _Password) {
    if (_Password.length() < 8) {
        qDebug() << "Ошибка: Пароль должен быть не менее 8 символов!";
        return;
    }
    Password = _Password;
}

// Установка полного имени: изменяет ФИО с проверкой
void User::setFullName(const QString& _fullName) {
    if (_fullName.isEmpty()) {
        qDebug() << "Ошибка: Полное имя не может быть пустым!";
        return;
    }
    FullName = _fullName;
}

// Установка email: изменяет email с проверкой
void User::setEmail(const QString& _email) {
    if (_email.isEmpty()) {
        qDebug() << "Ошибка: Почта не может быть пустой!";
        return;
    }
    Email = _email;
}

// Установка даты регистрации: изменяет дату регистрации
void User::setRegistrationDate(const QDate& _registrationDate) {
    RegistrationDate = _registrationDate;
}

// Установка ID аптеки: изменяет идентификатор аптеки с проверкой
void User::setPharmacyId(int id) {
    if (id < 0) {
        qDebug() << "Ошибка: ID аптеки не может быть отрицательным!";
        return;
    }
    pharmacyId = id;
}

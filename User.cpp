#include "User.h"
#include <iostream>
#include <QString>

User::User()
    : Role(UserRole::Unknown),
    Login(""),
    PasswordHash(""),
    FullName(""),
    Email(""),
    RegistrationDate(QDateTime::currentDateTime())
{}

User::User(UserRole Role,
         const QString& Login,
         const QString& PasswordHash,
         const QString& FullName,
         const QString& Email)
        : Role(Role),
        Login(Login),
        PasswordHash(PasswordHash),
        FullName(FullName),
        Email(Email) {}

// Геттеры
UserRole User::getRole() const
{
    return Role;
}
QString User::getLogin() const
{
    return Login;
}
QString User::getPasswordHash() const
{
    return PasswordHash;
}
QString User::getFullName() const
{
    return FullName;
}
QString User::getEmail() const
{
    return Email;
}
QDateTime User::getRegistrationDate() const
{
    return RegistrationDate;
}


// Сеттеры
void User::setRole(UserRole _role)
{
    Role = _role;
}

void User::setLogin(const QString& _login)
{
    Login = _login;
}

void User::setPasswordHash(const QString& _passwordHash)
{
    if (_passwordHash.length() < 8)
    {
        throw std::invalid_argument("Пароль должен быть не менее 8 символов");
    } else {
        std::cout << "Пароль надежный" << std::endl;
    }
    PasswordHash = _passwordHash;
}

void User::setFullName(const QString& _fullName)
{
    if (_fullName.isEmpty())
    {
        throw std::invalid_argument("Имя не может быть пустым");
    }
    FullName = _fullName;
}

void User::setEmail(const QString& _email)
{
    if (_email.isEmpty())
    {
        throw std::invalid_argument("Почта не может быть пустой");
    }
    Email = _email;
}

void User::setRegistrationDate(const QDateTime& _registationDate)
{
    RegistrationDate = _registationDate;
}

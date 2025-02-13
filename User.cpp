#include "User.h"

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
        Email(Email)
{}


#ifndef BUYER_H
#define BUYER_H

#include "User.h"
#include "JsonManager.h"

class Buyer : public User {
public:
    Buyer(); // Для самостоятельной регистрации
    Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email);

    // Возвращает true, если регистрация прошла успешно, и false в противном случае.
    static bool registerBuyer(const QString& login,
                              const QString& password,
                              const QString& fullName,
                              const QString& email,
                              JsonManager &jsonManager);
};

#endif // BUYER_H

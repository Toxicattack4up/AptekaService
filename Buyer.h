#ifndef BUYER_H
#define BUYER_H

#include "User.h"
#include "JsonManager.h"

class Buyer : public User {
public:
    // Конструктор: создаёт покупателя с заданными данными
    Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email);

    // Регистрирует нового покупателя, возвращает true при успехе
    static bool registerBuyer(const QString& login, const QString& password,
                              const QString& fullName, const QString& email,
                              JsonManager& jsonManager);
};

#endif // BUYER_H

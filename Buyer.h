#ifndef BUYER_H
#define BUYER_H

#include "User.h"

class Buyer : public User {
public:
    Buyer(); // Для самостоятельной регистрации
    Buyer(const QString& login, const QString& Password, const QString& fullName, const QString& email);
};

#endif // BUYER_H

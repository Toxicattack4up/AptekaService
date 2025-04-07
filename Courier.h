#ifndef COURIER_H
#define COURIER_H

#include "User.h"

class Courier : public User {
public:
    // Конструктор: создаёт курьера с заданными данными
    Courier(const QString& login, const QString& password, const QString& fullName, const QString& email);
};

#endif // COURIER_H

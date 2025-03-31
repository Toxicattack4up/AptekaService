#ifndef COURIER_H
#define COURIER_H

#include "User.h"

class Courier : public User {
public:
    Courier() = default;
    Courier(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email);

};

#endif // COURIER_H

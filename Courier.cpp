#include "Courier.h"

Courier::Courier(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email)
    : User(UserRole::Courier, login, passwordHash, fullName, email) {
}

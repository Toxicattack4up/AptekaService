#include "Courier.h"

Courier::Courier(const QString& login, const QString& Password, const QString& fullName, const QString& email)
    : User(UserRole::Courier, login, Password, fullName, email) {
}

#include "Buyer.h"

Buyer::Buyer()
    : User(UserRole::Buyer, "", "", "", "") {
    // Пустой конструктор для самостоятельной регистрации
}

Buyer::Buyer(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email)
    : User(UserRole::Buyer, login, passwordHash, fullName, email) {
}

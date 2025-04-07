#include "Courier.h"

// Конструктор: инициализирует курьера с ролью Courier
Courier::Courier(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Courier, login, password, fullName, email) {
}

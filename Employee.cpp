#include "Employee.h"

// Конструктор: инициализирует сотрудника с ролью Seller
Employee::Employee(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Seller, login, password, fullName, email) {
}

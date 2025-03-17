#include "Employee.h"

Employee::Employee(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email)
    : User(UserRole::Seller, login, passwordHash, fullName, email) { // Предполагаем роль Seller для сотрудника аптеки
}

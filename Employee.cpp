#include "Employee.h"

Employee::Employee(const QString& login, const QString& Password, const QString& fullName, const QString& email)
    : User(UserRole::Seller, login, Password, fullName, email) { // Предполагаем роль Seller для сотрудника аптеки
}

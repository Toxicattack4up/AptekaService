#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "User.h"

class Employee : public User {
public:
    // Конструктор: создаёт сотрудника (продавца) с заданными данными
    Employee(const QString& login, const QString& password, const QString& fullName, const QString& email);
};

#endif // EMPLOYEE_H

#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "User.h"

class Employee : public User {
public:
    Employee(const QString& login, const QString& Password, const QString& fullName, const QString& email);

};

#endif // EMPLOYEE_H

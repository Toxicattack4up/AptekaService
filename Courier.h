#ifndef COURIER_H
#define COURIER_H

#include "User.h"
#include "JsonManager.h"
#include "CentralWarehouse.h"

class Courier : public User {
public:
    Courier(const QString& login, const QString& password, const QString& fullName, const QString& email,
            JsonManager& manager, CentralWarehouse& warehouse);
    bool transferMedicine(const QString& medicineTitle, int quantity, int pharmacyId);

private:
    JsonManager& jsonManager;
    CentralWarehouse& warehouse;
};

#endif // COURIER_H

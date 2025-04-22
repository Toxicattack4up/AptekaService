#ifndef BUYER_H
#define BUYER_H

#include "User.h"
#include "JsonManager.h"

class Buyer : public User {
public:
    Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email);
    bool registerBuyer(const QString& login, const QString& password,
                       const QString& fullName, const QString& email, JsonManager& jsonManager);
    double getBalance(JsonManager& jsonManager) const;
    bool depositBalance(JsonManager& jsonManager, double amount);
    bool purchaseMedicine(const QString& medicineTitle, int quantity, int pharmacyId, JsonManager& jsonManager);
};

#endif // BUYER_H

#ifndef BUYER_H
#define BUYER_H

#include "User.h"
#include "JsonManager.h"
#include "Pharmacy.h"
#include <QStringList>

class Buyer : public User {
public:
    Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email);
    bool login(const QString& login, const QString& password, JsonManager& jsonManager);
    bool registerBuyer(const QString& login, const QString& password, const QString& fullName,
                       const QString& email, JsonManager& jsonManager);
    double getBalance(JsonManager& jsonManager) const;
    bool depositBalance(JsonManager& jsonManager, double amount);
    bool purchaseMedicine(const QString& medicineTitle, int pharmacyId, JsonManager& jsonManager);
    QList<Pharmacy> getAvailablePharmacies(JsonManager& jsonManager) const;
    QStringList getAvailableMedicines(int pharmacyId, JsonManager& jsonManager) const;

private:
    void showError(const QString& message);
};

#endif // BUYER_H

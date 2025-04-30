#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "User.h"
#include "JsonManager.h"
#include "centralwarehouse.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include <QComboBox>

class Administrator : public User {
public:
    Administrator(const QString& login, const QString& password, const QString& fullName,
                  const QString& email, JsonManager& manager, CentralWarehouse& warehouse);
    void addUser(const QString& role, const QString& login, const QString& password,
                 const QString& fullName, const QString& email, int pharmacyId);
    bool removeUser(const QString& login);
    User findUser(const QString& login);
    void createPharmacy(const Pharmacy& pharmacy);
    bool removePharmacy(int id);
    Pharmacy findPharmacy(int id);
    QList<Pharmacy> searchPharmacy(int id, const QString& address);
    void addMedicineToWarehouse(const PharmacyItem& item);
    bool removeMedicine(const QString& title);
    PharmacyItem findMedicine(const QString& title);
    QList<PharmacyItem> searchMedicine(const QString& title);
    void loadPharmaciesToComboBox(QComboBox* comboBox);

private:
    JsonManager& jsonManager;
    CentralWarehouse& warehouse;
    void showError(const QString& message);
};

#endif // ADMINISTRATOR_H

#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QList>
#include <QMap>
#include "User.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"

class JsonManager {
public:
    JsonManager();
    void addEmployee(const QString& role, const QString& login, const QString& password,
                     const QString& fullName, const QString& email, int pharmacyId = 0);
    bool removeEmployee(const QString& login);
    QList<User> getEmployee() const;
    void addMedicine(const PharmacyItem& item); // Новый метод без pharmacyId
    bool moveMedicineToPharmacy(const QString& title, int quantity, int pharmacyId); // Метод для курьера
    bool removeMedicine(const QString& title);
    bool removePharmacy(int id);
    void addPharmacy(const Pharmacy& pharmacy);
    QList<PharmacyItem> getMedicine() const;
    QList<Pharmacy> getPharmacy() const;
    QString validateUser(const QString& login, const QString& password);
    double getBuyerBalance(const QString& login) const;
    bool depositBalance(const QString& login, double amount);
    bool makePurchase(const QString& medicineTitle, int quantity, int pharmacyId, const QString& buyerLogin);
    QList<PharmacyItem> searchMedicine(const QString& title);
    QList<Pharmacy> searchPharmacy(int id, const QString& address);
    QList<User> searchEmployee(const QString& role, const QString& login, const QString& fullName, const QString& email);
    int getPharmacyStock(int pharmacyId) const;
    QList<PharmacyItem> getWarehouseItems() const; // Для получения списка лекарств на складе

private:
    void loadFromJson();
    void saveAllToJson();
    QList<User> employees;
    QList<PharmacyItem> medicines; // Лекарства в аптеках
    QList<PharmacyItem> warehouseItems; // Лекарства на складе
    QList<Pharmacy> pharmacies;
    QMap<QString, double> buyerBalances;
    QMap<int, double> pharmacyRevenues;
};

#endif // JSONMANAGER_H

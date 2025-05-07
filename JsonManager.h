#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include "User.h"
#include "PharmacyItem.h"
#include "Pharmacy.h"
#include "centralwarehouse.h"
#include <QList>
#include <QString>
#include <QMap>
#include <QDateTime>

class JsonManager {
public:
    struct Purchase {
        QDateTime timestamp;
        QString medicineTitle;
        double amount;
    };

    JsonManager();
    JsonManager(CentralWarehouse* warehouse); // Новый конструктор
    void loadFromJson();
    bool saveAllToJson();
    void addToBackpack(const QString& login, const QString& medicineTitle);
    void removeFromBackpack(const QString& login, const QString& medicineTitle);
    QStringList getBackpack(const QString& login) const;
    void setBuyerBalance(const QString& login, double balance);
    QList<Purchase> getPurchaseHistory(const QString& login) const;
    void addMedicine(const PharmacyItem& item);
    bool moveMedicineToPharmacy(const QString& title, int quantity, int pharmacyId);
    bool removeMedicine(const QString& title, int pharmacyId);
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
    QList<PharmacyItem> getWarehouseItems() const;
    void addEmployee(const QString& role, const QString& login, const QString& password,
                     const QString& fullName, const QString& email, int pharmacyId);
    bool removeEmployee(const QString& login);
    QList<User> getEmployee() const;

private:
    QList<User> employees;
    QList<PharmacyItem> medicines;
    QList<PharmacyItem> warehouseItems;
    QList<Pharmacy> pharmacies;
    QMap<QString, double> buyerBalances;
    QMap<int, double> pharmacyRevenues;
    QMap<QString, QStringList> buyerBackpacks;
    QMap<QString, QList<Purchase>> buyerPurchaseHistory;
    CentralWarehouse* centralWarehouse; // Указатель на CentralWarehouse
};

#endif // JSONMANAGER_H

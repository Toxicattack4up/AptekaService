#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include "User.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDebug>
#include <QList>

class JsonManager {
public:
    JsonManager();
    const QList<User> &getEmployee() const;
    const QList<Pharmacy> &getPharmacy() const;
    const QList<PharmacyItem> &getMedicine() const;
    bool makePurchase(const QString &medicineTitle, int quantity, int pharmacyId, const QString &buyerLogin = "");
    QString validateUser(const QString &login, const QString &password);
    void addEmployee(const QString &_role, const QString &_login, const QString &_password,
                     const QString &_fullName, const QString &_email);
    void removeEmployee(const QString &login);
    QList<User> searchEmployee(const QString &_role, const QString &_login, const QString &_password,
                               const QString &_fullName, const QString &_email);
    void addPharmacy(const Pharmacy &pharmacy);
    void removePharmacy(int id);
    QList<Pharmacy> searchPharmacy(int id, const QString &address);
    void addMedicine(const PharmacyItem &item, int pharmacyId);
    void removeMedicine(const QString &title);
    QList<PharmacyItem> searchMedicine(const QString &title);
    double getBuyerBalance(const QString &login) const;
    bool depositBalance(const QString &login, double amount);
    double getPharmacyRevenue(int pharmacyId) const;
    QJsonArray getPurchaseHistory(int pharmacyId) const;

private:
    QString hashPassword(const QString &password);
    QJsonObject loadJson();
    void saveAllToJson();
    QString filePath = "data.json";
    QList<User> employees;
    QList<Pharmacy> pharmacies;
    QList<PharmacyItem> medicines;
    QMap<QString, double> buyerBalances;
    QMap<int, double> pharmacyRevenues;
    QJsonArray purchaseHistory;
};

#endif // JSONMANAGER_H

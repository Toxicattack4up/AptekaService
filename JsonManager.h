#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDateTime>

#include "User.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"

class JsonManager
{
public:
    JsonManager();

    //_____Управление сотрудниками_____
    QList<User> LoadEmployees();
    void SaveEmployees();
    void AddEmployee(const QString &_role,
                     const QString &_login,
                     const QString &_password,
                     const QString &_fullName,
                     const QString &_email);
    void RemoveEmployee(const QString &login);
    QList<User> SearchEmployee(const QString &_role,
                               const QString &_login,
                               const QString &_password,
                               const QString &_fullName,
                               const QString &_email);

    // _____Аптеки_____
    QList<Pharmacy> LoadPharmacies();
    void SavePharmacies(const QList<Pharmacy>& pharmacies);
    void AddPharmacy(const Pharmacy& pharmacy);
    void RemovePharmacy(int id);
    QList<Pharmacy> SearchPharmacy(int id, const QString& address);


    // _____Медикаменты_____
    QList<PharmacyItem> LoadMedicines();
    void SaveMedicines(const QList<PharmacyItem>& medicines);
    void AddMedicine(const PharmacyItem& item);
    void RemoveMedicine(const QString& title);
    QList<PharmacyItem> SearchMedicine(const QString& title);


    // _____Остальные функции_____
    QJsonObject LoadJSON(const QString &filePath);
    void SaveToJSON(const QString &filePath, const QJsonObject &json);
    QString HashPassword(const QString &password);
    bool ValidateUser(const QString &login, const QString &password);

private:
    QList<User> employees;
    QList<Pharmacy> pharmacies;
    QList<PharmacyItem> medicines;

    // Преобразование списка в JSON
    QJsonArray EmployeesToJsonArray() const;
    QJsonArray PharmaciesToJsonArray() const;
    QJsonArray MedicinesToJsonArray() const;

};

#endif // JSONMANAGER_H

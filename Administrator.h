#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QString>
#include <QVector>

#include "User.h"
#include "Employee.h"
#include "Courier.h"
#include "PharmacyItem.h"
#include "Pharmacy.h"
#include "JsonManager.h"

class Administrator : public User {
private:
    QVector<User> users;               // Список пользователей
    QVector<Pharmacy> pharmacies;       // Список аптек
    QVector<PharmacyItem> medicines;    // Список лекарств
    JsonManager jsonManager;            // Класс для работы с файлами

public:
    // Конструктор
    Administrator(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email);

    // Работа с пользователями
    void addUser(const QString& role,
                 const QString& login,
                 const QString& passwordHash,
                 const QString& fullName,
                 const QString& email);
    bool removeUser(const QString& login);
    User* findUser(const QString& login);

    // Работа с аптечными помещениями
    void createPharmacy(const Pharmacy& pharmacy);
    bool removePharmacy(int id);
    Pharmacy* findPharmacy(int id);
    QList<Pharmacy> searchPharmacy(int id, const QString& address);

    // Управление складом аптек
    void addMedicine(const PharmacyItem& item);
    bool removeMedicine(const QString& title);
    PharmacyItem* findMedicine(const QString& title);
    QList<PharmacyItem> searchMedicine(const QString& title);
};

#endif // ADMINISTRATOR_H

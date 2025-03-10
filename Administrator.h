#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QString>
#include <QVector>
#include "User.h"
#include "PharmacyItem.h"
#include "Pharmacy.h"
//#include "JsonManager.h"

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
    void addUser(const User& user);
    bool removeUser(const QString& login);
    User* findUser(const QString& login);

    // Работа с аптечными помещениями
    void createPharmacy(const Pharmacy& pharmacy);
    bool removePharmacy(int id);
    Pharmacy* findPharmacy(int id);

    // Управление складом аптек
    void addMedicine(const PharmacyItem& item);
    bool removeMedicine(const QString& title);
    PharmacyItem* findMedicine(const QString& title);

    // Сохранение данных
    void saveData();
};

#endif // ADMINISTRATOR_H

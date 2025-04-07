#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "User.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include "JsonManager.h"

class Administrator : public User {
private:
    JsonManager& jsonManager; // Ссылка на JsonManager для работы с данными

public:
    // Конструктор: создаёт администратора с заданными данными
    Administrator(const QString& login, const QString& password, const QString& fullName,
                  const QString& email, JsonManager& manager);

    // Работа с пользователями
    void addUser(const QString& role, const QString& login, const QString& password,
                 const QString& fullName, const QString& email);
    bool removeUser(const QString& login);
    User findUser(const QString& login); // Возвращает копию пользователя

    // Работа с аптеками
    void createPharmacy(const Pharmacy& pharmacy);
    bool removePharmacy(int id);
    Pharmacy findPharmacy(int id); // Возвращает копию аптеки
    QList<Pharmacy> searchPharmacy(int id, const QString& address);

    // Работа с медикаментами
    void addMedicine(const PharmacyItem& item);
    bool removeMedicine(const QString& title);
    PharmacyItem findMedicine(const QString& title); // Возвращает копию медикамента
    QList<PharmacyItem> searchMedicine(const QString& title);
};

#endif // ADMINISTRATOR_H

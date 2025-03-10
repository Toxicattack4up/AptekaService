#include "Administrator.h"
#include "JsonManager.h"
#include "UserRoleHelper.h"

// Конструктор
Administrator::Administrator(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email)
    : User(UserRole::Administrator, login, passwordHash, fullName, email) {
}

// Работа с пользователями
void Administrator::addUser(const User& user) {
    users.append(user);
    jsonManager.AddEmployee(UserRoleHelper::getRoleNames().value(user.getRole()),
                            user.getLogin(),
                            user.getPasswordHash(),
                            user.getFullName(),
                            user.getEmail());
}

bool Administrator::removeUser(const QString& login) {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getLogin() == login) {
            users.remove(i);
            jsonManager.RemoveEmployee(login);
            return true;
        }
    }
    return false;
}

User* Administrator::findUser(const QString& login) {
    for (auto& user : users) {
        if (user.getLogin() == login) {
            return &user;
        }
    }
    return nullptr;
}

// Работа с аптечными помещениями
void Administrator::createPharmacy(const Pharmacy& pharmacy) {
    pharmacies.append(pharmacy);
    // В будущем добавим сохранение данных аптек
}

bool Administrator::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            pharmacies.remove(i);
            return true;
        }
    }
    return false;
}

Pharmacy* Administrator::findPharmacy(int id) {
    for (auto& pharmacy : pharmacies) {
        if (pharmacy.getId() == id) {
            return &pharmacy;
        }
    }
    return nullptr;
}

// Управление складом аптек
void Administrator::addMedicine(const PharmacyItem& item) {
    medicines.append(item);
    // В будущем добавим сохранение данных медикаментов
}

bool Administrator::removeMedicine(const QString& title) {
    for (int i = 0; i < medicines.size(); ++i) {
        if (medicines[i].getTitle() == title) {
            medicines.remove(i);
            return true;
        }
    }
    return false;
}

PharmacyItem* Administrator::findMedicine(const QString& title) {
    for (auto& medicine : medicines) {
        if (medicine.getTitle() == title) {
            return &medicine;
        }
    }
    return nullptr;
}

// Сохранение данных
void Administrator::saveData() {
    jsonManager.SaveEmployees();
    // Сохранение аптек и медикаментов можно добавить по аналогии в будущем.
}

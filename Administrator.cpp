#include "Administrator.h"
#include "JsonManager.h"
#include "UserRoleHelper.h"

// Конструктор
Administrator::Administrator(const QString& login, const QString& passwordHash, const QString& fullName, const QString& email)
    : User(UserRole::Administrator, login, passwordHash, fullName, email) {
}


// _____Работа с пользователями_____
void Administrator::addUser(const QString& role, const QString& login, const QString& passwordHash, const QString& fullName, const QString& email) {
    UserRole userRole = UserRoleHelper::fromString(role);
    if (userRole == UserRole::Seller)
    { // Сотрудник аптеки
        Employee employee(login, passwordHash, fullName, email);
        users.append(employee);
        jsonManager.AddEmployee(role, login, passwordHash, fullName, email);
    }
    else if (userRole == UserRole::Courier)
    { // Курьер
        Courier courier(login, passwordHash, fullName, email);
        users.append(courier);
        jsonManager.AddEmployee(role, login, passwordHash, fullName, email);
    }
    else
    {
        qDebug() << "Администратор может создавать только сотрудников или курьеров!";
        return;
    }
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


// _____Работа с аптекой_____
void Administrator::createPharmacy(const Pharmacy& pharmacy) {
    pharmacies.append(pharmacy);
    jsonManager.AddPharmacy(pharmacy);
}

bool Administrator::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            pharmacies.remove(i);
            jsonManager.RemovePharmacy(id);
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

QList<Pharmacy> Administrator::searchPharmacy(int id, const QString& address) {
    return jsonManager.SearchPharmacy(id, address);
}


// _____Управление складом_____
void Administrator::addMedicine(const PharmacyItem& item) {
    medicines.append(item);
    jsonManager.AddMedicine(item);
}

bool Administrator::removeMedicine(const QString& title) {
    for (int i = 0; i < medicines.size(); ++i) {
        if (medicines[i].getTitle() == title) {
            medicines.remove(i);
            jsonManager.RemoveMedicine(title);
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

QList<PharmacyItem> Administrator::searchMedicine(const QString& title) {
    return jsonManager.SearchMedicine(title);
}

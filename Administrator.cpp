#include "Administrator.h"
#include "UserRoleHelper.h"
#include <QDebug>

Administrator::Administrator(const QString& login, const QString& password, const QString& fullName,
                             const QString& email, JsonManager& manager, CentralWarehouse& warehouse)
    : User(UserRole::Administrator, login, password, fullName, email), jsonManager(manager), warehouse(warehouse) {
}

void Administrator::addUser(const QString& role, const QString& login, const QString& password,
                            const QString& fullName, const QString& email) {
    UserRole userRole = UserRoleHelper::fromString(role);
    if (userRole == UserRole::Seller || userRole == UserRole::Courier) {
        jsonManager.addEmployee(role, login, password, fullName, email);
        qDebug() << "Пользователь" << login << "успешно добавлен!";
    } else {
        qDebug() << "Ошибка: Администратор может создавать только продавцов или курьеров!";
    }
}

bool Administrator::removeUser(const QString& login) {
    jsonManager.removeEmployee(login);
    return true;
}

User Administrator::findUser(const QString& login) {
    QList<User> found = jsonManager.searchEmployee("", login, "", "", "");
    return found.isEmpty() ? User() : found.first();
}

void Administrator::createPharmacy(const Pharmacy& pharmacy) {
    jsonManager.addPharmacy(pharmacy);
    qDebug() << "Аптека с ID" << pharmacy.getId() << "успешно создана!";
}

bool Administrator::removePharmacy(int id) {
    jsonManager.removePharmacy(id);
    return true;
}

Pharmacy Administrator::findPharmacy(int id) {
    QList<Pharmacy> found = jsonManager.searchPharmacy(id, "");
    return found.isEmpty() ? Pharmacy(0, "", 0, 0) : found.first();
}

QList<Pharmacy> Administrator::searchPharmacy(int id, const QString& address) {
    return jsonManager.searchPharmacy(id, address);
}

void Administrator::addMedicineToWarehouse(const PharmacyItem& item) {
    warehouse.addMedicine(item);
    qDebug() << "Медикамент" << item.getTitle() << "успешно добавлен в центральный склад!";
}

bool Administrator::removeMedicine(const QString& title) {
    jsonManager.removeMedicine(title);
    return true;
}

PharmacyItem Administrator::findMedicine(const QString& title) {
    QList<PharmacyItem> found = jsonManager.searchMedicine(title);
    return found.isEmpty() ? PharmacyItem("", 0, false, QDate(), 0) : found.first();
}

QList<PharmacyItem> Administrator::searchMedicine(const QString& title) {
    return jsonManager.searchMedicine(title);
}

#include "Administrator.h"
#include "UserRoleHelper.h"
#include <QDebug>

// Конструктор: инициализирует администратора с ролью Administrator
Administrator::Administrator(const QString& login, const QString& password, const QString& fullName,
                             const QString& email, JsonManager& manager)
    : User(UserRole::Administrator, login, password, fullName, email), jsonManager(manager) {
}

// Добавление пользователя (продавца или курьера)
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

// Удаление пользователя по логину
bool Administrator::removeUser(const QString& login) {
    jsonManager.removeEmployee(login);
    return true; // Предполагаем, что JsonManager обработает ошибки
}

// Поиск пользователя по логину (возвращает копию)
User Administrator::findUser(const QString& login) {
    QList<User> found = jsonManager.searchEmployee("", login, "", "", "");
    return found.isEmpty() ? User() : found.first();
}

// Создание аптеки
void Administrator::createPharmacy(const Pharmacy& pharmacy) {
    jsonManager.addPharmacy(pharmacy);
    qDebug() << "Аптека с ID" << pharmacy.getId() << "успешно создана!";
}

// Удаление аптеки по ID
bool Administrator::removePharmacy(int id) {
    jsonManager.removePharmacy(id);
    return true; // Предполагаем, что JsonManager обработает ошибки
}

// Поиск аптеки по ID (возвращает копию)
Pharmacy Administrator::findPharmacy(int id) {
    QList<Pharmacy> found = jsonManager.searchPharmacy(id, "");
    return found.isEmpty() ? Pharmacy(0, "", 0, 0) : found.first();
}

// Поиск аптек по ID и адресу
QList<Pharmacy> Administrator::searchPharmacy(int id, const QString& address) {
    return jsonManager.searchPharmacy(id, address);
}

// Добавление медикамента
void Administrator::addMedicine(const PharmacyItem& item) {
    jsonManager.addMedicine(item);
    qDebug() << "Медикамент" << item.getTitle() << "успешно добавлен!";
}

// Удаление медикамента по названию
bool Administrator::removeMedicine(const QString& title) {
    jsonManager.removeMedicine(title);
    return true; // Предполагаем, что JsonManager обработает ошибки
}

// Поиск медикамента по названию (возвращает копию)
PharmacyItem Administrator::findMedicine(const QString& title) {
    QList<PharmacyItem> found = jsonManager.searchMedicine(title);
    return found.isEmpty() ? PharmacyItem("", 0, false, QDate(), 0) : found.first();
}

// Поиск медикаментов по названию
QList<PharmacyItem> Administrator::searchMedicine(const QString& title) {
    return jsonManager.searchMedicine(title);
}

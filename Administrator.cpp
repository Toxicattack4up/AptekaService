#include "Administrator.h"
#include "UserRoleHelper.h"
#include "logger.h"
#include <QMessageBox>

// Конструктор администратора
Administrator::Administrator(const QString& login, const QString& password, const QString& fullName,
                             const QString& email, JsonManager& manager, CentralWarehouse& warehouse)
    : User(UserRole::Administrator, login, password, fullName, email), jsonManager(manager), warehouse(warehouse) {
    Logger::instance().log("Administrator", QString("Создан администратор %1").arg(login));
}

// Добавление нового пользователя (продавца или курьера)
void Administrator::addUser(const QString& role, const QString& login, const QString& password,
                            const QString& fullName, const QString& email) {
    UserRole userRole = UserRoleHelper::fromString(role);
    if (userRole == UserRole::Seller || userRole == UserRole::Courier) {
        jsonManager.addEmployee(role, login, password, fullName, email);
        Logger::instance().log("Administrator", QString("Добавлен пользователь %1 (%2)").arg(login).arg(role));
    } else {
        showError("Ошибка: Администратор может создавать только продавцов или курьеров!");
    }
}

// Удаление пользователя
bool Administrator::removeUser(const QString& login) {
    if (jsonManager.removeEmployee(login)) {
        Logger::instance().log("Administrator", QString("Удалён пользователь %1").arg(login));
        return true;
    }
    showError(QString("Ошибка: Пользователь %1 не найден").arg(login));
    return false;
}

// Поиск пользователя по логину
User Administrator::findUser(const QString& login) {
    QList<User> found = jsonManager.searchEmployee("", login, "", ""); // Убран phone
    if (found.isEmpty()) {
        Logger::instance().log("Administrator", QString("Пользователь %1 не найден").arg(login));
        return User();
    }
    Logger::instance().log("Administrator", QString("Найден пользователь %1").arg(login));
    return found.first();
}

// Создание новой аптеки
void Administrator::createPharmacy(const Pharmacy& pharmacy) {
    jsonManager.addPharmacy(pharmacy);
    Logger::instance().log("Administrator", QString("Создана аптека ID %1").arg(pharmacy.getId()));
}

// Удаление аптеки
bool Administrator::removePharmacy(int id) {
    if (jsonManager.removePharmacy(id)) {
        Logger::instance().log("Administrator", QString("Удалена аптека ID %1").arg(id));
        return true;
    }
    showError(QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    return false;
}

// Поиск аптеки по ID
Pharmacy Administrator::findPharmacy(int id) {
    QList<Pharmacy> found = jsonManager.searchPharmacy(id, "");
    if (found.isEmpty()) {
        Logger::instance().log("Administrator", QString("Аптека ID %1 не найдена").arg(id));
        return Pharmacy(0, "", 0, 0);
    }
    Logger::instance().log("Administrator", QString("Найдена аптека ID %1").arg(id));
    return found.first();
}

// Поиск аптек по ID или адресу
QList<Pharmacy> Administrator::searchPharmacy(int id, const QString& address) {
    QList<Pharmacy> found = jsonManager.searchPharmacy(id, address);
    Logger::instance().log("Administrator", QString("Поиск аптек: найдено %1").arg(found.size()));
    return found;
}

// Добавление лекарства на центральный склад
void Administrator::addMedicineToWarehouse(const PharmacyItem& item) {
    if (item.getTitle().isEmpty()) {
        showError("Ошибка: Название лекарства не может быть пустым!");
        return;
    }
    if (item.getQuantity() <= 0) {
        showError("Ошибка: Количество должно быть больше 0!");
        return;
    }
    if (item.isExpired()) {
        showError("Ошибка: Лекарство просрочено!");
        return;
    }
    warehouse.addMedicine(item);
    Logger::instance().log("Administrator", QString("Добавлено лекарство %1 на склад").arg(item.getTitle()));
}

// Удаление лекарства из системы
bool Administrator::removeMedicine(const QString& title) {
    if (jsonManager.removeMedicine(title)) {
        Logger::instance().log("Administrator", QString("Удалено лекарство %1").arg(title));
        return true;
    }
    showError(QString("Ошибка: Лекарство %1 не найдено").arg(title));
    return false;
}

// Поиск лекарства по названию
PharmacyItem Administrator::findMedicine(const QString& title) {
    QList<PharmacyItem> found = jsonManager.searchMedicine(title);
    if (found.isEmpty()) {
        Logger::instance().log("Administrator", QString("Лекарство %1 не найдено").arg(title));
        return PharmacyItem("", 0, false, QDate(), 0);
    }
    Logger::instance().log("Administrator", QString("Найдено лекарство %1").arg(title));
    return found.first();
}

// Поиск лекарств по названию
QList<PharmacyItem> Administrator::searchMedicine(const QString& title) {
    QList<PharmacyItem> found = jsonManager.searchMedicine(title);
    Logger::instance().log("Administrator", QString("Поиск лекарств: найдено %1").arg(found.size()));
    return found;
}

// Вывод ошибки пользователю
void Administrator::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("Administrator", message);
}

#include "JsonManager.h"
#include "logger.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>

// Конструктор менеджера JSON
JsonManager::JsonManager() {
    loadFromJson();
    bool hasAdmin = false;
    for (const User& user : employees) {
        if (UserRoleHelper::toString(user.getRole()) == "Администратор") {
            hasAdmin = true;
            break;
        }
    }
    if (!hasAdmin) {
        addEmployee("Администратор", "admin", "admin123", "Admin User", "admin@example.com", 0);
        Logger::instance().log("JsonManager", "Создана базовая учётная запись администратора");
    }
}

// Загрузка данных из JSON
void JsonManager::loadFromJson() {
    QFile file("employees.json");
    if (!file.exists()) {
        Logger::instance().log("JsonManager", "Файл employees.json не найден, будет создан новый");
        saveAllToJson();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance().log("JsonManager", "Ошибка открытия employees.json для чтения");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        Logger::instance().log("JsonManager", "Ошибка парсинга employees.json");
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray employeesArray = root["employees"].toArray();
    employees.clear();
    for (const QJsonValue& value : employeesArray) {
        QJsonObject obj = value.toObject();
        User user(
            UserRoleHelper::fromString(obj["role"].toString()),
            obj["login"].toString(),
            obj["password"].toString(),
            obj["fullName"].toString(),
            obj["email"].toString()
            );
        user.setRegistrationDate(QDate::fromString(obj["registrationDate"].toString(), "yyyy-MM-dd"));
        employees.append(user);
    }

    QJsonArray medicinesArray = root["medicines"].toArray();
    medicines.clear();
    for (const QJsonValue& value : medicinesArray) {
        QJsonObject obj = value.toObject();
        PharmacyItem item(
            obj["title"].toString(),
            obj["price"].toDouble(),
            obj["recipeRequired"].toBool(),
            QDate::fromString(obj["expirationDate"].toString(), "yyyy-MM-dd"),
            obj["quantity"].toInt()
            );
        item.setPharmacyId(obj["pharmacyId"].toInt());
        medicines.append(item);
    }

    QJsonArray warehouseArray = root["warehouse"].toArray();
    warehouseItems.clear();
    for (const QJsonValue& value : warehouseArray) {
        QJsonObject obj = value.toObject();
        PharmacyItem item(
            obj["title"].toString(),
            obj["price"].toDouble(),
            obj["recipeRequired"].toBool(),
            QDate::fromString(obj["expirationDate"].toString(), "yyyy-MM-dd"),
            obj["quantity"].toInt()
            );
        warehouseItems.append(item);
    }

    QJsonArray pharmaciesArray = root["pharmacies"].toArray();
    pharmacies.clear();
    for (const QJsonValue& value : pharmaciesArray) {
        QJsonObject obj = value.toObject();
        Pharmacy pharmacy(
            obj["id"].toInt(),
            obj["address"].toString(),
            obj["size"].toDouble(),
            obj["maxCapacity"].toInt()
            );
        pharmacies.append(pharmacy);
    }

    QJsonObject balances = root["buyerBalances"].toObject();
    buyerBalances.clear();
    for (const QString& key : balances.keys()) {
        buyerBalances[key] = balances[key].toDouble();
    }

    QJsonObject revenues = root["pharmacyRevenues"].toObject();
    pharmacyRevenues.clear();
    for (const QString& key : revenues.keys()) {
        pharmacyRevenues[key.toInt()] = revenues[key].toDouble();
    }

    Logger::instance().log("JsonManager", QString("Загружено из employees.json: %1 сотрудников, %2 лекарств, %3 на складе, %4 аптек")
                                              .arg(employees.size()).arg(medicines.size()).arg(warehouseItems.size()).arg(pharmacies.size()));
}

// Сохранение данных в JSON
void JsonManager::saveAllToJson() {
    QJsonObject root;
    QJsonArray employeesArray;
    for (const User& user : employees) {
        QJsonObject obj;
        obj["role"] = UserRoleHelper::toString(user.getRole());
        obj["login"] = user.getLogin();
        obj["password"] = user.getPassword();
        obj["fullName"] = user.getFullName();
        obj["email"] = user.getEmail();
        obj["registrationDate"] = user.getRegistrationDate().toString("yyyy-MM-dd");
        employeesArray.append(obj);
    }
    root["employees"] = employeesArray;

    QJsonArray medicinesArray;
    for (const PharmacyItem& item : medicines) {
        QJsonObject obj;
        obj["title"] = item.getTitle();
        obj["price"] = item.getPrice();
        obj["quantity"] = item.getQuantity();
        obj["recipeRequired"] = item.isRecipeRequired();
        obj["expirationDate"] = item.getExpirationDate().toString("yyyy-MM-dd");
        obj["pharmacyId"] = item.getPharmacyId();
        medicinesArray.append(obj);
    }
    root["medicines"] = medicinesArray;

    QJsonArray warehouseArray;
    for (const PharmacyItem& item : warehouseItems) {
        QJsonObject obj;
        obj["title"] = item.getTitle();
        obj["price"] = item.getPrice();
        obj["quantity"] = item.getQuantity();
        obj["recipeRequired"] = item.isRecipeRequired();
        obj["expirationDate"] = item.getExpirationDate().toString("yyyy-MM-dd");
        warehouseArray.append(obj);
    }
    root["warehouse"] = warehouseArray;

    QJsonArray pharmaciesArray;
    for (const Pharmacy& pharmacy : pharmacies) {
        QJsonObject obj;
        obj["id"] = pharmacy.getId();
        obj["address"] = pharmacy.getAddress();
        obj["size"] = pharmacy.getSize();
        obj["maxCapacity"] = pharmacy.getMaxCapacity();
        pharmaciesArray.append(obj);
    }
    root["pharmacies"] = pharmaciesArray;

    QJsonObject balances;
    for (const QString& key : buyerBalances.keys()) {
        balances[key] = buyerBalances[key];
    }
    root["buyerBalances"] = balances;

    QJsonObject revenues;
    for (const int& key : pharmacyRevenues.keys()) {
        revenues[QString::number(key)] = pharmacyRevenues[key];
    }
    root["pharmacyRevenues"] = revenues;

    QJsonDocument doc(root);
    QFile file("employees.json");
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().log("JsonManager", "Ошибка открытия employees.json для записи");
        return;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    Logger::instance().log("JsonManager", "Данные сохранены в employees.json");
}

// Добавление лекарства на склад
void JsonManager::addMedicine(const PharmacyItem& item) {
    for (PharmacyItem& existing : warehouseItems) {
        if (existing.getTitle() == item.getTitle() &&
            existing.getPrice() == item.getPrice() &&
            existing.isRecipeRequired() == item.isRecipeRequired() &&
            existing.getExpirationDate() == item.getExpirationDate()) {
            existing.setQuantity(existing.getQuantity() + item.getQuantity());
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Добавлено %1 единиц %2 на склад")
                                                      .arg(item.getQuantity()).arg(item.getTitle()));
            return;
        }
    }
    PharmacyItem newItem = item;
    newItem.setPharmacyId(0); // Склад не имеет pharmacyId
    warehouseItems.append(newItem);
    saveAllToJson();
    Logger::instance().log("JsonManager", QString("Добавлено новое лекарство %1 на склад").arg(item.getTitle()));
}

// Перемещение лекарства со склада в аптеку
bool JsonManager::moveMedicineToPharmacy(const QString& title, int quantity, int pharmacyId) {
    // Проверка существования аптеки
    bool pharmacyExists = false;
    for (const Pharmacy& pharmacy : pharmacies) {
        if (pharmacy.getId() == pharmacyId) {
            pharmacyExists = true;
            break;
        }
    }
    if (!pharmacyExists) {
        Logger::instance().log("JsonManager", QString("Ошибка: Аптека ID %1 не найдена").arg(pharmacyId));
        return false;
    }

    // Поиск лекарства на складе
    for (PharmacyItem& warehouseItem : warehouseItems) {
        if (warehouseItem.getTitle() == title && warehouseItem.getQuantity() >= quantity) {
            // Уменьшаем количество на складе
            warehouseItem.setQuantity(warehouseItem.getQuantity() - quantity);
            if (warehouseItem.getQuantity() == 0) {
                warehouseItems.removeOne(warehouseItem);
            }

            // Добавляем в аптеку
            for (PharmacyItem& pharmacyItem : medicines) {
                if (pharmacyItem.getTitle() == title && pharmacyItem.getPharmacyId() == pharmacyId) {
                    pharmacyItem.setQuantity(pharmacyItem.getQuantity() + quantity);
                    saveAllToJson();
                    Logger::instance().log("JsonManager", QString("Перемещено %1 единиц %2 в аптеку %3")
                                                              .arg(quantity).arg(title).arg(pharmacyId));
                    return true;
                }
            }
            PharmacyItem newItem = warehouseItem;
            newItem.setQuantity(quantity);
            newItem.setPharmacyId(pharmacyId);
            medicines.append(newItem);
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Перемещено новое лекарство %1 (%2 единиц) в аптеку %3")
                                                      .arg(title).arg(quantity).arg(pharmacyId));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Недостаточно %1 (%2 единиц) на складе").arg(title).arg(quantity));
    return false;
}

// Получение списка лекарств на складе
QList<PharmacyItem> JsonManager::getWarehouseItems() const {
    Logger::instance().log("JsonManager", QString("Запрошен список лекарств на складе: %1").arg(warehouseItems.size()));
    return warehouseItems;
}

// Добавление пользователя
void JsonManager::addEmployee(const QString& role, const QString& login, const QString& password,
                              const QString& fullName, const QString& email, int pharmacyId) {
    // Проверка входных данных
    if (login.isEmpty() || password.isEmpty() || fullName.isEmpty() || email.isEmpty()) {
        Logger::instance().log("JsonManager", "Ошибка: Все поля должны быть заполнены");
        QMessageBox::warning(nullptr, "Ошибка", "Все поля должны быть заполнены!");
        return;
    }

    // Проверка формата email
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        Logger::instance().log("JsonManager", QString("Ошибка: Неверный формат email %1").arg(email));
        QMessageBox::warning(nullptr, "Ошибка", "Неверный формат email!");
        return;
    }

    // Проверка для продавца: должна существовать аптека
    if (role == "Продавец") {
        if (pharmacyId == 0) {
            Logger::instance().log("JsonManager", "Ошибка: Продавец должен быть привязан к аптеке");
            QMessageBox::warning(nullptr, "Ошибка", "Выберите аптеку для продавца!");
            return;
        }
        // Проверка существования аптеки
        bool pharmacyExists = false;
        for (const Pharmacy& pharmacy : pharmacies) {
            if (pharmacy.getId() == pharmacyId) {
                pharmacyExists = true;
                break;
            }
        }
        if (!pharmacyExists) {
            Logger::instance().log("JsonManager", QString("Ошибка: Аптека ID %1 не существует").arg(pharmacyId));
            QMessageBox::warning(nullptr, "Ошибка", QString("Аптека ID %1 не существует!").arg(pharmacyId));
            return;
        }
    }

    // Проверка на уникальность логина
    for (const User& user : employees) {
        if (user.getLogin() == login) {
            Logger::instance().log("JsonManager", QString("Ошибка: Логин %1 уже занят").arg(login));
            QMessageBox::warning(nullptr, "Ошибка", "Логин уже занят!");
            return;
        }
    }

    // Создание и добавление пользователя
    User employee(UserRoleHelper::fromString(role), login, password, fullName, email);
    if (role == "Продавец") {
        employee.setPharmacyId(pharmacyId);
    }
    employees.append(employee);
    saveAllToJson();
    Logger::instance().log("JsonManager", QString("Добавлен сотрудник %1 (%2), аптека ID %3")
                                              .arg(login).arg(role).arg(pharmacyId));
}

bool JsonManager::removeEmployee(const QString& login) {
    if (login.isEmpty()) {
        Logger::instance().log("JsonManager", "Ошибка: Логин не может быть пустым");
        return false;
    }
    for (int i = 0; i < employees.size(); ++i) {
        if (employees[i].getLogin() == login) {
            employees.removeAt(i);
            buyerBalances.remove(login);
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Удалён сотрудник %1").arg(login));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Сотрудник %1 не найден").arg(login));
    return false;
}

QList<User> JsonManager::getEmployee() const {
    Logger::instance().log("JsonManager", QString("Запрошен список сотрудников: %1").arg(employees.size()));
    return employees;
}

bool JsonManager::removeMedicine(const QString& title) {
    if (title.isEmpty()) {
        Logger::instance().log("JsonManager", "Ошибка: Название лекарства не может быть пустым");
        return false;
    }
    for (int i = 0; i < medicines.size(); ++i) {
        if (medicines[i].getTitle() == title) {
            int pharmacyId = medicines[i].getPharmacyId();
            medicines.removeAt(i);
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Удалено лекарство %1 из аптеки %2").arg(title).arg(pharmacyId));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Лекарство %1 не найдено").arg(title));
    return false;
}

bool JsonManager::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            pharmacies.removeAt(i);
            pharmacyRevenues.remove(id);
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Удалена аптека ID %1").arg(id));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    return false;
}

void JsonManager::addPharmacy(const Pharmacy& pharmacy) {
    pharmacies.append(pharmacy);
    saveAllToJson();
    Logger::instance().log("JsonManager", QString("Добавлена аптека ID %1").arg(pharmacy.getId()));
}

QList<PharmacyItem> JsonManager::getMedicine() const {
    Logger::instance().log("JsonManager", QString("Запрошен список лекарств: %1").arg(medicines.size()));
    return medicines;
}

QList<Pharmacy> JsonManager::getPharmacy() const {
    Logger::instance().log("JsonManager", QString("Запрошен список аптек: %1").arg(pharmacies.size()));
    return pharmacies;
}

QString JsonManager::validateUser(const QString& login, const QString& password) {
    for (const User& user : employees) {
        if (user.getLogin() == login && user.getPassword() == password) {
            Logger::instance().log("JsonManager", QString("Успешная авторизация пользователя %1").arg(login));
            return UserRoleHelper::toString(user.getRole());
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка авторизации: неверный логин %1 или пароль").arg(login));
    return "";
}

double JsonManager::getBuyerBalance(const QString& login) const {
    double balance = buyerBalances.value(login, 0.0);
    Logger::instance().log("JsonManager", QString("Баланс пользователя %1: %2").arg(login).arg(balance));
    return balance;
}

bool JsonManager::depositBalance(const QString& login, double amount) {
    if (amount <= 0) {
        Logger::instance().log("JsonManager", QString("Ошибка: Сумма пополнения %1 для %2 недопустима").arg(amount).arg(login));
        return false;
    }
    buyerBalances[login] += amount;
    saveAllToJson();
    Logger::instance().log("JsonManager", QString("Пополнен баланс %1 на %2").arg(login).arg(amount));
    return true;
}

bool JsonManager::makePurchase(const QString& medicineTitle, int quantity, int pharmacyId, const QString& buyerLogin) {
    for (PharmacyItem& item : medicines) {
        if (item.getTitle() == medicineTitle && item.getPharmacyId() == pharmacyId) {
            if (item.getQuantity() < quantity) {
                Logger::instance().log("JsonManager", QString("Ошибка: Недостаточно %1 (%2 единиц) в аптеке %3")
                                                          .arg(medicineTitle).arg(item.getQuantity()).arg(pharmacyId));
                return false;
            }
            double cost = item.getPrice() * quantity;
            if (!buyerLogin.isEmpty() && buyerBalances[buyerLogin] < cost) {
                Logger::instance().log("JsonManager", QString("Ошибка: Недостаточно средств у %1 для покупки %2")
                                                          .arg(buyerLogin).arg(medicineTitle));
                return false;
            }
            item.setQuantity(item.getQuantity() - quantity);
            if (!buyerLogin.isEmpty()) {
                buyerBalances[buyerLogin] -= cost;
            }
            pharmacyRevenues[pharmacyId] += cost;
            saveAllToJson();
            Logger::instance().log("JsonManager", QString("Покупка: %1 купил %2 (%3 единиц) в аптеке %4")
                                                      .arg(buyerLogin.isEmpty() ? "Продавец" : buyerLogin)
                                                      .arg(medicineTitle).arg(quantity).arg(pharmacyId));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Лекарство %1 не найдено в аптеке %2").arg(medicineTitle).arg(pharmacyId));
    return false;
}

QList<PharmacyItem> JsonManager::searchMedicine(const QString& title) {
    QList<PharmacyItem> result;
    for (const PharmacyItem& item : medicines) {
        if (title.isEmpty() || item.getTitle().contains(title, Qt::CaseInsensitive)) {
            result.append(item);
        }
    }
    Logger::instance().log("JsonManager", QString("Поиск лекарств '%1': найдено %2").arg(title).arg(result.size()));
    return result;
}

QList<Pharmacy> JsonManager::searchPharmacy(int id, const QString& address) {
    QList<Pharmacy> result;
    for (const Pharmacy& pharmacy : pharmacies) {
        if ((id == 0 || pharmacy.getId() == id) &&
            (address.isEmpty() || pharmacy.getAddress().contains(address, Qt::CaseInsensitive))) {
            result.append(pharmacy);
        }
    }
    Logger::instance().log("JsonManager", QString("Поиск аптек ID %1, адрес '%2': найдено %3")
                                              .arg(id).arg(address).arg(result.size()));
    return result;
}

QList<User> JsonManager::searchEmployee(const QString& role, const QString& login, const QString& fullName,
                                        const QString& email) {
    QList<User> result;
    for (const User& user : employees) {
        if ((role.isEmpty() || UserRoleHelper::toString(user.getRole()) == role) &&
            (login.isEmpty() || user.getLogin().contains(login, Qt::CaseInsensitive)) &&
            (fullName.isEmpty() || user.getFullName().contains(fullName, Qt::CaseInsensitive)) &&
            (email.isEmpty() || user.getEmail().contains(email, Qt::CaseInsensitive))) {
            result.append(user);
        }
    }
    Logger::instance().log("JsonManager", QString("Поиск сотрудников: найдено %1").arg(result.size()));
    return result;
}

int JsonManager::getPharmacyStock(int pharmacyId) const {
    int total = 0;
    for (const PharmacyItem& item : medicines) {
        if (item.getPharmacyId() == pharmacyId) {
            total += item.getQuantity();
        }
    }
    Logger::instance().log("JsonManager", QString("Запрошен запас аптеки ID %1: %2 единиц").arg(pharmacyId).arg(total));
    return total;
}

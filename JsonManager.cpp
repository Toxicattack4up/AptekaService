#include "JsonManager.h"
#include "logger.h"
#include "historymanager.h"
#include "centralwarehouse.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QFileInfo>

JsonManager::JsonManager() : centralWarehouse(nullptr) {
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

JsonManager::JsonManager(CentralWarehouse* warehouse) : centralWarehouse(warehouse) {
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

void JsonManager::loadFromJson() {
    QFile file("data.json");
    QFileInfo fileInfo(file);
    if (!file.exists()) {
        Logger::instance().log("JsonManager", "Файл data.json не найден, будет создан новый");
        saveAllToJson();
        return;
    }
    if (!fileInfo.isReadable()) {
        Logger::instance().log("JsonManager", "Ошибка: Нет прав на чтение файла data.json");
        QMessageBox::critical(nullptr, "Ошибка", "Нет прав на чтение файла data.json!");
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance().log("JsonManager", QString("Ошибка открытия data.json для чтения: %1").arg(file.errorString()));
        QMessageBox::critical(nullptr, "Ошибка", QString("Не удалось открыть data.json: %1").arg(file.errorString()));
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    if (data.isEmpty()) {
        Logger::instance().log("JsonManager", "Файл data.json пуст, инициализация пустых данных");
        saveAllToJson();
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        Logger::instance().log("JsonManager", "Ошибка парсинга data.json: некорректный JSON");
        QMessageBox::critical(nullptr, "Ошибка", "Некорректный формат файла data.json!");
        return;
    }
    QJsonObject root = doc.object();

    // Загрузка сотрудников
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
        user.setPharmacyId(obj["pharmacyId"].toInt(0));
        if (obj.contains("registrationDate")) {
            user.setRegistrationDate(QDate::fromString(obj["registrationDate"].toString(), "yyyy-MM-dd"));
        }
        employees.append(user);
    }

    // Загрузка лекарств
    QJsonArray medicinesArray = root["medicines"].toArray();
    medicines.clear();
    for (const QJsonValue& value : medicinesArray) {
        QJsonObject obj = value.toObject();
        QDate expirationDate = QDate::currentDate();
        if (obj.contains("expirationDate")) {
            expirationDate = QDate::fromString(obj["expirationDate"].toString(), "yyyy-MM-dd");
        }
        PharmacyItem item(
            obj["title"].toString(),
            obj["price"].toDouble(),
            obj["recipeRequired"].toBool(),
            expirationDate,
            obj["quantity"].toInt()
            );
        item.setPharmacyId(obj["pharmacyId"].toInt());
        medicines.append(item);
    }

    // Загрузка склада
    QJsonArray warehouseArray = root["warehouse"].toArray();
    warehouseItems.clear();
    if (centralWarehouse) {
        centralWarehouse->clearMedicines();
    }
    for (const QJsonValue& value : warehouseArray) {
        QJsonObject obj = value.toObject();
        QDate expirationDate = QDate::currentDate();
        if (obj.contains("expirationDate")) {
            expirationDate = QDate::fromString(obj["expirationDate"].toString(), "yyyy-MM-dd");
        }
        PharmacyItem item(
            obj["title"].toString(),
            obj["price"].toDouble(),
            obj["recipeRequired"].toBool(),
            expirationDate,
            obj["quantity"].toInt()
            );
        item.setPharmacyId(0);
        warehouseItems.append(item);
        if (centralWarehouse) {
            centralWarehouse->addMedicine(item);
        }
    }

    // Загрузка аптек
    QJsonArray pharmaciesArray = root["pharmacies"].toArray();
    pharmacies.clear();
    for (const QJsonValue& value : pharmaciesArray) {
        QJsonObject obj = value.toObject();
        Pharmacy pharmacy(
            obj["id"].toInt(),
            obj["address"].toString(),
            obj["size"].toDouble(0.0),
            obj["maxCapacity"].toInt(0)
            );
        pharmacies.append(pharmacy);
    }

    // Загрузка балансов покупателей
    QJsonObject balances = root["buyerBalances"].toObject();
    buyerBalances.clear();
    for (const QString& key : balances.keys()) {
        buyerBalances[key] = balances[key].toDouble();
    }

    // Загрузка доходов аптек
    QJsonObject revenues = root["pharmacyRevenues"].toObject();
    pharmacyRevenues.clear();
    for (const QString& key : revenues.keys()) {
        pharmacyRevenues[key.toInt()] = revenues[key].toDouble();
    }

    // Загрузка рюкзаков покупателей
    QJsonObject backpacks = root["buyerBackpacks"].toObject();
    buyerBackpacks.clear();
    for (const QString &key : backpacks.keys()) {
        QJsonArray items = backpacks[key].toArray();
        QStringList itemList;
        for (const QJsonValue &value : items) {
            itemList.append(value.toString());
        }
        buyerBackpacks[key] = itemList;
    }

    // Загрузка истории покупок
    QJsonObject histories = root["buyerPurchaseHistory"].toObject();
    buyerPurchaseHistory.clear();
    for (const QString &key : histories.keys()) {
        QJsonArray purchases = histories[key].toArray();
        QList<Purchase> purchaseList;
        for (const QJsonValue &value : purchases) {
            QJsonObject obj = value.toObject();
            Purchase purchase;
            purchase.timestamp = QDateTime::fromString(obj["timestamp"].toString(), "yyyy-MM-dd HH:mm:ss");
            purchase.medicineTitle = obj["medicineTitle"].toString();
            purchase.amount = obj["amount"].toDouble();
            purchaseList.append(purchase);
        }
        buyerPurchaseHistory[key] = purchaseList;
    }

    Logger::instance().log("JsonManager", QString("Загружено из data.json: %1 сотрудников, %2 лекарств, %3 на складе, %4 аптек")
                                              .arg(employees.size()).arg(medicines.size()).arg(warehouseItems.size()).arg(pharmacies.size()));
}

bool JsonManager::saveAllToJson() {
    QJsonObject root;

    // Сериализация списка сотрудников
    QJsonArray employeeArray;
    for (const User &user : employees) {
        QJsonObject obj;
        obj["role"] = UserRoleHelper::toString(user.getRole());
        obj["login"] = user.getLogin();
        obj["password"] = user.getPassword();
        obj["fullName"] = user.getFullName();
        obj["email"] = user.getEmail();
        obj["pharmacyId"] = user.getPharmacyId();
        if (user.getRegistrationDate().isValid()) {
            obj["registrationDate"] = user.getRegistrationDate().toString("yyyy-MM-dd");
        }
        employeeArray.append(obj);
    }
    root["employees"] = employeeArray;

    // Сериализация списка лекарств
    QJsonArray medicineArray;
    for (const PharmacyItem &item : medicines) {
        QJsonObject obj;
        obj["title"] = item.getTitle();
        obj["price"] = item.getPrice();
        obj["quantity"] = item.getQuantity();
        obj["pharmacyId"] = item.getPharmacyId();
        obj["recipeRequired"] = item.isRecipeRequired();
        obj["expirationDate"] = item.getExpirationDate().toString("yyyy-MM-dd");
        medicineArray.append(obj);
    }
    root["medicines"] = medicineArray;

    // Сериализация списка склада
    QJsonArray warehouseArray;
    if (centralWarehouse) {
        for (const PharmacyItem &item : centralWarehouse->getMedicines()) {
            QJsonObject obj;
            obj["title"] = item.getTitle();
            obj["price"] = item.getPrice();
            obj["quantity"] = item.getQuantity();
            obj["pharmacyId"] = item.getPharmacyId();
            obj["recipeRequired"] = item.isRecipeRequired();
            obj["expirationDate"] = item.getExpirationDate().toString("yyyy-MM-dd");
            warehouseArray.append(obj);
        }
    } else {
        for (const PharmacyItem &item : warehouseItems) {
            QJsonObject obj;
            obj["title"] = item.getTitle();
            obj["price"] = item.getPrice();
            obj["quantity"] = item.getQuantity();
            obj["pharmacyId"] = item.getPharmacyId();
            obj["recipeRequired"] = item.isRecipeRequired();
            obj["expirationDate"] = item.getExpirationDate().toString("yyyy-MM-dd");
            warehouseArray.append(obj);
        }
    }
    root["warehouse"] = warehouseArray;

    // Сериализация списка аптек
    QJsonArray pharmacyArray;
    for (const Pharmacy &pharmacy : pharmacies) {
        QJsonObject obj;
        obj["id"] = pharmacy.getId();
        obj["address"] = pharmacy.getAddress();
        obj["size"] = pharmacy.getSize();
        obj["maxCapacity"] = pharmacy.getMaxCapacity();
        pharmacyArray.append(obj);
    }
    root["pharmacies"] = pharmacyArray;

    // Сериализация балансов покупателей
    QJsonObject balances;
    for (const QString &key : buyerBalances.keys()) {
        balances[key] = buyerBalances[key];
    }
    root["buyerBalances"] = balances;

    // Сериализация доходов аптек
    QJsonObject revenues;
    for (const int &key : pharmacyRevenues.keys()) {
        revenues[QString::number(key)] = pharmacyRevenues[key];
    }
    root["pharmacyRevenues"] = revenues;

    // Сериализация рюкзаков покупателей
    QJsonObject backpacks;
    for (const QString &key : buyerBackpacks.keys()) {
        QJsonArray items;
        for (const QString &item : buyerBackpacks[key]) {
            items.append(item);
        }
        backpacks[key] = items;
    }
    root["buyerBackpacks"] = backpacks;

    // Сериализация истории покупок
    QJsonObject histories;
    for (const QString &key : buyerPurchaseHistory.keys()) {
        QJsonArray purchases;
        for (const Purchase &purchase : buyerPurchaseHistory[key]) {
            QJsonObject obj;
            obj["timestamp"] = purchase.timestamp.toString("yyyy-MM-dd HH:mm:ss");
            obj["medicineTitle"] = purchase.medicineTitle;
            obj["amount"] = purchase.amount;
            purchases.append(obj);
        }
        histories[key] = purchases;
    }
    root["buyerPurchaseHistory"] = histories;

    // Запись данных в файл
    QFile file("data.json");
    QFileInfo fileInfo(file);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            Logger::instance().log("JsonManager", "Ошибка: Не удалось создать директорию для data.json");
            QMessageBox::critical(nullptr, "Ошибка", "Не удалось создать директорию для data.json!");
            return false;
        }
    }
    if (!fileInfo.isWritable() && file.exists()) {
        Logger::instance().log("JsonManager", "Ошибка: Нет прав на запись в файл data.json");
        QMessageBox::critical(nullptr, "Ошибка", "Нет прав на запись в файл data.json!");
        return false;
    }
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().log("JsonManager", QString("Не удалось открыть файл data.json для записи: %1").arg(file.errorString()));
        QMessageBox::critical(nullptr, "Ошибка", QString("Не удалось открыть data.json: %1").arg(file.errorString()));
        return false;
    }

    QJsonDocument doc(root);
    if (file.write(doc.toJson()) == -1) {
        Logger::instance().log("JsonManager", QString("Ошибка при записи данных в файл data.json: %1").arg(file.errorString()));
        QMessageBox::critical(nullptr, "Ошибка", QString("Ошибка записи в data.json: %1").arg(file.errorString()));
        file.close();
        return false;
    }
    file.close();
    Logger::instance().log("JsonManager", "Данные успешно сохранены в data.json");
    return true;
}

void JsonManager::addToBackpack(const QString& login, const QString& medicineTitle) {
    buyerBackpacks[login].append(medicineTitle);
    if (!saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения рюкзака для %1").arg(login));
        buyerBackpacks[login].removeLast(); // Откат
    }
}

void JsonManager::removeFromBackpack(const QString& login, const QString& medicineTitle) {
    QStringList& backpack = buyerBackpacks[login];
    if (backpack.contains(medicineTitle)) {
        backpack.removeOne(medicineTitle);
        if (!saveAllToJson()) {
            Logger::instance().log("JsonManager", QString("Ошибка сохранения рюкзака для %1").arg(login));
            backpack.append(medicineTitle); // Откат
        }
    }
}

QStringList JsonManager::getBackpack(const QString& login) const {
    return buyerBackpacks.value(login, QStringList());
}

void JsonManager::setBuyerBalance(const QString& login, double balance) {
    double oldBalance = buyerBalances[login];
    buyerBalances[login] = balance;
    if (!saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения баланса для %1").arg(login));
        buyerBalances[login] = oldBalance; // Откат
    }
}

QList<JsonManager::Purchase> JsonManager::getPurchaseHistory(const QString& login) const {
    return buyerPurchaseHistory.value(login, QList<Purchase>());
}

void JsonManager::addMedicine(const PharmacyItem& item) {
    for (PharmacyItem& existing : warehouseItems) {
        if (existing.getTitle() == item.getTitle() &&
            existing.getPrice() == item.getPrice() &&
            existing.isRecipeRequired() == item.isRecipeRequired() &&
            existing.getExpirationDate() == item.getExpirationDate()) {
            int oldQuantity = existing.getQuantity();
            existing.setQuantity(oldQuantity + item.getQuantity());
            if (centralWarehouse) {
                centralWarehouse->addMedicine(existing);
            }
            if (saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Добавлено %1 единиц %2 на склад").arg(item.getQuantity()).arg(item.getTitle()));
            } else {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения %1 на склад").arg(item.getTitle()));
                existing.setQuantity(oldQuantity); // Откат
                if (centralWarehouse) {
                    centralWarehouse->addMedicine(existing); // Восстановить в centralWarehouse
                }
            }
            return;
        }
    }
    PharmacyItem newItem = item;
    newItem.setPharmacyId(0);
    warehouseItems.append(newItem);
    if (centralWarehouse) {
        centralWarehouse->addMedicine(newItem);
    }
    if (saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Добавлено новое лекарство %1 на склад").arg(item.getTitle()));
    } else {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения нового лекарства %1").arg(item.getTitle()));
        warehouseItems.removeLast(); // Откат
        if (centralWarehouse) {
            centralWarehouse->removeMedicine(item.getTitle(), item.getQuantity()); // Откат в centralWarehouse
        }
    }
}

bool JsonManager::moveMedicineToPharmacy(const QString& title, int quantity, int pharmacyId) {
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
    for (PharmacyItem& warehouseItem : warehouseItems) {
        if (warehouseItem.getTitle() == title && warehouseItem.getQuantity() >= quantity) {
            int oldWarehouseQuantity = warehouseItem.getQuantity();
            warehouseItem.setQuantity(oldWarehouseQuantity - quantity);
            bool wasRemoved = false;
            if (warehouseItem.getQuantity() == 0) {
                wasRemoved = true;
                warehouseItems.removeOne(warehouseItem);
                if (centralWarehouse) {
                    for (const Pharmacy& pharmacy : pharmacies) {
                        if (pharmacy.getId() == pharmacyId) {
                            centralWarehouse->transferMedicine(title, quantity, pharmacyId, pharmacy.getMaxCapacity());
                            break;
                        }
                    }
                }
            }
            for (PharmacyItem& pharmacyItem : medicines) {
                if (pharmacyItem.getTitle() == title && pharmacyItem.getPharmacyId() == pharmacyId) {
                    int oldPharmacyQuantity = pharmacyItem.getQuantity();
                    pharmacyItem.setQuantity(oldPharmacyQuantity + quantity);
                    if (!saveAllToJson()) {
                        Logger::instance().log("JsonManager", QString("Ошибка сохранения перемещения %1 в аптеку %2").arg(title).arg(pharmacyId));
                        pharmacyItem.setQuantity(oldPharmacyQuantity); // Откат
                        if (wasRemoved) {
                            warehouseItems.append(warehouseItem);
                        } else {
                            warehouseItem.setQuantity(oldWarehouseQuantity);
                        }
                        return false;
                    }
                    Logger::instance().log("JsonManager", QString("Перемещено %1 единиц %2 в аптеку %3")
                                                              .arg(quantity).arg(title).arg(pharmacyId));
                    return true;
                }
            }
            PharmacyItem newItem = warehouseItem;
            newItem.setQuantity(quantity);
            newItem.setPharmacyId(pharmacyId);
            medicines.append(newItem);
            if (!saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения нового лекарства %1 в аптеку %2").arg(title).arg(pharmacyId));
                medicines.removeLast(); // Откат
                if (wasRemoved) {
                    warehouseItems.append(warehouseItem);
                } else {
                    warehouseItem.setQuantity(oldWarehouseQuantity);
                }
                return false;
            }
            Logger::instance().log("JsonManager", QString("Перемещено новое лекарство %1 (%2 единиц) в аптеку %3")
                                                      .arg(title).arg(quantity).arg(pharmacyId));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Недостаточно %1 (%2 единиц) на складе").arg(title).arg(quantity));
    return false;
}

bool JsonManager::removeMedicine(const QString& title, int pharmacyId) {
    if (title.isEmpty()) {
        Logger::instance().log("JsonManager", "Ошибка: Название лекарства не может быть пустым");
        return false;
    }
    bool removed = false;
    if (pharmacyId == 0) {
        QList<PharmacyItem> removedItems;
        for (int i = warehouseItems.size() - 1; i >= 0; --i) {
            if (warehouseItems[i].getTitle().compare(title, Qt::CaseInsensitive) == 0) {
                removedItems.append(warehouseItems[i]);
                warehouseItems.removeAt(i);
                removed = true;
            }
        }
        if (centralWarehouse && removed) {
            for (const PharmacyItem& item : removedItems) {
                centralWarehouse->removeMedicine(item.getTitle(), item.getQuantity());
            }
        }
        if (removed) {
            if (!saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения удаления %1 со склада").arg(title));
                warehouseItems.append(removedItems); // Откат
                if (centralWarehouse) {
                    for (const PharmacyItem& item : removedItems) {
                        centralWarehouse->addMedicine(item); // Восстановить в centralWarehouse
                    }
                }
                return false;
            }
            Logger::instance().log("JsonManager", QString("Удалено лекарство %1 со склада").arg(title));
            return true;
        }
        Logger::instance().log("JsonManager", QString("Ошибка: Лекарство %1 не найдено на складе").arg(title));
        return false;
    }
    QList<PharmacyItem> removedItems;
    for (int i = medicines.size() - 1; i >= 0; --i) {
        if (medicines[i].getTitle().compare(title, Qt::CaseInsensitive) == 0 && medicines[i].getPharmacyId() == pharmacyId) {
            removedItems.append(medicines[i]);
            medicines.removeAt(i);
            removed = true;
        }
    }
    if (removed) {
        if (!saveAllToJson()) {
            Logger::instance().log("JsonManager", QString("Ошибка сохранения удаления %1 из аптеки %2").arg(title).arg(pharmacyId));
            medicines.append(removedItems); // Откат
            return false;
        }
        Logger::instance().log("JsonManager", QString("Удалено лекарство %1 из аптеки %2").arg(title).arg(pharmacyId));
        return true;
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Лекарство %1 не найдено в аптеке %2").arg(title).arg(pharmacyId));
    return false;
}

bool JsonManager::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            Pharmacy removedPharmacy = pharmacies[i];
            pharmacies.removeAt(i);
            pharmacyRevenues.remove(id);
            if (!saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения удаления аптеки ID %1").arg(id));
                pharmacies.insert(i, removedPharmacy); // Откат
                pharmacyRevenues[id] = 0.0; // Восстановить доход
                return false;
            }
            Logger::instance().log("JsonManager", QString("Удалена аптека ID %1").arg(id));
            return true;
        }
    }
    Logger::instance().log("JsonManager", QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    return false;
}

void JsonManager::addPharmacy(const Pharmacy& pharmacy) {
    for (const Pharmacy& existing : pharmacies) {
        if (existing.getId() == pharmacy.getId()) {
            Logger::instance().log("JsonManager", QString("Ошибка: Аптека с ID %1 уже существует").arg(pharmacy.getId()));
            return;
        }
    }
    pharmacies.append(pharmacy);
    if (!saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения аптеки ID %1").arg(pharmacy.getId()));
        pharmacies.removeLast();
        return;
    }
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
    double oldBalance = buyerBalances[login];
    buyerBalances[login] += amount;
    if (!saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения пополнения баланса для %1").arg(login));
        buyerBalances[login] = oldBalance; // Откат
        return false;
    }
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
            int oldQuantity = item.getQuantity();
            double oldBalance = buyerBalances[buyerLogin];
            double oldRevenue = pharmacyRevenues[pharmacyId];
            item.setQuantity(oldQuantity - quantity);
            if (!buyerLogin.isEmpty()) {
                buyerBalances[buyerLogin] -= cost;
                Purchase purchase;
                purchase.timestamp = QDateTime::currentDateTime();
                purchase.medicineTitle = medicineTitle;
                purchase.amount = cost;
                buyerPurchaseHistory[buyerLogin].append(purchase);
            }
            pharmacyRevenues[pharmacyId] += cost;
            HistoryManager::instance().addOperation(
                buyerLogin.isEmpty() ? "Продавец" : buyerLogin,
                "Продажа",
                QString("Куплено %1 (%2 единиц) за %3").arg(medicineTitle).arg(quantity).arg(cost),
                pharmacyId
                );
            if (!saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения покупки %1").arg(medicineTitle));
                item.setQuantity(oldQuantity); // Откат
                if (!buyerLogin.isEmpty()) {
                    buyerBalances[buyerLogin] = oldBalance; // Откат
                    buyerPurchaseHistory[buyerLogin].removeLast(); // Откат
                }
                pharmacyRevenues[pharmacyId] = oldRevenue; // Откат
                return false;
            }
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

QList<PharmacyItem> JsonManager::getWarehouseItems() const {
    Logger::instance().log("JsonManager", QString("Запрошен список лекарств на складе: %1").arg(warehouseItems.size()));
    return warehouseItems;
}

void JsonManager::addEmployee(const QString& role, const QString& login, const QString& password,
                              const QString& fullName, const QString& email, int pharmacyId) {
    if (login.isEmpty() || password.isEmpty() || fullName.isEmpty() || email.isEmpty()) {
        Logger::instance().log("JsonManager", "Ошибка: Все поля должны быть заполнены");
        QMessageBox::warning(nullptr, "Ошибка", "Все поля должны быть заполнены!");
        return;
    }
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        Logger::instance().log("JsonManager", QString("Ошибка: Неверный формат email %1").arg(email));
        QMessageBox::warning(nullptr, "Ошибка", "Неверный формат email!");
        return;
    }
    if (role == "Продавец") {
        if (pharmacyId == 0) {
            Logger::instance().log("JsonManager", "Ошибка: Продавец должен быть привязан к аптеке");
            QMessageBox::warning(nullptr, "Ошибка", "Выберите аптеку для продавца!");
            return;
        }
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
    for (const User& user : employees) {
        if (user.getLogin() == login) {
            Logger::instance().log("JsonManager", QString("Ошибка: Логин %1 уже занят").arg(login));
            QMessageBox::warning(nullptr, "Ошибка", "Логин уже занят!");
            return;
        }
    }
    User employee(UserRoleHelper::fromString(role), login, password, fullName, email);
    if (role == "Продавец") {
        employee.setPharmacyId(pharmacyId);
    }
    employees.append(employee);
    if (!saveAllToJson()) {
        Logger::instance().log("JsonManager", QString("Ошибка сохранения сотрудника %1").arg(login));
        employees.removeLast(); // Откат
        return;
    }
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
            User removedEmployee = employees[i];
            double removedBalance = buyerBalances[login];
            employees.removeAt(i);
            buyerBalances.remove(login);
            if (!saveAllToJson()) {
                Logger::instance().log("JsonManager", QString("Ошибка сохранения удаления сотрудника %1").arg(login));
                employees.insert(i, removedEmployee); // Откат
                buyerBalances[login] = removedBalance; // Откат
                return false;
            }
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

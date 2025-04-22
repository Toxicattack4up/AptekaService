#include "JsonManager.h"
#include "logger.h"
#include "UserRoleHelper.h" // Добавлено явное включение

JsonManager::JsonManager() {
    QJsonObject json = loadJson();

    // Initialize admin if JSON is empty
    if (json.isEmpty() || !json.contains("Users") || !json.contains("Pharmacies") || !json.contains("Medicines") || !json.contains("Balances") || !json.contains("Revenues") || !json.contains("History")) {
        User admin;
        admin.setRole(UserRole::Administrator);
        admin.setLogin("admin");
        admin.setPassword(hashPassword("admin123"));
        admin.setFullName("Админ Админович");
        admin.setEmail("admin@example.com");
        admin.setRegistrationDate(QDateTime::currentDateTime());
        employees.append(admin);
        buyerBalances["admin"] = 0.0;
        saveAllToJson();
        Logger::instance().log("JsonManager", "Initialized JSON with default admin");
        return;
    }

    // Load users
    QJsonArray usersArray = json["Users"].toArray();
    for (const QJsonValue &value : usersArray) {
        QJsonObject obj = value.toObject();
        User user;
        user.setLogin(obj["login"].toString());
        user.setPassword(obj["password"].toString());
        user.setFullName(obj["fullName"].toString());
        user.setEmail(obj["email"].toString());
        user.setRegistrationDate(QDateTime::fromString(obj["registrationDate"].toString(), Qt::ISODate));
        user.setRole(UserRoleHelper::fromString(obj["role"].toString()));
        employees.append(user);
    }

    // Load pharmacies
    QJsonArray pharmaciesArray = json["Pharmacies"].toArray();
    for (const QJsonValue &value : pharmaciesArray) {
        QJsonObject obj = value.toObject();
        Pharmacy ph(obj["id"].toInt(),
                    obj["address"].toString(),
                    obj["size"].toDouble(),
                    obj["maxCapacity"].toInt());
        pharmacies.append(ph);
    }

    // Load medicines
    QJsonArray medicinesArray = json["Medicines"].toArray();
    for (const QJsonValue &value : medicinesArray) {
        QJsonObject obj = value.toObject();
        PharmacyItem item(obj["title"].toString(),
                          obj["price"].toDouble(),
                          obj["recipe"].toBool(),
                          QDate::fromString(obj["expiration_date"].toString(), Qt::ISODate),
                          obj["quantity"].toInt());
        item.pharmacyId = obj["pharmacyId"].toInt();
        medicines.append(item);
    }

    // Load balances
    QJsonObject balancesObj = json["Balances"].toObject();
    for (const QString &login : balancesObj.keys()) {
        buyerBalances[login] = balancesObj[login].toDouble();
    }

    // Load revenues
    QJsonObject revenuesObj = json["Revenues"].toObject();
    for (const QString &id : revenuesObj.keys()) {
        pharmacyRevenues[id.toInt()] = revenuesObj[id].toDouble();
    }

    // Load purchase history
    purchaseHistory = json["History"].toArray();
}

const QList<User> &JsonManager::getEmployee() const {
    return employees;
}

const QList<Pharmacy> &JsonManager::getPharmacy() const {
    return pharmacies;
}

const QList<PharmacyItem> &JsonManager::getMedicine() const {
    return medicines;
}

bool JsonManager::makePurchase(const QString &medicineTitle, int quantity, int pharmacyId, const QString &buyerLogin) {
    for (PharmacyItem &item : medicines) {
        if (item.getTitle() == medicineTitle && item.pharmacyId == pharmacyId) {
            if (item.getQuantity() < quantity) {
                QMessageBox::warning(nullptr, "Ошибка", QString("Недостаточно медикамента %1 в аптеке %2. Доступно: %3")
                                                            .arg(medicineTitle).arg(pharmacyId).arg(item.getQuantity()));
                Logger::instance().log("Purchase", QString("Failed: Insufficient stock of %1 in pharmacy %2")
                                                       .arg(medicineTitle).arg(pharmacyId));
                return false;
            }
            if (quantity <= 0) {
                QMessageBox::warning(nullptr, "Ошибка", "Количество должно быть положительным");
                Logger::instance().log("Purchase", "Failed: Invalid quantity");
                return false;
            }

            // Check buyer balance if applicable
            double totalCost = item.getPrice() * quantity;
            if (!buyerLogin.isEmpty() && buyerBalances[buyerLogin] < totalCost) {
                QMessageBox::warning(nullptr, "Ошибка", QString("Недостаточно средств на балансе для покупки %1")
                                                            .arg(medicineTitle));
                Logger::instance().log("Purchase", QString("Failed: Insufficient balance for %1 by %2")
                                                       .arg(medicineTitle).arg(buyerLogin));
                return false;
            }

            // Update stock and balance
            item.setQuantity(item.getQuantity() - quantity);
            if (!buyerLogin.isEmpty()) {
                buyerBalances[buyerLogin] -= totalCost;
            }
            pharmacyRevenues[pharmacyId] = pharmacyRevenues.value(pharmacyId, 0.0) + totalCost;

            // Log purchase
            Logger::instance().log("Purchase", QString("Bought %1 units of %2 from pharmacy %3 by %4")
                                                   .arg(quantity).arg(medicineTitle).arg(pharmacyId).arg(buyerLogin.isEmpty() ? "seller" : buyerLogin));

            // Save to history
            QJsonObject historyEntry;
            historyEntry["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            historyEntry["pharmacyId"] = pharmacyId;
            historyEntry["medicine"] = medicineTitle;
            historyEntry["quantity"] = quantity;
            historyEntry["totalCost"] = totalCost;
            historyEntry["buyer"] = buyerLogin.isEmpty() ? "seller" : buyerLogin;
            purchaseHistory.append(historyEntry);

            if (item.getQuantity() == 0) {
                medicines.removeOne(item);
                Logger::instance().log("Purchase", QString("Removed %1 from pharmacy %2 (stock depleted)")
                                                       .arg(medicineTitle).arg(pharmacyId));
            }
            saveAllToJson();
            QMessageBox::information(nullptr, "Успешно", QString("Куплено %1 шт. медикамента %2 из аптеки %3")
                                                             .arg(quantity).arg(medicineTitle).arg(pharmacyId));
            return true;
        }
    }
    QMessageBox::warning(nullptr, "Ошибка", QString("Медикамент %1 не найден в аптеке %2")
                                                .arg(medicineTitle).arg(pharmacyId));
    Logger::instance().log("Purchase", QString("Failed: %1 not found in pharmacy %2")
                                           .arg(medicineTitle).arg(pharmacyId));
    return false;
}

QString JsonManager::hashPassword(const QString &password) {
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

QJsonObject JsonManager::loadJson() {
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: Не удалось открыть файл:" << file.errorString();
        Logger::instance().log("JsonManager", QString("Failed to open %1: %2").arg(filePath, file.errorString()));
        return QJsonObject();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Ошибка: Некорректный JSON.";
        Logger::instance().log("JsonManager", "Invalid JSON format");
        return QJsonObject();
    }

    return doc.object();
}

void JsonManager::saveAllToJson() {
    QJsonObject json;

    // Save users
    QJsonArray usersArray;
    for (const User &user : employees) {
        QJsonObject userObject;
        userObject["role"] = UserRoleHelper::toString(user.getRole());
        userObject["login"] = user.getLogin();
        userObject["password"] = user.getPassword();
        userObject["fullName"] = user.getFullName();
        userObject["email"] = user.getEmail();
        userObject["registrationDate"] = user.getRegistrationDate().toString(Qt::ISODate);
        usersArray.append(userObject);
    }
    json["Users"] = usersArray;

    // Save pharmacies
    QJsonArray pharmaciesArray;
    for (const Pharmacy &ph : pharmacies) {
        QJsonObject phObject;
        phObject["id"] = ph.getId();
        phObject["address"] = ph.getAddress();
        phObject["size"] = ph.getSize();
        phObject["maxCapacity"] = ph.getMaxCapacity();
        pharmaciesArray.append(phObject);
    }
    json["Pharmacies"] = pharmaciesArray;

    // Save medicines
    QJsonArray medicinesArray;
    for (const PharmacyItem &item : medicines) {
        QJsonObject itemObject;
        itemObject["title"] = item.getTitle();
        itemObject["price"] = item.getPrice();
        itemObject["recipe"] = item.isRecipeRequired();
        itemObject["expiration_date"] = item.getExpirationDate().toString(Qt::ISODate);
        itemObject["quantity"] = item.getQuantity();
        itemObject["pharmacyId"] = item.pharmacyId;
        medicinesArray.append(itemObject);
    }
    json["Medicines"] = medicinesArray;

    // Save balances
    QJsonObject balancesObj;
    for (const QString &login : buyerBalances.keys()) {
        balancesObj[login] = buyerBalances[login];
    }
    json["Balances"] = balancesObj;

    // Save revenues
    QJsonObject revenuesObj;
    for (const int &id : pharmacyRevenues.keys()) {
        revenuesObj[QString::number(id)] = pharmacyRevenues[id];
    }
    json["Revenues"] = revenuesObj;

    // Save history
    json["History"] = purchaseHistory;

    // Write to file
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: Не удалось записать файл:" << file.errorString();
        Logger::instance().log("JsonManager", QString("Failed to write %1: %2").arg(filePath, file.errorString()));
        return;
    }
    QJsonDocument doc(json);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    Logger::instance().log("JsonManager", "Saved all data to JSON");
}

QString JsonManager::validateUser(const QString &login, const QString &password) {
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "Ошибка", "Логин и пароль не могут быть пустыми");
        Logger::instance().log("Auth", "Failed: Empty login or password");
        return "";
    }

    QString hashedPassword = hashPassword(password);
    for (const User &user : employees) {
        if (user.getLogin() == login && user.getPassword() == hashedPassword) {
            Logger::instance().log("Auth", QString("Successful login for %1").arg(login));
            return UserRoleHelper::toString(user.getRole());
        }
    }

    QMessageBox::warning(nullptr, "Ошибка", "Такого пользователя не существует");
    Logger::instance().log("Auth", QString("Failed login attempt for %1").arg(login));
    return "";
}

void JsonManager::addEmployee(const QString &_role, const QString &_login, const QString &_password,
                              const QString &_fullName, const QString &_email) {
    UserRole role = UserRoleHelper::fromString(_role);
    if (role == UserRole::Unknown) {
        qDebug() << "Ошибка: Неизвестная роль";
        QMessageBox::warning(nullptr, "Ошибка", "Неизвестная роль");
        Logger::instance().log("Employee", QString("Failed to add user: Unknown role %1").arg(_role));
        return;
    }

    for (const User &user : employees) {
        if (user.getLogin() == _login) {
            qDebug() << "Ошибка: Логин" << _login << "уже существует";
            QMessageBox::warning(nullptr, "Ошибка", "Логин уже существует");
            Logger::instance().log("Employee", QString("Failed to add user: Login %1 already exists").arg(_login));
            return;
        }
    }

    User newUser;
    newUser.setRole(role);
    newUser.setLogin(_login);
    newUser.setPassword(hashPassword(_password));
    newUser.setFullName(_fullName);
    newUser.setEmail(_email);
    newUser.setRegistrationDate(QDateTime::currentDateTime());
    employees.append(newUser);
    if (role == UserRole::Buyer) {
        buyerBalances[_login] = 0.0;
    }
    saveAllToJson();
    QMessageBox::information(nullptr, "Успешно", "Пользователь успешно добавлен");
    Logger::instance().log("Employee", QString("Added user %1 with role %2").arg(_login, _role));
}

void JsonManager::removeEmployee(const QString &login) {
    if (login.isEmpty()) {
        qDebug() << "Ошибка: Логин не может быть пустым";
        QMessageBox::warning(nullptr, "Ошибка", "Логин не может быть пустым");
        Logger::instance().log("Employee", "Failed to remove user: Empty login");
        return;
    }

    for (int i = 0; i < employees.size(); ++i) {
        if (employees[i].getLogin() == login) {
            employees.removeAt(i);
            buyerBalances.remove(login);
            saveAllToJson();
            QMessageBox::information(nullptr, "Успешно", "Пользователь успешно удален");
            Logger::instance().log("Employee", QString("Removed user %1").arg(login));
            return;
        }
    }
    qDebug() << "Ошибка: Пользователь" << login << "не найден";
    QMessageBox::warning(nullptr, "Ошибка", "Пользователь не найден");
    Logger::instance().log("Employee", QString("Failed to remove user: %1 not found").arg(login));
}

QList<User> JsonManager::searchEmployee(const QString &_role, const QString &_login, const QString &_password,
                                        const QString &_fullName, const QString &_email) {
    QList<User> result;
    UserRole role = UserRoleHelper::fromString(_role);

    for (const User &user : employees) {
        bool match = true;
        if (!_role.isEmpty() && user.getRole() != role) match = false;
        if (!_login.isEmpty() && user.getLogin() != _login) match = false;
        if (!_password.isEmpty() && user.getPassword() != hashPassword(_password)) match = false;
        if (!_fullName.isEmpty() && user.getFullName() != _fullName) match = false;
        if (!_email.isEmpty() && user.getEmail() != _email) match = false;

        if (match) {
            result.append(user);
        }
    }
    Logger::instance().log("Employee", QString("Searched employees with criteria role=%1, login=%2").arg(_role, _login));
    return result;
}

void JsonManager::addPharmacy(const Pharmacy &pharmacy) {
    for (const Pharmacy &ph : pharmacies) {
        if (ph.getId() == pharmacy.getId()) {
            qDebug() << "Ошибка: Аптека с ID" << pharmacy.getId() << "уже существует";
            QMessageBox::warning(nullptr, "Ошибка", QString("Аптека с ID %1 уже существует").arg(pharmacy.getId()));
            Logger::instance().log("Pharmacy", QString("Failed to add pharmacy: ID %1 already exists").arg(pharmacy.getId()));
            return;
        }
    }
    pharmacies.append(pharmacy);
    pharmacyRevenues[pharmacy.getId()] = 0.0;
    saveAllToJson();
    QMessageBox::information(nullptr, "Успешно", "Аптека успешно добавлена");
    Logger::instance().log("Pharmacy", QString("Added pharmacy with ID %1").arg(pharmacy.getId()));
}

void JsonManager::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            pharmacies.removeAt(i);
            pharmacyRevenues.remove(id);
            saveAllToJson();
            QMessageBox::information(nullptr, "Успешно", "Аптека удалена");
            Logger::instance().log("Pharmacy", QString("Removed pharmacy with ID %1").arg(id));
            return;
        }
    }
    qDebug() << "Ошибка: Аптека с ID" << id << "не найдена";
    QMessageBox::warning(nullptr, "Ошибка", QString("Аптека с ID %1 не найдена").arg(id));
    Logger::instance().log("Pharmacy", QString("Failed to remove pharmacy: ID %1 not found").arg(id));
}

QList<Pharmacy> JsonManager::searchPharmacy(int id, const QString &address) {
    QList<Pharmacy> result;
    for (const Pharmacy &ph : pharmacies) {
        bool match = true;
        if (id != -1 && ph.getId() != id) match = false;
        if (!address.isEmpty() && ph.getAddress() != address) match = false;
        if (match) {
            result.append(ph);
        }
    }
    Logger::instance().log("Pharmacy", QString("Searched pharmacies with ID=%1, address=%2").arg(id).arg(address));
    return result;
}

void JsonManager::addMedicine(const PharmacyItem &item, int pharmacyId) {
    for (const PharmacyItem &med : medicines) {
        if (med.getTitle() == item.getTitle() && med.pharmacyId == pharmacyId) {
            qDebug() << "Ошибка: Медикамент" << item.getTitle() << "уже существует в аптеке" << pharmacyId;
            QMessageBox::warning(nullptr, "Ошибка", QString("Медикамент %1 уже существует в аптеке %2").arg(item.getTitle()).arg(pharmacyId));
            Logger::instance().log("Medicine", QString("Failed to add medicine: %1 already exists in pharmacy %2").arg(item.getTitle()).arg(pharmacyId));
            return;
        }
    }
    PharmacyItem newItem = item;
    newItem.pharmacyId = pharmacyId;
    medicines.append(newItem);
    saveAllToJson();
    QMessageBox::information(nullptr, "Успешно", QString("Медикамент %1 добавлен в аптеку %2").arg(item.getTitle()).arg(pharmacyId));
    Logger::instance().log("Medicine", QString("Added medicine %1 to pharmacy %2").arg(item.getTitle()).arg(pharmacyId));
}

void JsonManager::removeMedicine(const QString &title) {
    if (title.isEmpty()) {
        qDebug() << "Ошибка: Название медикамента не может быть пустым";
        Logger::instance().log("Medicine", "Failed to remove medicine: Empty title");
        return;
    }
    for (int i = 0; i < medicines.size(); ++i) {
        if (medicines[i].getTitle() == title) {
            int pharmacyId = medicines[i].pharmacyId;
            medicines.removeAt(i);
            saveAllToJson();
            QMessageBox::information(nullptr, "Успешно", QString("Лекарство %1 успешно удалено").arg(title));
            Logger::instance().log("Medicine", QString("Removed medicine %1 from pharmacy %2").arg(title).arg(pharmacyId));
            return;
        }
    }
    qDebug() << "Ошибка: Медикамент" << title << "не найден";
    QMessageBox::warning(nullptr, "Ошибка", QString("Лекарство %1 не найдено").arg(title));
    Logger::instance().log("Medicine", QString("Failed to remove medicine: %1 not found").arg(title));
}

QList<PharmacyItem> JsonManager::searchMedicine(const QString &title) {
    QList<PharmacyItem> result;
    for (const PharmacyItem &item : medicines) {
        if (!title.isEmpty() && item.getTitle() == title) {
            result.append(item);
        }
    }
    Logger::instance().log("Medicine", QString("Searched medicines with title=%1").arg(title));
    return result;
}

double JsonManager::getBuyerBalance(const QString &login) const {
    return buyerBalances.value(login, 0.0);
}

bool JsonManager::depositBalance(const QString &login, double amount) {
    if (amount <= 0) {
        QMessageBox::warning(nullptr, "Ошибка", "Сумма пополнения должна быть положительной");
        Logger::instance().log("Balance", QString("Failed to deposit for %1: Invalid amount %2").arg(login).arg(amount));
        return false;
    }
    buyerBalances[login] = buyerBalances.value(login, 0.0) + amount;
    saveAllToJson();
    QMessageBox::information(nullptr, "Успешно", QString("Баланс пополнен на %1 для %2").arg(amount).arg(login));
    Logger::instance().log("Balance", QString("Deposited %1 to %2's balance").arg(amount).arg(login));
    return true;
}

double JsonManager::getPharmacyRevenue(int pharmacyId) const {
    return pharmacyRevenues.value(pharmacyId, 0.0);
}

QJsonArray JsonManager::getPurchaseHistory(int pharmacyId) const {
    QJsonArray result;
    for (const QJsonValue &value : purchaseHistory) {
        QJsonObject obj = value.toObject();
        if (obj["pharmacyId"].toInt() == pharmacyId) {
            result.append(obj);
        }
    }
    Logger::instance().log("History", QString("Retrieved purchase history for pharmacy %1").arg(pharmacyId));
    return result;
}

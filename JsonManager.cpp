#include "JsonManager.h"

JsonManager::JsonManager() {
    QJsonObject json = loadJson();

    // Если файл пуст, добавляем администратора
    if (json.isEmpty() || !json.contains("Users") || !json.contains("Pharmacies") || !json.contains("Medicines")) {
        User admin;
        admin.setRole(UserRole::Administrator);
        admin.setLogin("admin");
        admin.setPassword(hashPassword("admin123"));
        admin.setFullName("Админ Админович");
        admin.setEmail("admin@example.com");
        admin.setRegistrationDate(QDateTime::currentDateTime());
        employees.append(admin);
        saveAllToJson(); // Сохраняем с администратором
        return;
    }

    // Загружаем пользователей
    QJsonArray usersArray = json["Users"].toArray();
    for (const QJsonValue &value : usersArray) {
        QJsonObject obj = value.toObject();
        User user;
        user.setLogin(obj["login"].toString());
        user.setPassword(obj["password"].toString());
        user.setFullName(obj["fullName"].toString());
        user.setEmail(obj["email"].toString());
        user.setRegistrationDate(QDateTime::fromString(obj["registrationDate"].toString(), Qt::ISODate));
        user.setRole(UserRoleHelper::fromString(obj["role"].toString())); // Предполагается, что есть метод для конвертации строки в UserRole
        employees.append(user);
    }

    // Загружаем аптеки
    QJsonArray pharmaciesArray = json["Pharmacies"].toArray();
    for (const QJsonValue &value : pharmaciesArray) {
        QJsonObject obj = value.toObject();
        Pharmacy ph(obj["id"].toInt(),
                    obj["address"].toString(),
                    obj["size"].toDouble(),
                    obj["maxCapacity"].toInt());
        pharmacies.append(ph);
    }

    // Загружаем медикаменты
    QJsonArray medicinesArray = json["Medicines"].toArray();
    for (const QJsonValue &value : medicinesArray) {
        QJsonObject obj = value.toObject();
        PharmacyItem item(obj["title"].toString(),
                          obj["price"].toDouble(),
                          obj["recipe"].toBool(),
                          QDate::fromString(obj["expiration_date"].toString(), Qt::ISODate),
                          obj["quantity"].toInt());
        medicines.append(item);
    }
}

const QList<User> &JsonManager::getEmployee() const {
    return employees;
}
const QList<Pharmacy> &JsonManager::getPharmacy() const{
    return pharmacies;
}
const QList<PharmacyItem> &JsonManager::getMedicine() const{
    return medicines;
}

bool JsonManager::makePurchase(const QString &medicineTitle, int quantity){

    for (PharmacyItem &item : medicines) {
        if (item.getTitle() == medicineTitle) {
            // Проверяем, достаточно ли медикамента
            if (item.getQuantity() < quantity) {
                QMessageBox::warning(nullptr, "Ошибка", QString("Недостаточно медикамента %1. Доступно: %2").arg(medicineTitle).arg(item.getQuantity()));
                return false;
            }
            if (quantity <= 0) {
                QMessageBox::warning(nullptr, "Ошибка", "Количество должно быть положительным");
                return false;
            }

            // Уменьшаем количество
            item.setQuantity(item.getQuantity() - quantity);

            if (item.getQuantity() == 0) {
                removeMedicine(medicineTitle);
                qDebug() << "Медикамент" << medicineTitle << "удалён, так как количество стало 0";
            }
            // Сохраняем изменения в JSON
            saveAllToJson();

            QMessageBox::information(nullptr, "Успешно", QString("Куплено %1 шт. медикамента %2").arg(quantity).arg(medicineTitle));
            return true;
        }
    }

    // Медикамент не найден
    QMessageBox::warning(nullptr, "Ошибка", QString("Медикамент %1 не найден").arg(medicineTitle));
    return false;
}

// Хеширование пароля
QString JsonManager::hashPassword(const QString &password) {
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

// Загрузка JSON из файла
QJsonObject JsonManager::loadJson() {
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: Не удалось открыть файл:" << file.errorString();
        return QJsonObject();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Ошибка: Некорректный JSON.";
        return QJsonObject();
    }

    return doc.object();
}

// Сохранение всех данных в JSON
void JsonManager::saveAllToJson() {
    QJsonObject json;

    // Сохраняем пользователей
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

    // Сохраняем аптеки
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

    // Сохраняем медикаменты
    QJsonArray medicinesArray;
    for (const PharmacyItem &item : medicines) {
        QJsonObject itemObject;
        itemObject["title"] = item.getTitle();
        itemObject["price"] = item.getPrice();
        itemObject["recipe"] = item.isRecipeRequired();
        itemObject["expiration_date"] = item.getExpirationDate().toString(Qt::ISODate);
        itemObject["quantity"] = item.getQuantity();
        medicinesArray.append(itemObject);
    }
    json["Medicines"] = medicinesArray;

    // Записываем данные в файл
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка: Не удалось записать файл:" << file.errorString();
        return;
    }
    QJsonDocument doc(json);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

// Проверка логина и пароля
QString JsonManager::validateUser(const QString &login, const QString &password) {
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "Ошибка", "Логин и пароль не могут быть пустыми");
        return "";
    }

    QString hashedPassword = hashPassword(password);
    qDebug() << "Введенный логин:" << login;
    qDebug() << "Введенный пароль:" << password;
    qDebug() << "Хешированный пароль:" << hashedPassword;
    qDebug() << "Пользователи в employees" << employees.size();

    for (const User &user : employees) {
        if (user.getLogin() == login && user.getPassword() == hashedPassword) {
            return UserRoleHelper::toString(user.getRole());
        }
    }

    QMessageBox::warning(nullptr, "Ошибка", "Такого пользователя не существует");
    return "";
}


//_______СОТРУДНИКИ_______
// Добавление пользователя
void JsonManager::addEmployee(const QString &_role, const QString &_login, const QString &_password,
                              const QString &_fullName, const QString &_email) {
    UserRole role = UserRoleHelper::fromString(_role);
    if (role == UserRole::Unknown) {
        qDebug() << "Ошибка: Неизвестная роль";
        QMessageBox::warning(nullptr, "Ошибка", "Неизвестная роль");
        return;
    }

    for (const User &user : employees) {
        if (user.getLogin() == _login) {
            qDebug() << "Ошибка: Логин" << _login << "уже существует";
            QMessageBox::warning(nullptr, "Ошибка", "Логин уже существует");
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
    saveAllToJson(); // Сохраняем все данные
    QMessageBox::information(nullptr, "Успешно", "Пользователь успешно добавлен");
}

// Удаление пользователя
void JsonManager::removeEmployee(const QString &login) {
    if (login.isEmpty()) {
        qDebug() << "Ошибка: Логин не может быть пустым";
        QMessageBox::warning(nullptr, "Ошибка", "Логин не может быть пустым");
        return;
    }

    for (int i = 0; i < employees.size(); ++i) {
        if (employees[i].getLogin() == login) {
            employees.removeAt(i);
            saveAllToJson();
            return;
            QMessageBox::information(nullptr, "Успешно", "Пользователь успешно удален");
        }
    }
    qDebug() << "Ошибка: Пользователь" << login << "не найден";
    QMessageBox::warning(nullptr, "Ошибка", "Пользователь не найден");
    QMessageBox::information(nullptr, "Успешно", "Пользователь успешно удален");
}

// Поиск пользователей
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
    return result;
}
//________________________


//_______АПТЕКИ_______
// Добавление аптеки
void JsonManager::addPharmacy(const Pharmacy &pharmacy) {
    for (const Pharmacy &ph : pharmacies) {
        if (ph.getId() == pharmacy.getId()) {
            qDebug() << "Ошибка: Аптека с ID" << pharmacy.getId() << "уже существует";
            QMessageBox::warning(nullptr, "Ошибка", QString("Аптека с ID %1 уже существует").arg(pharmacy.getId()));
            return;
        }
    }
    pharmacies.append(pharmacy);
    QMessageBox::information(nullptr,"Успешно", "Аптека успешно добавлена");
    saveAllToJson();
}

// Удаление аптеки
void JsonManager::removePharmacy(int id) {
    for (int i = 0; i < pharmacies.size(); ++i) {
        if (pharmacies[i].getId() == id) {
            pharmacies.removeAt(i);
            QMessageBox::information(nullptr, "Успешно","Аптека удалена");
            saveAllToJson();
            return;
        }
    }
    qDebug() << "Ошибка: Аптека с ID" << id << "не найдена";
    QMessageBox::warning(nullptr, "Ошибка", QString("Аптека с ID %1 не найдена").arg(id));
}

// Поиск аптек
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
    return result;
}
//___________________

//______ЛЕКАРСТВА______
// Добавление медикамента
void JsonManager::addMedicine(const PharmacyItem &item) {
    for (const PharmacyItem &med : medicines) {
        if (med.getTitle() == item.getTitle()) {
            qDebug() << "Ошибка: Медикамент" << item.getTitle() << "уже существует";
            QMessageBox::warning(nullptr, "Ошибка", QString("Медикамент %1 уже существует").arg(item.getTitle()));
            return;
        }
    }
    medicines.append(item);
    saveAllToJson();
    QMessageBox::information(nullptr, "Успешно", "Медикамент добавлен");
}

// Удаление медикамента
void JsonManager::removeMedicine(const QString &title) {
    if (title.isEmpty()) {
        qDebug() << "Ошибка: Название медикамента не может быть пустым";

        return;
    }
    for (int i = 0; i < medicines.size(); ++i) {
        if (medicines[i].getTitle() == title) {
            medicines.removeAt(i);
            saveAllToJson();
            QMessageBox::information(nullptr, "Успешно", QString("Лекарство %1 успешно удалено").arg(title));
            return;
        }
    }
    qDebug() << "Ошибка: Медикамент" << title << "не найден";
    QMessageBox::warning(nullptr, "Ошибка", QString("Лекартсво %1 не найдено").arg(title));
}

// Поиск медикаментов
QList<PharmacyItem> JsonManager::searchMedicine(const QString &title) {
    QList<PharmacyItem> result;
    for (const PharmacyItem &item : medicines) {
        if (!title.isEmpty() && item.getTitle() == title) {
            result.append(item);
        }
    }
    return result;
}
//_____________________

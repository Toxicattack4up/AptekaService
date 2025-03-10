#include "JsonManager.h"

JsonManager::JsonManager()
{
    employees = LoadEmployees();
    QJsonObject json = LoadJSON("Base.json");
    // Если файл пуст или отсутствуют данные по аптекам и медикаментам – создаём базовую структуру
    if (json.isEmpty() ||
        !json.contains("Employees") ||
        !json.contains("Pharmacies") ||
        !json.contains("Medicines"))
    {
        QJsonObject newJson;
        newJson["Employees"] = QJsonArray();
        newJson["Pharmacies"] = QJsonArray();
        newJson["Medicines"] = QJsonArray();
        SaveToJSON("Base.json", newJson);
    }
}

// _____________ Методы для сотрудников (Employees) _____________
// (Оставляем без изменений – см. ранее предоставленный код)

QList<User> JsonManager::LoadEmployees()
{
    QJsonObject json = LoadJSON("Base.json");
    if(json.isEmpty())
    {
        qDebug() << "Не удалось загрузить данные сотрудников из файла";
        return QList<User>();
    }
    QJsonArray employeesArray = json["Employees"].toArray();
    QList<User> employees;

    for(const QJsonValue &value : employeesArray)
    {
        QJsonObject obj = value.toObject();
        User user;
        user.setRole(UserRoleHelper::fromString(obj["role"].toString()));
        user.setLogin(obj["login"].toString());
        user.setPasswordHash(obj["passwordHash"].toString());
        user.setFullName(obj["fullName"].toString());
        user.setEmail(obj["email"].toString());
        user.setRegistrationDate(QDateTime::fromString(obj["registrationDate"].toString(), Qt::ISODate));
        employees.append(user);
    }
    return employees;
}

QJsonArray JsonManager::EmployeesToJsonArray() const
{
    QJsonArray employeesArray;
    const QMap<UserRole, QString>& roleNames = UserRoleHelper::getRoleNames();

    for(const User &user : employees)
    {
        QJsonObject employeeObj;
        employeeObj["role"] = roleNames.value(user.getRole());
        employeeObj["login"] = user.getLogin();
        employeeObj["passwordHash"] = user.getPasswordHash();
        employeeObj["fullName"] = user.getFullName();
        employeeObj["email"] = user.getEmail();
        employeeObj["registrationDate"] = user.getRegistrationDate().toString(Qt::ISODate);
        employeesArray.append(employeeObj);
    }
    return employeesArray;
}

void JsonManager::SaveEmployees()
{
    QJsonObject json = LoadJSON("Base.json");
    json["Employees"] = EmployeesToJsonArray();
    SaveToJSON("Base.json", json);
}

// _____________ Методы для аптек (Pharmacies) _____________

QList<Pharmacy> JsonManager::LoadPharmacies()
{
    QJsonObject json = LoadJSON("Base.json");
    QList<Pharmacy> list;
    if(json.isEmpty())
    {
        qDebug() << "Не удалось загрузить данные аптек из файла";
        return list;
    }
    QJsonArray arr = json["Pharmacies"].toArray();
    for(const QJsonValue &value : arr)
    {
        QJsonObject obj = value.toObject();
        int id = obj["id"].toInt();
        QString address = obj["address"].toString();
        double size = obj["size"].toDouble();
        int maxCapacity = obj["maxCapacity"].toInt();
        Pharmacy ph(id, address, size, maxCapacity);
        list.append(ph);
    }
    return list;
}

QJsonArray JsonManager::PharmaciesToJsonArray() const
{
    QJsonArray arr;
    // Предполагаем, что аптечные данные уже загружены в файл (или их передадут в метод SavePharmacies)
    // Для каждого объекта Pharmacy создаём JSON-объект
    // Здесь мы не используем внутреннюю переменную, а ожидаем, что список аптек передается как параметр в SavePharmacies.
    return arr;
}

void JsonManager::SavePharmacies(const QList<Pharmacy>& pharmacies)
{
    QJsonArray arr;
    for(const Pharmacy &ph : pharmacies)
    {
        QJsonObject obj;
        obj["id"] = ph.getId();
        obj["address"] = ph.getAddress();
        obj["size"] = ph.getSize();
        obj["maxCapacity"] = ph.getMaxCapacity();
        arr.append(obj);
    }
    QJsonObject json = LoadJSON("Base.json");
    json["Pharmacies"] = arr;
    SaveToJSON("Base.json", json);
}

// _____________ Методы для медикаментов (Medicines) _____________

QList<PharmacyItem> JsonManager::LoadMedicines()
{
    QJsonObject json = LoadJSON("Base.json");
    QList<PharmacyItem> list;
    if(json.isEmpty())
    {
        qDebug() << "Не удалось загрузить данные медикаментов из файла";
        return list;
    }
    QJsonArray arr = json["Medicines"].toArray();
    for(const QJsonValue &value : arr)
    {
        QJsonObject obj = value.toObject();
        QString title = obj["title"].toString();
        double price = obj["price"].toDouble();
        bool recipe = obj["recipe"].toBool();
        QDate expiration = QDate::fromString(obj["expiration_date"].toString(), Qt::ISODate);
        int quantity = obj["quantity"].toInt();
        PharmacyItem item(title, price, recipe, expiration, quantity);
        list.append(item);
    }
    return list;
}

QJsonArray JsonManager::MedicinesToJsonArray() const
{
    QJsonArray arr;
    // Аналогично с аптеками – список медикаментов передаётся в метод SaveMedicines.
    return arr;
}

void JsonManager::SaveMedicines(const QList<PharmacyItem>& medicines)
{
    QJsonArray arr;
    for(const PharmacyItem &item : medicines)
    {
        QJsonObject obj;
        obj["title"] = item.getTitle();
        obj["price"] = item.getPrice();
        obj["recipe"] = item.isRecipeRequired();
        obj["expiration_date"] = item.getExpirationDate().toString(Qt::ISODate);
        obj["quantity"] = item.getQuantity();
        arr.append(obj);
    }
    QJsonObject json = LoadJSON("Base.json");
    json["Medicines"] = arr;
    SaveToJSON("Base.json", json);
}

// _____________ Остальные методы JsonManager _____________

QJsonObject JsonManager::LoadJSON(const QString &filePath)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка при открытии файла:" << file.errorString();
        return QJsonObject();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject())
    {
        qDebug() << "Ошибка: Некорректный JSON.";
        return QJsonObject();
    }

    return doc.object();
}

void JsonManager::SaveToJSON(const QString &filePath, const QJsonObject &json)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка при записи файла:" << file.errorString();
        return;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

QString JsonManager::HashPassword(const QString &password)
{
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

bool JsonManager::ValidateUser(const QString &login,
                               const QString &password,
                               const QJsonObject &users)
{
    // Заглушка для проверки пользователя
    return false;
}

void JsonManager::AddEmployee(const QString &_role,
                              const QString &_login,
                              const QString &_password,
                              const QString &_fullName,
                              const QString &_email)
{
    UserRole role = UserRoleHelper::fromString(_role);
    if(role == UserRole::Unknown)
    {
        qDebug() << "Ошибка: неизвестная роль";
        return;
    }

    for(const User &user : employees)
    {
        if(user.getLogin() == _login)
        {
            qDebug() << "Логин" << _login << "уже существует";
            return;
        }
    }

    QString passwordHash = HashPassword(_password);
    User newUser(role, _login, passwordHash, _fullName, _email);
    newUser.setRegistrationDate(QDateTime::currentDateTime());
    employees.append(newUser);
}

void JsonManager::RemoveEmployee(const QString &login)
{
    if(login.isEmpty())
    {
        qDebug() << "Ошибка: логин не может быть пустым";
        return;
    }

    for(int i = 0; i < employees.size(); ++i)
    {
        if(employees[i].getLogin() == login)
        {
            employees.removeAt(i);
            return;
        }
    }
    qDebug() << "Сотрудник" << login << "не найден";
}

QList<User> JsonManager::SearchEmployee(const QString &_role,
                                        const QString &_login,
                                        const QString &_password,
                                        const QString &_fullName,
                                        const QString &_email)
{
    QList<User> result;
    UserRole role = UserRoleHelper::fromString(_role);
    for(const User &user : employees)
    {
        bool match = true;
        if(!_role.isEmpty() && user.getRole() != role) match = false;
        if(!_login.isEmpty() && user.getLogin() != _login) match = false;
        if(!_password.isEmpty() && user.getPasswordHash() != HashPassword(_password)) match = false;
        if(!_fullName.isEmpty() && user.getFullName() != _fullName) match = false;
        if(!_email.isEmpty() && user.getEmail() != _email) match = false;

        if(match)
            result.append(user);
    }
    return result;
}


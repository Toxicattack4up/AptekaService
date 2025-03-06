#include "JsonMeneger.h"

JsonMeneger::JsonMeneger()
{
    employees = LoadEmployees();
}

// Загрузка данных в память
QList<User> JsonMeneger::LoadEmployees()
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

// Преобразование списка в JSON
QJsonArray JsonMeneger::EmployeesToJsonArray() const
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

// Сохранение списка в файл
void JsonMeneger::SaveEmployees()
{
    QJsonObject json;
    json["Employees"] = EmployeesToJsonArray();
    SaveToJSON("Base.json", json);
}


// Загрузка файла JSON
QJsonObject JsonMeneger::LoadJSON(const QString &filePath)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка при открытии файла: " << file.errorString();
        //qDebug() << "Путь к файлу: " << fullPath;
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

// Сохранение в файл JSON
void JsonMeneger::SaveToJSON(const QString &filePath, const QJsonObject &json)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка при записи файла: " << file.errorString();
        return;
    }

    QJsonDocument doc(json); // Преобразование в документ
    file.write(doc.toJson(QJsonDocument::Indented)); // Записываем с отступами
    file.close();
}

// Хеширование пароля
QString JsonMeneger::HashPassword(const QString &password)
{
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

// _____Сотрудники_____

// Добавление сотрудника
void JsonMeneger::AddEmployee(const QString &_role,
                              const QString &_login,
                              const QString &_password,
                              const QString &_fullName,
                              const QString &_email)
{
    // После добавления окон необходимо сделать обработку вывода операций (ошибка или правильность выполенения)


    UserRole role = UserRoleHelper::fromString(_role);
    if(role == UserRole::Unknown)
    {
        qDebug() << "Ошибка: неизвестная роль";
        return;
    }

    // Проверка на дубль логина
    for(const User &user : employees)
    {
        if(user.getLogin() == _login)
        {
            qDebug() << "Логин " << _login << " уже существует";
            return;
        }
    }

    // Хеширование
    QString passwordHash = HashPassword(_password);

    // Создание сотрудника
    User newUser(role, _login, passwordHash, _fullName, _email);

    // Дата регистрации
    newUser.setRegistrationDate(QDateTime::currentDateTime());

    // Добавление нового сотрудника в массив
    employees.append(newUser);
}

// Удаление сотрудика
void JsonMeneger::RemoveEmployee(const QString &login)
{
    // Если пуст, то завершить
    if(login.isEmpty())
    {
        qDebug() << "Ошибка: логин не может быть пустым";
        return;
    }

    for(int i = 0; i < employees.size(); ++i)
    {
        QJsonObject employeeObj = employees[i].toObject();

        if(employees[i].getLogin() == login)
        {
            employees.removeAt(i);
            return;
        }
    }

    qDebug() << "Сотрудник " << login << " не найден";
}

// Поиск сотрудника
void JsonMeneger::SearchEmployee(const QString &_role,
                                 const QString &_login,
                                 const QString &_password,
                                 const QString &_fullName,
                                 const QString &_email)
{
    QList<User> result;
    UserRole role = UserRoleHelper::fromString(_login);

    for(const User &user : employees)
    {
        bool match = true;
        if(!_role.isEmpty() && user.getRole() != role) match = false;
        if(!_login.isEmpty() && user.getLogin() != _login) match = false;
        if(!_password.isEmpty() && user.getPasswordHash() != HashPassword(_password)) match = false;
        if(!_fullName.isEmpty() && user.getFullName() != _fullName) match = false;
        if(!_email.isEmpty() && user.getEmail() != _email) match = false;


        if(match){
            result.append(user);;
        }
        return result;
    }
}

#include "JsonMeneger.h"

JsonMeneger::JsonMeneger() {}

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
    QString fullPath = QCoreApplication::applicationFilePath() + "/" + filePath;

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
void JsonMeneger::AddEmployee(const QString &_role,
                              const QString &_login,
                              const QString &_password,
                              const QString &_fullName,
                              const QString &_email)
{
    // После добавления окон необходимо сделать обработку вывода операций (ошибка или правильность выполенения)

    // Присваиваем стандартную роль
    UserRole role = UserRole::Unknown;
    const QMap<UserRole, QString>& roleNames = UserRoleHelper::getRoleNames();

    // Проверка роли и преобразование ее
    for(auto i = roleNames.begin(); i != roleNames.end(); ++i)
    {
        if(i.value() == _role)
        {
            role = i.key();
            break;
        }
    }
    //Нужно добавить проверку для почты, чтобы она была отображалась нормально
    // Хеширование
    QString passwordHash = HashPassword(_password);

    // Создание сотрудника
    User newUser(role, _login, passwordHash, _fullName, _email);

    // Дата регистрации
    newUser.setRegistrationDate(QDateTime::currentDateTime());

    // Формирование структуры для дальнейшей загрузки данных в файл
    QJsonObject employeeObj;
    employeeObj["role"] = roleNames.value(newUser.getRole());
    employeeObj["login"] = newUser.getLogin();
    employeeObj["passwordHash"] = newUser.getPasswordHash();
    employeeObj["fullName"] = newUser.getFullName();
    employeeObj["email"] = newUser.getEmail();
    employeeObj["registrationDate"] = newUser.getRegistrationDate().toString(Qt::ISODate);

    // Загрука файла с определенными категориями
    QJsonObject json = LoadJSON("Base.json");
    QJsonArray employees = json["Employees"].toArray();

    // Проверка на дубль логина
    for(const QJsonValue &value : employees)
    {
        if(value.toObject()["login"] == _login)
        {
            qDebug() << "Логин уже существует";
            return;
        }
    }

    // Добавление нового сотрудника в массив
    employees.append(employeeObj);

    // Загружаем массив обратно в файл и сохраняем
    json["employees"] = employees;
    SaveToJSON("Base.json", json);
}

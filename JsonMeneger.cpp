#include "JsonMeneger.h"

JsonMeneger::JsonMeneger() {}

// Загрузка файла JSON
QJsonObject JsonMeneger::LoadJSON(const QString &filePath)
{
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filePath;
    QFile file(fullPath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Ошибка при открытии файла: " << file.errorString();
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

// Хеширование пароля
QString JsonMeneger::HashPassword(const QString &password)
{
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

// _____Сотрудники_____
void JsonMeneger::AddEmployee(const QString &_name,
                 const QString &_login,
                 const QString &_password,
                 const QString &_fullName,
                 const QString &_role,
                 const QString &_email,
                 const QDateTime &_registrationDate)
{

}

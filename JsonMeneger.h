#ifndef JSONMENEGER_H
#define JSONMENEGER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDateTime>

class JsonMeneger
{
public:
    JsonMeneger();

private:
    // Загрузка файла JSON
    QJsonObject LoadJSON(const QString &filePath);

    // Хеширование пароля
    QString HashPassword(const QString &password);

    // Проверка пароля и логина
    bool ValidateUser(const QString &login,
                      const QString &password,
                      const QJsonObject &users);

    // _____Сотрудники_____
    // Добавление сотрудника
    void AddEmployee(const QString &_name,
                     const QString &_login,
                     const QString &_password,
                     const QString &_fullName,
                     const QString &_role,
                     const QString &_email,
                     const QDateTime &_registrationDate);

    // Удаление сотрудика
    void RemoveEmployee(const QString &login);

    // Поиск сотрудника

    //_____Склад_____
    // Добавление лекарства


    //Удаление лекарства


    // Поиск лекарства



};

#endif // JSONMENEGER_H

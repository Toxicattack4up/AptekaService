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

#include "User.h"

class JsonMeneger
{
public:
    JsonMeneger();

private:
    //_____Доп функции_____
    // Загрузка файла JSON
    QJsonObject LoadJSON(const QString &filePath);

    // Сохранение в файл JSON
    void SaveToJSON(const QString &filePath, const QJsonObject &json);

    // Хеширование пароля
    QString HashPassword(const QString &password);

    // Проверка пароля и логина
    bool ValidateUser(const QString &login,
                      const QString &password,
                      const QJsonObject &users);



    // _____Сотрудники_____
    // Добавление сотрудника
    void AddEmployee(const QString &_role,
                     const QString &_login,
                     const QString &_password,
                     const QString &_fullName,
                     const QString &_email);

    // Удаление сотрудика
    void RemoveEmployee(const QString &login);

    // Поиск сотрудника
    void SearchEmployee(const QString &_name,
                        const QString &_login,
                        const QString &_fullName,
                        const QString &_role,
                        const QString &_email);


    //_____Склад_____
    // Нужно добавить класс, который будет отвечать за препараты
    // Добавление лекарства
    void AddMedicine();

    //Удаление лекарства
    void RemoveMedicine();

    // Поиск лекарства
    void SearchEmployee();



    //_____

};

#endif // JSONMENEGER_H

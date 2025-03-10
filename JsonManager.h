#ifndef JSONMANAGER_H
#define JSONMANAGER_H

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

class JsonManager
{
protected:
    QList<User> employees;
public:
    JsonManager();

    // Загрузка данных в память
    QList<User> LoadEmployees();

    // Преобразование списка в JSON
    QJsonArray EmployeesToJsonArray() const;

    // Сохранение в файл
    void SaveEmployees();

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
    void AddEmployee(const QString &_role,
                     const QString &_login,
                     const QString &_password,
                     const QString &_fullName,
                     const QString &_email);

    void RemoveEmployee(const QString &login);

    QList<User> SearchEmployee(const QString &_role,
                               const QString &_login,
                               const QString &_password,
                               const QString &_fullName,
                               const QString &_email);

    // _____Склад_____
    // Методы для работы с лекарствами (заглушки)
    void AddMedicine();
    void RemoveMedicine();
    void SearchMedicine();
};

#endif // JSONMANAGER_H

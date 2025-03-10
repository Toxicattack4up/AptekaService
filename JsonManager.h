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
#include "Pharmacy.h"
#include "PharmacyItem.h"

class JsonManager
{
protected:
    QList<User> employees;
public:
    JsonManager();

    //_____Доп функции_____
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
    // Добавление сотрудника
    void AddEmployee(const QString &_role,
                     const QString &_login,
                     const QString &_password,
                     const QString &_fullName,
                     const QString &_email);

    // Удаление сотрудника
    void RemoveEmployee(const QString &login);

    // Поиск сотрудника
    QList<User> SearchEmployee(const QString &_role,
                               const QString &_login,
                               const QString &_password,
                               const QString &_fullName,
                               const QString &_email);

    // _____Склад (Медикаменты)_____
    // Заглушки для медикаментов (будут дополнены аналогично)
    void AddMedicine();
    void RemoveMedicine();

    // _____Новые методы для аптек и медикаментов_____
    // Для аптек
    QList<Pharmacy> LoadPharmacies();
    QJsonArray PharmaciesToJsonArray() const;
    void SavePharmacies(const QList<Pharmacy>& pharmacies);

    // Для медикаментов
    QList<PharmacyItem> LoadMedicines();
    QJsonArray MedicinesToJsonArray() const;
    void SaveMedicines(const QList<PharmacyItem>& medicines);

    // Замечание: все данные будут храниться в одном файле Base.json, где ключи:
    // "Employees", "Pharmacies" и "Medicines" содержат соответствующие массивы.
};

#endif // JSONMANAGER_H

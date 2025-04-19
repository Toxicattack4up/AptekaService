#ifndef JSONMANAGER_H
#define JSONMANAGER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include "UserRoleHelper.h"
#include "User.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"

class JsonManager {
private:
    QList<User> employees;         // Список всех пользователей (включая покупателей)
    QList<Pharmacy> pharmacies;    // Список аптек
    QList<PharmacyItem> medicines; // Список медикаментов
    QString filePath = "Base.json"; // Путь к JSON-файлу

    // Хеширование пароля с помощью SHA-256
    QString hashPassword(const QString &password);

    // Загрузка данных из JSON-файла
    QJsonObject loadJson();

    // Сохранение всех данных в JSON-файл
    void saveAllToJson();

public:
    JsonManager(); // Конструктор, загружающий данные при запуске

    // Метод для совершения покупки: уменьшает количество медикамента
    bool makePurchase(const QString &medicineTitle, int quantity);

    // Геттеры для передачи данный в таблицы
    const QList<User> &getEmployee() const;
    const QList<Pharmacy> &getPharmacy() const;
    const QList<PharmacyItem> &getMedicine() const;

    // Методы для работы с пользователями
    void addEmployee(const QString &_role, const QString &_login, const QString &_password,
                     const QString &_fullName, const QString &_email);
    void removeEmployee(const QString &login);
    QList<User> searchEmployee(const QString &_role, const QString &_login, const QString &_password,
                               const QString &_fullName, const QString &_email);
    QString validateUser(const QString &login, const QString &password);

    // Методы для работы с аптеками
    void addPharmacy(const Pharmacy &pharmacy);
    void removePharmacy(int id);
    QList<Pharmacy> searchPharmacy(int id, const QString &address);

    // Методы для работы с медикаментами
    void addMedicine(const PharmacyItem &item);
    void removeMedicine(const QString &title);
    QList<PharmacyItem> searchMedicine(const QString &title);
};

#endif // JSONMANAGER_H

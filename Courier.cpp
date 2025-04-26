#include "Courier.h"
#include "logger.h"
#include <QDebug>

Courier::Courier(const QString& login, const QString& password, const QString& fullName, const QString& email,
                 JsonManager& manager, CentralWarehouse& warehouse)
    : User(UserRole::Courier, login, password, fullName, email), jsonManager(manager), warehouse(warehouse) {
}

bool Courier::transferMedicine(const QString& medicineTitle, int quantity, int pharmacyId) {
    // Проверка существования аптеки
    QList<Pharmacy> pharmacies = jsonManager.searchPharmacy(pharmacyId, "");
    if (pharmacies.isEmpty()) {
        qDebug() << "Ошибка: Аптека с ID" << pharmacyId << "не найдена";
        Logger::instance().log("Courier", QString("Ошибка: Аптека с ID %1 не найдена").arg(pharmacyId));
        return false;
    }

    // Проверка максимальной вместимости аптеки
    Pharmacy pharmacy = pharmacies.first();
    int currentStock = jsonManager.getPharmacyStock(pharmacyId);
    if (currentStock + quantity > pharmacy.getMaxCapacity()) {
        qDebug() << "Ошибка: Аптека ID" << pharmacyId << "превысит максимальную вместимость";
        Logger::instance().log("Courier", QString("Ошибка: Аптека ID %1 превысит максимальную вместимость").arg(pharmacyId));
        return false;
    }

    // Перенос лекарства из склада в аптеку через JsonManager
    if (!jsonManager.moveMedicineToPharmacy(medicineTitle, quantity, pharmacyId)) {
        qDebug() << "Ошибка: Не удалось перенести" << quantity << "единиц" << medicineTitle;
        Logger::instance().log("Courier", QString("Ошибка: Не удалось перенести %1 единиц %2").arg(quantity).arg(medicineTitle));
        return false;
    }

    // Синхронизация с CentralWarehouse (если требуется)
    if (!warehouse.transferMedicine(medicineTitle, quantity, pharmacyId, pharmacy.getMaxCapacity())) {
        qDebug() << "Ошибка: Не удалось обновить склад для" << medicineTitle;
        Logger::instance().log("Courier", QString("Ошибка: Не удалось обновить склад для %1").arg(medicineTitle));
        // Откат операции в JsonManager, если нужно
        return false;
    }

    qDebug() << "Успешно перенесено" << quantity << "единиц" << medicineTitle << "в аптеку" << pharmacyId;
    Logger::instance().log("Courier", QString("Успешно перенесено %1 единиц %2 в аптеку %3").arg(quantity).arg(medicineTitle).arg(pharmacyId));
    return true;
}

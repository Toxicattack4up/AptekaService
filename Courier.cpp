#include "Courier.h"
#include <QDebug>

Courier::Courier(const QString& login, const QString& password, const QString& fullName, const QString& email,
                 JsonManager& manager, CentralWarehouse& warehouse)
    : User(UserRole::Courier, login, password, fullName, email), jsonManager(manager), warehouse(warehouse) {
}

bool Courier::transferMedicine(const QString& medicineTitle, int quantity, int pharmacyId) {
    // Find pharmacy
    QList<Pharmacy> pharmacies = jsonManager.searchPharmacy(pharmacyId, "");
    if (pharmacies.isEmpty()) {
        qDebug() << "Ошибка: Аптека с ID" << pharmacyId << "не найдена";
        return false;
    }
    Pharmacy pharmacy = pharmacies.first();

    // Transfer from warehouse
    if (!warehouse.transferMedicine(medicineTitle, quantity, pharmacyId, pharmacy.getMaxCapacity())) {
        qDebug() << "Ошибка: Не удалось перенести" << quantity << "единиц" << medicineTitle;
        return false;
    }

    // Add to pharmacy stock
    PharmacyItem item = warehouse.findMedicine(medicineTitle);
    item.setQuantity(quantity);
    jsonManager.addMedicine(item, pharmacyId);
    qDebug() << "Успешно перенесено" << quantity << "единиц" << medicineTitle << "в аптеку" << pharmacyId;
    return true;
}

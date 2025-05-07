#ifndef CENTRALWAREHOUSE_H
#define CENTRALWAREHOUSE_H

#include "PharmacyItem.h"
#include <QList>
#include <QString>

class CentralWarehouse {
public:
    CentralWarehouse();
    void addMedicine(const PharmacyItem &item);
    bool transferMedicine(const QString &title, int quantity, int pharmacyId, int maxCapacity);
    QList<PharmacyItem> getMedicines() const;
    int getMedicineQuantity(const QString &title) const;
    PharmacyItem findMedicine(const QString &title) const;
    void removeMedicine(const QString& title, int quantity);
    void clearMedicines(); // Новый метод для очистки склада

private:
    QList<PharmacyItem> medicines;
    void showError(const QString& message);
};

#endif // CENTRALWAREHOUSE_H

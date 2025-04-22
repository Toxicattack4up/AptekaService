#ifndef CENTRALWAREHOUSE_H
#define CENTRALWAREHOUSE_H

#include "PharmacyItem.h"
#include <QList>

class CentralWarehouse {
public:
    CentralWarehouse();
    void addMedicine(const PharmacyItem &item);
    bool transferMedicine(const QString &title, int quantity, int pharmacyId, int maxCapacity);
    QList<PharmacyItem> getMedicines() const;
    int getMedicineQuantity(const QString &title) const;
    PharmacyItem findMedicine(const QString &title) const;

private:
    QList<PharmacyItem> medicines;
};

#endif // CENTRALWAREHOUSE_H

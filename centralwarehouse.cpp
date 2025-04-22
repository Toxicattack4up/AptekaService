#include "centralwarehouse.h"
#include "logger.h"
#include <QDebug>

CentralWarehouse::CentralWarehouse() {}

void CentralWarehouse::addMedicine(const PharmacyItem &item) {
    for (PharmacyItem &existing : medicines) {
        if (existing.getTitle() == item.getTitle()) {
            existing.setQuantity(existing.getQuantity() + item.getQuantity());
            Logger::instance().log("CentralWarehouse", QString("Added %1 units of %2 to central warehouse (total: %3)")
                                                           .arg(item.getQuantity()).arg(item.getTitle()).arg(existing.getQuantity()));
            return;
        }
    }
    medicines.append(item);
    Logger::instance().log("CentralWarehouse", QString("Added new medicine %1 with %2 units to central warehouse")
                                                   .arg(item.getTitle()).arg(item.getQuantity()));
}

bool CentralWarehouse::transferMedicine(const QString &title, int quantity, int pharmacyId, int maxCapacity) {
    for (PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            if (item.getQuantity() < quantity) {
                Logger::instance().log("CentralWarehouse", QString("Failed to transfer %1 units of %2 to pharmacy %3: insufficient stock (%4)")
                                           .arg(quantity).arg(title).arg(pharmacyId).arg(item.getQuantity()));
                return false;
            }
            if (quantity > maxCapacity) {
                Logger::instance().log("CentralWarehouse", QString("Failed to transfer %1 units of %2 to pharmacy %3: exceeds max capacity (%4)")
                                           .arg(quantity).arg(title).arg(pharmacyId).arg(maxCapacity));
                return false;
            }
            item.setQuantity(item.getQuantity() - quantity);
            Logger::instance().log("CentralWarehouse", QString("Transferred %1 units of %2 to pharmacy %3 (remaining: %4)")
                                                           .arg(quantity).arg(title).arg(pharmacyId).arg(item.getQuantity()));
            if (item.getQuantity() == 0) {
                medicines.removeOne(item);
                Logger::instance().log("CentralWarehouse", QString("Removed %1 from central warehouse (stock depleted)").arg(title));
            }
            return true;
        }
    }
    Logger::instance().log("CentralWarehouse", QString("Failed to transfer %1: medicine not found").arg(title));
    return false;
}

QList<PharmacyItem> CentralWarehouse::getMedicines() const {
    return medicines;
}

int CentralWarehouse::getMedicineQuantity(const QString &title) const {
    for (const PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            return item.getQuantity();
        }
    }
    return 0;
}

PharmacyItem CentralWarehouse::findMedicine(const QString &title) const {
    for (const PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            return item;
        }
    }
    return PharmacyItem("", 0, false, QDate(), 0);
}

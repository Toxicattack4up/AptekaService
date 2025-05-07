#include "centralwarehouse.h"
#include "logger.h"
#include <QMessageBox>

CentralWarehouse::CentralWarehouse() {}

void CentralWarehouse::addMedicine(const PharmacyItem &item) {
    if (item.getTitle().isEmpty()) {
        showError("Ошибка: Название лекарства не может быть пустым!");
        return;
    }
    if (item.getQuantity() <= 0) {
        showError("Ошибка: Количество должно быть больше 0!");
        return;
    }
    if (item.isExpired()) {
        showError("Ошибка: Лекарство просрочено!");
        return;
    }

    for (PharmacyItem& existing : medicines) {
        if (existing.getTitle() == item.getTitle() &&
            existing.getPrice() == item.getPrice() &&
            existing.isRecipeRequired() == item.isRecipeRequired() &&
            existing.getExpirationDate() == item.getExpirationDate()) {
            existing.setQuantity(existing.getQuantity() + item.getQuantity());
            Logger::instance().log("CentralWarehouse", QString("Добавлено %1 единиц %2").arg(item.getQuantity()).arg(item.getTitle()));
            return;
        }
    }
    medicines.append(item);
    Logger::instance().log("CentralWarehouse", QString("Добавлено новое лекарство %1").arg(item.getTitle()));
}

bool CentralWarehouse::transferMedicine(const QString &title, int quantity, int pharmacyId, int maxCapacity) {
    if (title.isEmpty()) {
        showError("Ошибка: Название лекарства не может быть пустым!");
        return false;
    }
    if (quantity <= 0) {
        showError("Ошибка: Количество должно быть больше 0!");
        return false;
    }

    for (PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            if (item.isExpired()) {
                showError(QString("Ошибка: Лекарство %1 просрочено!").arg(title));
                return false;
            }
            if (item.getQuantity() < quantity) {
                showError(QString("Ошибка: Недостаточно %1 на складе (%2 доступно)")
                              .arg(title).arg(item.getQuantity()));
                Logger::instance().log("CentralWarehouse", QString("Не удалось перенести %1 единиц %2 в аптеку %3: недостаточно на складе (%4)")
                                                               .arg(quantity).arg(title).arg(pharmacyId).arg(item.getQuantity()));
                return false;
            }
            if (quantity > maxCapacity) {
                showError(QString("Ошибка: Превышена вместимость аптеки %1 (%2)")
                              .arg(pharmacyId).arg(maxCapacity));
                Logger::instance().log("CentralWarehouse", QString("Не удалось перенести %1 единиц %2 в аптеку %3: превышена вместимость (%4)")
                                                               .arg(quantity).arg(title).arg(pharmacyId).arg(maxCapacity));
                return false;
            }
            item.setQuantity(item.getQuantity() - quantity);
            Logger::instance().log("CentralWarehouse", QString("Перенесено %1 единиц %2 в аптеку %3 (осталось: %4)")
                                                           .arg(quantity).arg(title).arg(pharmacyId).arg(item.getQuantity()));
            if (item.getQuantity() == 0) {
                medicines.removeOne(item);
                Logger::instance().log("CentralWarehouse", QString("Удалено %1 со склада (запасы исчерпаны)").arg(title));
            }
            return true;
        }
    }
    showError(QString("Ошибка: Лекарство %1 не найдено на складе").arg(title));
    Logger::instance().log("CentralWarehouse", QString("Не удалось перенести %1: лекарство не найдено").arg(title));
    return false;
}

QList<PharmacyItem> CentralWarehouse::getMedicines() const {
    Logger::instance().log("CentralWarehouse", QString("Запрошен список лекарств: %1 элементов").arg(medicines.size()));
    return medicines;
}

int CentralWarehouse::getMedicineQuantity(const QString &title) const {
    for (const PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            Logger::instance().log("CentralWarehouse", QString("Найдено %1: %2 единиц").arg(title).arg(item.getQuantity()));
            return item.getQuantity();
        }
    }
    Logger::instance().log("CentralWarehouse", QString("Лекарство %1 не найдено").arg(title));
    return 0;
}

PharmacyItem CentralWarehouse::findMedicine(const QString &title) const {
    for (const PharmacyItem &item : medicines) {
        if (item.getTitle() == title) {
            Logger::instance().log("CentralWarehouse", QString("Найдено лекарство %1").arg(title));
            return item;
        }
    }
    Logger::instance().log("CentralWarehouse", QString("Лекарство %1 не найдено").arg(title));
    return PharmacyItem("", 0, false, QDate(), 0);
}

void CentralWarehouse::removeMedicine(const QString& title, int quantity) {
    for (int i = medicines.size() - 1; i >= 0; --i) {
        if (medicines[i].getTitle().compare(title, Qt::CaseInsensitive) == 0) {
            int currentQuantity = medicines[i].getQuantity();
            if (currentQuantity <= quantity) {
                quantity -= currentQuantity;
                medicines.removeAt(i);
                Logger::instance().log("CentralWarehouse", QString("Удалено %1 единиц %2").arg(currentQuantity).arg(title));
            } else {
                medicines[i].setQuantity(currentQuantity - quantity);
                Logger::instance().log("CentralWarehouse", QString("Удалено %1 единиц %2").arg(quantity).arg(title));
                return;
            }
        }
    }
    if (quantity > 0) {
        Logger::instance().log("CentralWarehouse", QString("Ошибка: Не удалось удалить %1 единиц %2, недостаточно на складе").arg(quantity).arg(title));
    }
}

void CentralWarehouse::clearMedicines() {
    medicines.clear();
    Logger::instance().log("CentralWarehouse", "Склад очищен");
}

void CentralWarehouse::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("CentralWarehouse", message);
}

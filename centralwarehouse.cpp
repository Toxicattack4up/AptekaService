#include "centralwarehouse.h"
#include "logger.h"
#include <QMessageBox>

// Конструктор центрального склада
CentralWarehouse::CentralWarehouse() {}

// Добавление лекарства на склад
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

    for (PharmacyItem &existing : medicines) {
        if (existing.getTitle() == item.getTitle()) {
            existing.setQuantity(existing.getQuantity() + item.getQuantity());
            Logger::instance().log("CentralWarehouse", QString("Добавлено %1 единиц %2 на склад (всего: %3)")
                                                           .arg(item.getQuantity()).arg(item.getTitle()).arg(existing.getQuantity()));
            return;
        }
    }
    medicines.append(item);
    Logger::instance().log("CentralWarehouse", QString("Добавлено новое лекарство %1 (%2 единиц) на склад")
                                                   .arg(item.getTitle()).arg(item.getQuantity()));
}

// Перемещение лекарства в аптеку
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

// Получение списка всех лекарств на складе
QList<PharmacyItem> CentralWarehouse::getMedicines() const {
    Logger::instance().log("CentralWarehouse", QString("Запрошен список лекарств: %1 элементов").arg(medicines.size()));
    return medicines;
}

// Получение количества лекарства по названию
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

// Поиск лекарства по названию
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

// Вывод ошибки пользователю
void CentralWarehouse::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("CentralWarehouse", message);
}

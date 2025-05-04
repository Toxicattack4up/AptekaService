#include "historymanager.h"
#include <QDebug>

HistoryManager& HistoryManager::instance() {
    static HistoryManager manager;
    return manager;
}

void HistoryManager::addOperation(const QString& userLogin, const QString& action, const QString& details, int pharmacyId) {
    Operation op;
    op.userLogin = userLogin;
    op.action = action;
    op.details = details;
    op.timestamp = QDateTime::currentDateTime();
    op.pharmacyId = pharmacyId;
    operations.append(op);
    qDebug() << "Добавлена операция: " << action << "для аптеки ID" << pharmacyId;
}

QList<Operation> HistoryManager::getOperations(int pharmacyId) const {
    if (pharmacyId == 0) {
        return operations; // Для администратора возвращаем все операции
    }
    QList<Operation> filtered;
    for (const Operation& op : operations) {
        if (op.pharmacyId == pharmacyId) {
            filtered.append(op);
        }
    }
    return filtered;
}

QMap<QDate, int> HistoryManager::getSalesByDate(int pharmacyId) const {
    QMap<QDate, int> salesByDate;
    for (const Operation& op : operations) {
        if (op.action == "Продажа" && (pharmacyId == 0 || op.pharmacyId == pharmacyId)) {
            QDate date = op.timestamp.date();
            salesByDate[date] += 1; // Подсчет количества продаж
        }
    }
    return salesByDate;
}

void HistoryManager::clearOperations() {
    operations.clear();
    qDebug() << "Список операций очищен";
}

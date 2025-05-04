#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QString>
#include <QList>
#include <QDateTime>

// Определение структуры Operation
struct Operation {
    QString userLogin;
    QString action;
    QString details;
    QDateTime timestamp;
    int pharmacyId; // Добавлено поле для идентификатора аптеки
};

class HistoryManager {
public:
    static HistoryManager& instance();
    void addOperation(const QString& userLogin, const QString& action, const QString& details, int pharmacyId);
    QList<Operation> getOperations(int pharmacyId = 0) const; // Фильтрация по pharmacyId
    QMap<QDate, int> getSalesByDate(int pharmacyId) const; // Статистика продаж по датам
    void clearOperations(); // Метод для очистки операций

private:
    HistoryManager() = default;
    QList<Operation> operations; // Хранилище операций
};

#endif // HISTORYMANAGER_H

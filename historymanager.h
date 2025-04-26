#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QString>
#include <QDateTime>

class HistoryManager {
public:
    static HistoryManager& instance();
    void addOperation(const QString& userLogin, const QString& operation, const QString& details);

private:
    HistoryManager();
    void saveToJson(const QString& userLogin, const QString& operation, const QString& details);
};

#endif // HISTORYMANAGER_H

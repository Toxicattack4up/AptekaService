#include "HistoryManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

HistoryManager::HistoryManager() {
    QDir().mkpath("."); // Убедимся, что директория существует
}

HistoryManager& HistoryManager::instance() {
    static HistoryManager manager;
    return manager;
}

void HistoryManager::addOperation(const QString& userLogin, const QString& operation, const QString& details) {
    saveToJson(userLogin, operation, details);
}

void HistoryManager::saveToJson(const QString& userLogin, const QString& operation, const QString& details) {
    QFile file("history.json");
    QJsonArray historyArray;

    // Читаем существующую историю
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull()) {
            historyArray = doc.array();
        }
        file.close();
    }

    // Добавляем новую операцию
    QJsonObject opObj;
    opObj["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    opObj["userLogin"] = userLogin;
    opObj["operation"] = operation;
    opObj["details"] = details;
    historyArray.append(opObj);

    // Сохраняем историю
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(historyArray);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "Ошибка открытия history.json для записи";
    }
}

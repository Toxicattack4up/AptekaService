#include "logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

Logger::Logger() {
    QDir().mkpath("."); // Убедимся, что директория существует
}

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::log(const QString& source, const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString logMessage = QString("[%1] %2: %3").arg(timestamp, source, message);
    saveToJson(source, message);
}

void Logger::saveToJson(const QString& source, const QString& message) {
    QFile file("logs.json");
    QJsonArray logsArray;

    // Читаем существующие логи
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull()) {
            logsArray = doc.array();
        }
        file.close();
    }

    // Добавляем новый лог
    QJsonObject logObj;
    logObj["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    logObj["source"] = source;
    logObj["message"] = message;
    logsArray.append(logObj);

    // Сохраняем логи
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(logsArray);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "Ошибка открытия logs.json для записи";
    }
}

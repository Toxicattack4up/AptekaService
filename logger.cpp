#include "logger.h"
#include <QDebug>

Logger::Logger() {
    QString fullPath = QCoreApplication::applicationDirPath() + "/logs.json";
    logFile.setFileName(fullPath);
    if (!logFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Error: Could not open logs.json for writing:" << logFile.errorString();
    }
}

Logger::~Logger() {
    if (logFile.isOpen()) {
        logFile.close();
    }
}

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::log(const QString &category, const QString &message) {
    QJsonDocument doc;
    QJsonArray logsArray;

    // Read existing logs
    if (logFile.size() > 0) {
        logFile.seek(0);
        QByteArray fileData = logFile.readAll();
        doc = QJsonDocument::fromJson(fileData);
        if (doc.isArray()) {
            logsArray = doc.array();
        }
    }

    // Create new log entry
    QJsonObject logEntry;
    logEntry["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    logEntry["category"] = category;
    logEntry["message"] = message;
    logsArray.append(logEntry);

    // Write back to file
    doc.setArray(logsArray);
    logFile.seek(0);
    logFile.resize(0);
    logFile.write(doc.toJson(QJsonDocument::Indented));
    logFile.flush();
    qDebug() << QString("[%1] %2: %3").arg(logEntry["timestamp"].toString(), category, message);
}

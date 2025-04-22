#ifndef LOGGER_H
#define LOGGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>
#include <QCoreApplication>
#include <QString>

class Logger {
public:
    static Logger& instance();
    void log(const QString &category, const QString &message);

private:
    Logger();
    ~Logger();
    QFile logFile;
};

#endif // LOGGER_H

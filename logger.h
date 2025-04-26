#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDateTime>

class Logger {
public:
    static Logger& instance();
    void log(const QString& source, const QString& message);

private:
    Logger();
    void saveToJson(const QString& source, const QString& message);
};

#endif // LOGGER_H

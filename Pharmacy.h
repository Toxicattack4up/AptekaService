#ifndef PHARMACY_H
#define PHARMACY_H

#include <QString>

class Pharmacy {
private:
    int id;                // Уникальный ID аптеки
    QString address;       // Адрес аптеки
    double size;           // Размер аптеки в квадратных метрах
    int maxCapacity;       // Максимальная вместимость лекарств

public:
    // Конструкторы
    Pharmacy(int id, const QString& address, double size, int maxCapacity);

    // Геттеры
    int getId() const;
    QString getAddress() const;
    double getSize() const;
    int getMaxCapacity() const;

    // Сеттеры
    void setAddress(const QString& address);
    void setSize(double size);
    void setMaxCapacity(int maxCapacity);
};

#endif // PHARMACY_H

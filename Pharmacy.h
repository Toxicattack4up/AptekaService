#ifndef PHARMACY_H
#define PHARMACY_H

#include <QString>

class Pharmacy {
private:
    int id;                // Уникальный идентификатор аптеки
    QString address;       // Адрес аптеки
    double size;           // Размер аптеки в квадратных метрах
    int maxCapacity;       // Максимальная вместимость лекарств

public:
    // Конструктор: создаёт аптеку с заданными параметрами
    Pharmacy(int id, const QString& address, double size, int maxCapacity);

    // Геттеры: возвращают значения полей
    int getId() const;
    QString getAddress() const;
    double getSize() const;
    int getMaxCapacity() const;

    // Сеттеры: устанавливают новые значения с проверкой корректности
    void setAddress(const QString& address);
    void setSize(double size);
    void setMaxCapacity(int maxCapacity);
};

#endif // PHARMACY_H

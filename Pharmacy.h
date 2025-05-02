#ifndef PHARMACY_H
#define PHARMACY_H

#include <QString>
#include <QDebug>

class Pharmacy {
public:
    Pharmacy(int id = 0, const QString& address = "", double size = 0.0, int maxCapacity = 0);
    int getId() const;
    QString getAddress() const;
    double getSize() const;
    double getSquare() const;
    int getMaxCapacity() const;
    void setAddress(const QString& newAddress);
    void setSize(double newSize);
    void setMaxCapacity(int newMaxCapacity);

private:
    int id;
    QString address;
    double size;
    int maxCapacity;
};

#endif // PHARMACY_H

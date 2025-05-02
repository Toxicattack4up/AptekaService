#include "Pharmacy.h"
#include <QDebug>

Pharmacy::Pharmacy(int id, const QString& address, double size, int maxCapacity)
    : id(id), address(address), size(size), maxCapacity(maxCapacity) {}

int Pharmacy::getId() const {
    return id;
}

QString Pharmacy::getAddress() const {
    return address;
}

double Pharmacy::getSize() const {
    return size;
}

int Pharmacy::getMaxCapacity() const {
    return maxCapacity;
}

void Pharmacy::setAddress(const QString& newAddress) {
    if (newAddress.isEmpty()) {
        qDebug() << "Ошибка: Адрес не может быть пустым";
        return;
    }
    address = newAddress;
}

void Pharmacy::setSize(double newSize) {
    if (newSize <= 0) {
        qDebug() << "Ошибка: Размер аптеки должен быть больше 0";
        return;
    }
    size = newSize;
}

void Pharmacy::setMaxCapacity(int newMaxCapacity) {
    if (newMaxCapacity <= 0) {
        qDebug() << "Ошибка: Максимальная вместимость должна быть больше 0";
        return;
    }
    maxCapacity = newMaxCapacity;
}

double Pharmacy::getSquare() const
{
    return size;
}

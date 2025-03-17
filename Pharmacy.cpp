#include "Pharmacy.h"
#include <QDebug>

// Конструктор
Pharmacy::Pharmacy(int id, const QString& address, double size, int maxCapacity)
    : id(id), address(address), size(size), maxCapacity(maxCapacity) {}

// Геттеры
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

// Сеттеры
void Pharmacy::setAddress(const QString& newAddress) {
    if (newAddress.isEmpty())
        qDebug() << "Адрес не может быть пустым";
    address = newAddress;
}

void Pharmacy::setSize(double newSize) {
    if (newSize <= 0)
        qDebug() << "Размер аптеки должен быть больше 0";
    size = newSize;
}

void Pharmacy::setMaxCapacity(int newMaxCapacity) {
    if (newMaxCapacity <= 0)
        qDebug() << "Максимальная вместимость должна быть больше 0";
    maxCapacity = newMaxCapacity;
}

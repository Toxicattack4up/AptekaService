#include "Pharmacy.h"
#include <QDebug>

// Конструктор аптеки: инициализирует объект аптеки с заданными параметрами
Pharmacy::Pharmacy(int id, const QString& address, double size, int maxCapacity)
    : id(id), address(address), size(size), maxCapacity(maxCapacity) {}

// Получение ID: возвращает идентификатор аптеки
int Pharmacy::getId() const {
    return id;
}

// Получение адреса: возвращает адрес аптеки
QString Pharmacy::getAddress() const {
    return address;
}

// Получение размера: возвращает размер аптеки в квадратных метрах
double Pharmacy::getSize() const {
    return size;
}

// Получение максимальной вместимости: возвращает максимальное количество лекарств
int Pharmacy::getMaxCapacity() const {
    return maxCapacity;
}

// Установка адреса: изменяет адрес аптеки с проверкой
void Pharmacy::setAddress(const QString& newAddress) {
    if (newAddress.isEmpty()) {
        qDebug() << "Ошибка: Адрес не может быть пустым";
        return;
    }
    address = newAddress;
}

// Установка размера: изменяет размер аптеки с проверкой
void Pharmacy::setSize(double newSize) {
    if (newSize <= 0) {
        qDebug() << "Ошибка: Размер аптеки должен быть больше 0";
        return;
    }
    size = newSize;
}

// Установка максимальной вместимости: изменяет вместимость с проверкой
void Pharmacy::setMaxCapacity(int newMaxCapacity) {
    if (newMaxCapacity <= 0) {
        qDebug() << "Ошибка: Максимальная вместимость должна быть больше 0";
        return;
    }
    maxCapacity = newMaxCapacity;
}

// Получение площади: возвращает размер аптеки (дублирует getSize)
double Pharmacy::getSquare() const {
    return size;
}

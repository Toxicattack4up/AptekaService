#include "PharmacyItem.h"
#include <QDebug>

// Конструктор: инициализирует медикамент с заданными параметрами
PharmacyItem::PharmacyItem(const QString& title, double price, bool recipe, const QDate& expiration_date, int quantity)
    : title(title), price(price), recipe(recipe), expiration_date(expiration_date), quantity(quantity) {}

// Геттеры
QString PharmacyItem::getTitle() const {
    return title;
}

double PharmacyItem::getPrice() const {
    return price;
}

bool PharmacyItem::isRecipeRequired() const {
    return recipe;
}

QDate PharmacyItem::getExpirationDate() const {
    return expiration_date;
}

int PharmacyItem::getQuantity() const {
    return quantity;
}

// Сеттеры с проверками
void PharmacyItem::setTitle(const QString& newTitle) {
    if (newTitle.isEmpty()) {
        qDebug() << "Ошибка: Название лекарства не может быть пустым!";
        return; // Не устанавливаем пустое название
    }
    title = newTitle;
}

void PharmacyItem::setPrice(double newPrice) {
    if (newPrice < 0) {
        qDebug() << "Ошибка: Цена не может быть отрицательной!";
        return; // Не устанавливаем отрицательную цену
    }
    price = newPrice;
}

void PharmacyItem::setRecipeRequired(bool newRecipe) {
    recipe = newRecipe;
}

void PharmacyItem::setExpirationDate(const QDate& newExpirationDate) {
    expiration_date = newExpirationDate;
}

void PharmacyItem::setQuantity(int newQuantity) {
    if (newQuantity < 0) {
        qDebug() << "Ошибка: Количество не может быть отрицательным!";
        return; // Не устанавливаем отрицательное количество
    }
    quantity = newQuantity;
}

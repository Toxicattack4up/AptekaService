#include "Pharmacy.h"

// Конструктор
Pharmacy::Pharmacy(const QString& title, double price, bool recipe, const QDate& expiration_date, int quantity)
    : title(title), price(price), recipe(recipe), expiration_date(expiration_date), quantity(quantity) {}

// Геттеры
QString Pharmacy::getTitle() const {
    return title;
}

double Pharmacy::getPrice() const {
    return price;
}

bool Pharmacy::isRecipeRequired() const {
    return recipe;
}

QDate Pharmacy::getExpirationDate() const {
    return expiration_date;
}

int Pharmacy::getQuantity() const {
    return quantity;
}

// Сеттеры
void Pharmacy::setTitle(const QString& newTitle) {
    title = newTitle;
}

void Pharmacy::setPrice(double newPrice) {
    if (newPrice >= 0) { // Проверяем, чтобы цена была неотрицательной
        price = newPrice;
    }
}

void Pharmacy::setRecipeRequired(bool newRecipe) {
    recipe = newRecipe;
}

void Pharmacy::setExpirationDate(const QDate& newExpirationDate) {
    expiration_date = newExpirationDate;
}

void Pharmacy::setQuantity(int newQuantity) {
    if (newQuantity >= 0) { // Проверяем, чтобы количество не было отрицательным
        quantity = newQuantity;
    }
}

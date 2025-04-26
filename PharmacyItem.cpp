#include "PharmacyItem.h"
#include "logger.h"
#include <QMessageBox>

// Проверка строки на пустоту
static bool isValidString(const QString &value, const QString &fieldName) {
    if (value.isEmpty()) {
        Logger::instance().log("PharmacyItem", QString("Ошибка: %1 не может быть пустым!").arg(fieldName));
        return false;
    }
    return true;
}

// Конструктор медикамента с валидацией данных
PharmacyItem::PharmacyItem(const QString& title, double price, bool recipe, const QDate &expiration_date, int quantity)
    : title(title), price(price), recipe(recipe), expiration_date(expiration_date), quantity(quantity), pharmacyId(0) {
    if (title.isEmpty() || price < 0 || quantity < 0 || !expiration_date.isValid()) {
        showError("Ошибка: Некорректные данные для медикамента!");
        return;
    }
    Logger::instance().log("PharmacyItem", QString("Создан медикамент %1").arg(title));
}

// Получение названия медикамента
QString PharmacyItem::getTitle() const {
    return title;
}

// Получение цены медикамента
double PharmacyItem::getPrice() const {
    return price;
}

// Проверка, требуется ли рецепт
bool PharmacyItem::isRecipeRequired() const {
    return recipe;
}

// Получение даты истечения срока годности
QDate PharmacyItem::getExpirationDate() const {
    return expiration_date;
}

// Получение количества медикамента
int PharmacyItem::getQuantity() const {
    return quantity;
}

// Получение ID аптеки
int PharmacyItem::getPharmacyId() const {
    return pharmacyId;
}

// Установка названия медикамента
void PharmacyItem::setTitle(const QString& newTitle) {
    if (!isValidString(newTitle, "Название")) {
        showError("Ошибка: Название медикамента не может быть пустым!");
        return;
    }
    title = newTitle;
    Logger::instance().log("PharmacyItem", QString("Изменено название медикамента на %1").arg(newTitle));
}

// Установка цены медикамента
void PharmacyItem::setPrice(double newPrice) {
    if (newPrice < 0) {
        showError("Ошибка: Цена не может быть отрицательной!");
        return;
    }
    price = newPrice;
    Logger::instance().log("PharmacyItem", QString("Изменена цена медикамента %1 на %2").arg(title).arg(newPrice));
}

// Установка требования рецепта
void PharmacyItem::setRecipeRequired(bool newRecipe) {
    recipe = newRecipe;
    Logger::instance().log("PharmacyItem", QString("Требование рецепта для %1 изменено на %2").arg(title).arg(newRecipe ? "да" : "нет"));
}

// Установка даты истечения срока годности
void PharmacyItem::setExpirationDate(const QDate& newExpirationDate) {
    if (!newExpirationDate.isValid()) {
        showError("Ошибка: Некорректная дата истечения срока годности!");
        return;
    }
    expiration_date = newExpirationDate;
    Logger::instance().log("PharmacyItem", QString("Изменена дата истечения для %1 на %2").arg(title).arg(newExpirationDate.toString(Qt::ISODate)));
}

// Установка количества медикамента
void PharmacyItem::setQuantity(int newQuantity) {
    if (newQuantity < 0) {
        showError("Ошибка: Количество не может быть отрицательным!");
        return;
    }
    quantity = newQuantity;
    Logger::instance().log("PharmacyItem", QString("Изменено количество медикамента %1 на %2").arg(title).arg(newQuantity));
}

// Установка ID аптеки
void PharmacyItem::setPharmacyId(int newPharmacyId) {
    if (newPharmacyId < 0) {
        showError("Ошибка: ID аптеки не может быть отрицательным!");
        return;
    }
    pharmacyId = newPharmacyId;
    Logger::instance().log("PharmacyItem", QString("Изменён ID аптеки для %1 на %2").arg(title).arg(newPharmacyId));
}

// Проверка, просрочен ли медикамент
bool PharmacyItem::isExpired() const {
    bool expired = expiration_date < QDate::currentDate();
    Logger::instance().log("PharmacyItem", QString("Проверка медикамента %1: %2").arg(title).arg(expired ? "просрочен" : "действителен"));
    return expired;
}

// Сравнение медикаментов
bool PharmacyItem::operator==(const PharmacyItem &other) const {
    bool equal = title == other.title && pharmacyId == other.pharmacyId;
    Logger::instance().log("PharmacyItem", QString("Сравнение медикамента %1 с %2: %3").arg(title).arg(other.title).arg(equal ? "равны" : "различны"));
    return equal;
}

// Вывод ошибки пользователю с логированием
void PharmacyItem::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("PharmacyItem", message);
}

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

// Конструктор медикамента: инициализирует объект с валидацией входных данных
PharmacyItem::PharmacyItem(const QString& title, double price, bool recipe, const QDate &expiration_date, int quantity)
    : title(title), price(price), recipe(recipe), expiration_date(expiration_date), quantity(quantity), pharmacyId(0) {
    if (!isValidString(title, "Название") || price < 0 || quantity < 0 || !expiration_date.isValid()) {
        showError("Ошибка: Некорректные данные для медикамента!");
        return;
    }
    Logger::instance().log("PharmacyItem", QString("Создан медикамент %1").arg(title));
}

// Получение названия: возвращает название медикамента
QString PharmacyItem::getTitle() const {
    return title;
}

// Получение цены: возвращает цену медикамента
double PharmacyItem::getPrice() const {
    return price;
}

// Проверка рецепта: возвращает, требуется ли рецепт
bool PharmacyItem::isRecipeRequired() const {
    return recipe;
}

// Получение даты истечения: возвращает дату срока годности
QDate PharmacyItem::getExpirationDate() const {
    return expiration_date;
}

// Получение количества: возвращает количество медикамента
int PharmacyItem::getQuantity() const {
    return quantity;
}

// Получение ID аптеки: возвращает идентификатор аптеки
int PharmacyItem::getPharmacyId() const {
    return pharmacyId;
}

// Установка названия: изменяет название с проверкой
void PharmacyItem::setTitle(const QString& newTitle) {
    if (!isValidString(newTitle, "Название")) {
        showError("Ошибка: Название медикамента не может быть пустым!");
        return;
    }
    title = newTitle;
    Logger::instance().log("PharmacyItem", QString("Изменено название медикамента на %1").arg(newTitle));
}

// Установка цены: изменяет цену с проверкой
void PharmacyItem::setPrice(double newPrice) {
    if (newPrice < 0) {
        showError("Ошибка: Цена не может быть отрицательной!");
        return;
    }
    price = newPrice;
    Logger::instance().log("PharmacyItem", QString("Изменена цена медикамента %1 на %2").arg(title).arg(newPrice));
}

// Установка требования рецепта: изменяет флаг рецепта
void PharmacyItem::setRecipeRequired(bool newRecipe) {
    recipe = newRecipe;
    Logger::instance().log("PharmacyItem", QString("Требование рецепта для %1 изменено на %2").arg(title).arg(newRecipe ? "да" : "нет"));
}

// Установка даты истечения: изменяет дату срока годности с проверкой
void PharmacyItem::setExpirationDate(const QDate& newExpirationDate) {
    if (!newExpirationDate.isValid()) {
        showError("Ошибка: Некорректная дата истечения срока годности!");
        return;
    }
    expiration_date = newExpirationDate;
    Logger::instance().log("PharmacyItem", QString("Изменена дата истечения для %1 на %2").arg(title).arg(newExpirationDate.toString(Qt::ISODate)));
}

// Установка количества: изменяет количество с проверкой
void PharmacyItem::setQuantity(int newQuantity) {
    if (newQuantity < 0) {
        showError("Ошибка: Количество не может быть отрицательным!");
        return;
    }
    quantity = newQuantity;
    Logger::instance().log("PharmacyItem", QString("Изменено количество медикамента %1 на %2").arg(title).arg(newQuantity));
}

// Установка ID аптеки: изменяет идентификатор аптеки с проверкой
void PharmacyItem::setPharmacyId(int newPharmacyId) {
    if (newPharmacyId < 0) {
        showError("Ошибка: ID аптеки не может быть отрицательным!");
        return;
    }
    pharmacyId = newPharmacyId;
    Logger::instance().log("PharmacyItem", QString("Изменён ID аптеки для %1 на %2").arg(title).arg(newPharmacyId));
}

// Проверка просрочки: определяет, истек ли срок годности медикамента
bool PharmacyItem::isExpired() const {
    bool expired = expiration_date < QDate::currentDate();
    Logger::instance().log("PharmacyItem", QString("Проверка медикамента %1: %2").arg(title).arg(expired ? "просрочен" : "действителен"));
    return expired;
}

// Сравнение медикаментов: проверяет равенство по названию и ID аптеки
bool PharmacyItem::operator==(const PharmacyItem &other) const {
    bool equal = title == other.title && pharmacyId == other.pharmacyId;
    Logger::instance().log("PharmacyItem", QString("Сравнение медикамента %1 с %2: %3").arg(title).arg(other.title).arg(equal ? "равны" : "различны"));
    return equal;
}

// Вывод ошибки: показывает сообщение об ошибке пользователю
void PharmacyItem::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("PharmacyItem", message);
}

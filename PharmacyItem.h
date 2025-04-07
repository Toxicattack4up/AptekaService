#ifndef PHARMACYITEM_H
#define PHARMACYITEM_H

#include <QString>
#include <QDate>

class PharmacyItem {
private:
    QString title;         // Название лекарства
    double price;          // Цена лекарства
    bool recipe;           // Требуется ли рецепт
    QDate expiration_date; // Дата окончания срока годности
    int quantity;          // Количество на складе

public:
    // Конструктор: создаёт медикамент с заданными параметрами
    PharmacyItem(const QString& title, double price, bool recipe, const QDate& expiration_date, int quantity);

    // Геттеры: возвращают значения полей
    QString getTitle() const;
    double getPrice() const;
    bool isRecipeRequired() const;
    QDate getExpirationDate() const;
    int getQuantity() const;

    // Сеттеры: устанавливают новые значения с проверкой корректности
    void setTitle(const QString& newTitle);
    void setPrice(double newPrice);
    void setRecipeRequired(bool newRecipe);
    void setExpirationDate(const QDate& newExpirationDate);
    void setQuantity(int newQuantity);
};

#endif // PHARMACYITEM_H

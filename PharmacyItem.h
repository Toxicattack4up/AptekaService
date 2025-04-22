#ifndef PHARMACYITEM_H
#define PHARMACYITEM_H

#include <QString>
#include <QDate>
#include <QDebug>

class PharmacyItem {
public:
    PharmacyItem(const QString& title = "", double price = 0.0, bool recipe = false,
                 const QDate &expiration_date = QDate(), int quantity = 0);
    QString getTitle() const;
    double getPrice() const;
    bool isRecipeRequired() const;
    QDate getExpirationDate() const;
    int getQuantity() const;
    void setTitle(const QString& newTitle);
    void setPrice(double newPrice);
    void setRecipeRequired(bool newRecipe);
    void setExpirationDate(const QDate& newExpirationDate);
    void setQuantity(int newQuantity);
    bool operator==(const PharmacyItem &other) const;

    int pharmacyId;

private:
    QString title;
    double price;
    bool recipe;
    QDate expiration_date;
    int quantity;
};

#endif // PHARMACYITEM_H

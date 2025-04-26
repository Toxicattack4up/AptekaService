#ifndef PHARMACYITEM_H
#define PHARMACYITEM_H

#include <QString>
#include <QDate>

class PharmacyItem {
public:
    PharmacyItem(const QString& title, double price, bool recipe, const QDate &expiration_date, int quantity);
    QString getTitle() const;
    double getPrice() const;
    bool isRecipeRequired() const;
    QDate getExpirationDate() const;
    int getQuantity() const;
    int getPharmacyId() const;  // Added getter for pharmacyId
    void setTitle(const QString& newTitle);
    void setPrice(double newPrice);
    void setRecipeRequired(bool newRecipe);
    void setExpirationDate(const QDate& newExpirationDate);
    void setQuantity(int newQuantity);
    void setPharmacyId(int newPharmacyId);  // Added setter for pharmacyId
    bool isExpired() const;
    bool operator==(const PharmacyItem &other) const;

private:
    QString title;
    double price;
    bool recipe;
    QDate expiration_date;
    int quantity;
    int pharmacyId;
    void showError(const QString& message);
};

#endif // PHARMACYITEM_H

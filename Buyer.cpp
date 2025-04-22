#include "Buyer.h"
#include <QRegularExpression>
#include <QDebug>

Buyer::Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Buyer, login, password, fullName, email) {
}

bool Buyer::registerBuyer(const QString& login, const QString& password,
                          const QString& fullName, const QString& email, JsonManager& jsonManager) {
    QString trimmedLogin = login.trimmed();
    QString trimmedPassword = password.trimmed();
    QString trimmedFullName = fullName.trimmed();
    QString trimmedEmail = email.trimmed();

    if (trimmedLogin.isEmpty() || trimmedPassword.isEmpty() ||
        trimmedFullName.isEmpty() || trimmedEmail.isEmpty()) {
        qDebug() << "Ошибка: Все поля должны быть заполнены!";
        return false;
    }

    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(trimmedEmail).hasMatch()) {
        qDebug() << "Ошибка: Неверный формат email!";
        return false;
    }

    jsonManager.addEmployee("Покупатель", trimmedLogin, trimmedPassword, trimmedFullName, trimmedEmail);
    qDebug() << "Покупатель успешно зарегистрирован!";
    return true;
}

double Buyer::getBalance(JsonManager& jsonManager) const {
    return jsonManager.getBuyerBalance(getLogin());
}

bool Buyer::depositBalance(JsonManager& jsonManager, double amount) {
    return jsonManager.depositBalance(getLogin(), amount);
}

bool Buyer::purchaseMedicine(const QString& medicineTitle, int quantity, int pharmacyId, JsonManager& jsonManager) {
    return jsonManager.makePurchase(medicineTitle, quantity, pharmacyId, getLogin());
}

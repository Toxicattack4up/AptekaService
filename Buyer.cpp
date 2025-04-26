#include "Buyer.h"
#include "logger.h"
#include <QMessageBox>

// Конструктор покупателя с валидацией данных
Buyer::Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Buyer, login, password, fullName, email) {
    if (login.isEmpty() || password.isEmpty() || fullName.isEmpty() || email.isEmpty()) {
        showError(QString("Ошибка: Пустые данные для покупателя %1").arg(login));
        return;
    }
    Logger::instance().log("Buyer", QString("Создан покупатель %1").arg(login));
}

// Авторизация покупателя
bool Buyer::login(const QString& login, const QString& password, JsonManager& jsonManager) {
    QString role = jsonManager.validateUser(login, password);
    if (role != "Покупатель") {
        showError(QString("Ошибка: Неверный логин или пароль для %1").arg(login));
        return false;
    }
    setLogin(login);
    Logger::instance().log("Buyer", QString("Покупатель %1 авторизован").arg(login));
    return true;
}

// Регистрация нового покупателя
bool Buyer::registerBuyer(const QString& login, const QString& password, const QString& fullName, const QString& email, JsonManager& jsonManager) {
    QString trimmedLogin = login.trimmed();
    QString trimmedPassword = password.trimmed();
    QString trimmedFullName = fullName.trimmed();
    QString trimmedEmail = email.trimmed();

    if (trimmedLogin.isEmpty() || trimmedPassword.isEmpty() || trimmedFullName.isEmpty() || trimmedEmail.isEmpty()) {
        showError("Ошибка: Все поля должны быть заполнены!");
        return false;
    }

    User tempUser;
    tempUser.setEmail(trimmedEmail); // Валидация email из User
    if (tempUser.getEmail().isEmpty()) {
        showError("Ошибка: Неверный формат email!");
        return false;
    }

    jsonManager.addEmployee("Покупатель", trimmedLogin, trimmedPassword, trimmedFullName, trimmedEmail);
    Logger::instance().log("Buyer", QString("Покупатель %1 зарегистрирован").arg(trimmedLogin));
    return true;
}

// Получение баланса покупателя
double Buyer::getBalance(JsonManager& jsonManager) const {
    double balance = jsonManager.getBuyerBalance(getLogin());
    Logger::instance().log("Buyer", QString("Баланс %1: %2").arg(getLogin()).arg(balance));
    return balance;
}

// Пополнение баланса
bool Buyer::depositBalance(JsonManager& jsonManager, double amount) {
    if (amount <= 0) {
        showError("Ошибка: Сумма пополнения должна быть больше 0!");
        return false;
    }
    bool success = jsonManager.depositBalance(getLogin(), amount);
    if (success) {
        Logger::instance().log("Buyer", QString("Баланс %1 пополнен на %2").arg(getLogin()).arg(amount));
    } else {
        showError("Ошибка: Не удалось пополнить баланс!");
    }
    return success;
}

// Покупка медикамента (1 единица)
bool Buyer::purchaseMedicine(const QString& medicineTitle, int pharmacyId, JsonManager& jsonManager) {
    if (medicineTitle.isEmpty()) {
        showError("Ошибка: Название медикамента не может быть пустым!");
        return false;
    }
    auto medicines = jsonManager.searchMedicine(medicineTitle);
    if (medicines.isEmpty()) {
        showError(QString("Ошибка: Медикамент %1 не найден").arg(medicineTitle));
        return false;
    }
    if (medicines.first().isExpired()) {
        showError(QString("Ошибка: Медикамент %1 просрочен").arg(medicineTitle));
        return false;
    }
    bool success = jsonManager.makePurchase(medicineTitle, 1, pharmacyId, getLogin());
    if (success) {
        Logger::instance().log("Buyer", QString("Куплен медикамент %1 в аптеке %2").arg(medicineTitle).arg(pharmacyId));
    } else {
        showError(QString("Ошибка: Не удалось купить %1").arg(medicineTitle));
    }
    return success;
}

// Получение списка аптек
QList<Pharmacy> Buyer::getAvailablePharmacies(JsonManager& jsonManager) const {
    auto pharmacies = jsonManager.getPharmacy();
    Logger::instance().log("Buyer", QString("Получен список аптек: %1").arg(pharmacies.size()));
    return pharmacies;
}

// Получение списка названий медикаментов в аптеке
QStringList Buyer::getAvailableMedicines(int pharmacyId, JsonManager& jsonManager) const {
    QStringList titles;
    for (const PharmacyItem& item : jsonManager.getMedicine()) {
        if (item.getPharmacyId() == pharmacyId && !item.isExpired()) {
            titles.append(item.getTitle());
        }
    }
    Logger::instance().log("Buyer", QString("Получен список медикаментов для аптеки %1: %2").arg(pharmacyId).arg(titles.size()));
    return titles;
}

// Вывод ошибки пользователю с логированием
void Buyer::showError(const QString& message) {
    QMessageBox::warning(nullptr, "Ошибка", message);
    Logger::instance().log("Buyer", message);
}

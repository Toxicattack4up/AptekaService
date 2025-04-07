#include "Buyer.h"
#include <QRegularExpression>
#include <QDebug>

// Конструктор: инициализирует покупателя с ролью Buyer
Buyer::Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Buyer, login, password, fullName, email) {
}

// Регистрация покупателя: добавляет нового покупателя через JsonManager
bool Buyer::registerBuyer(const QString& login, const QString& password,
                          const QString& fullName, const QString& email,
                          JsonManager& jsonManager) {
    // Удаляем лишние пробелы
    QString trimmedLogin = login.trimmed();
    QString trimmedPassword = password.trimmed();
    QString trimmedFullName = fullName.trimmed();
    QString trimmedEmail = email.trimmed();

    // Проверяем, что все поля заполнены
    if (trimmedLogin.isEmpty() || trimmedPassword.isEmpty() ||
        trimmedFullName.isEmpty() || trimmedEmail.isEmpty()) {
        qDebug() << "Ошибка: Все поля должны быть заполнены!";
        return false;
    }

    // Проверка формата email
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(trimmedEmail).hasMatch()) {
        qDebug() << "Ошибка: Неверный формат email!";
        return false;
    }

    // Добавляем покупателя через JsonManager (проверки уникальности там)
    jsonManager.addEmployee("Покупатель", trimmedLogin, trimmedPassword, trimmedFullName, trimmedEmail);
    qDebug() << "Покупатель успешно зарегистрирован!";
    return true;
}

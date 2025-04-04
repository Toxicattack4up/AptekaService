#include "Buyer.h"
#include <QRegularExpression>
#include <QDebug>
#include <QDateTime>

Buyer::Buyer()
    : User(UserRole::Buyer, "", "", "", "") {
    // Пустой конструктор для самостоятельной регистрации
}

Buyer::Buyer(const QString& login, const QString& password, const QString& fullName, const QString& email)
    : User(UserRole::Buyer, login, password, fullName, email) {
}

// Статический метод регистрации покупателя
bool Buyer::registerBuyer(const QString& login,
                          const QString& password,
                          const QString& fullName,
                          const QString& email,
                          JsonManager &jsonManager)
{
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

    // Проверка формата email с использованием регулярного выражения
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    if (!emailRegex.match(trimmedEmail).hasMatch()) {
        qDebug() << "Ошибка: Email имеет неверный формат";
        return false;
    }

    // Проверка длины пароля (минимум 8 символов)
    if (trimmedPassword.length() < 8) {
        qDebug() << "Ошибка: Пароль должен содержать не менее 8 символов";
        return false;
    }

    // Проверка уникальности логина. Здесь используем функцию поиска в базе (по роли "Покупатель")
    QList<User> existing = jsonManager.SearchEmployee("Покупатель", trimmedLogin, "", "", "");
    if (!existing.isEmpty()) {
        qDebug() << "Ошибка: Логин уже существует!";
        return false;
    }

    // Создаём нового покупателя и устанавливаем дату регистрации
    Buyer newBuyer(trimmedLogin, trimmedPassword, trimmedFullName, trimmedEmail);
    newBuyer.setRegistrationDate(QDateTime::currentDateTime());

    // Добавляем покупателя в базу. Предполагается, что метод AddEmployee сохраняет данные в оперативную память.
    jsonManager.AddEmployee("Покупатель", trimmedLogin, trimmedPassword, trimmedFullName, trimmedEmail);

    qDebug() << "Регистрация прошла успешно!";
    return true;
}

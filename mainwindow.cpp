#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Buyer.h"
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this); // Инициализация интерфейса
}

MainWindow::~MainWindow() {
    delete ui; // Освобождаем память
}

// Обработчик кнопки авторизации
void MainWindow::on_login_button_clicked() {
    QString login = ui->lineEdit_login->text().trimmed();
    QString pass = ui->lineEdit_pass->text().trimmed();

    // Проверяем пользователя
    QString role = jsonManager.validateUser(login, pass);
    if (role.isEmpty()) {
        return; // Ошибка уже обработана в validateUser
    }

    QMessageBox::information(this, "Успех", "Вы успешно вошли!");
    if (role == "Администратор") {
        ui->stackedWidget->setCurrentIndex(1); // Переход на страницу администратора
    } else if (role == "Продавец") {
        ui->stackedWidget->setCurrentIndex(2); // Переход на страницу продавца
    } else if (role == "Покупатель") {
        ui->stackedWidget->setCurrentIndex(0); // Пока нет страницы для покупателя
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
    }
}

// Переход на страницу регистрации
void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(3); // Страница регистрации
}

// Возврат на главную страницу
void MainWindow::on_cancelToMain_Button_clicked() {
    ui->stackedWidget->setCurrentIndex(0); // Главная страница
}

// Завершение регистрации
void MainWindow::on_Registration_Button_clicked() {
    QString login = ui->reglog_lineEdit->text().trimmed();
    QString password = ui->regpass_lineEdit->text().trimmed();
    QString fullName = ui->FIO_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();

    // Проверка email
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Неверный формат email!");
        return;
    }

    // Проверка пароля
    if (password.length() < 8) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен быть не менее 8 символов!");
        return;
    }

    // Регистрация покупателя
    jsonManager.addEmployee("Покупатель", login, password, fullName, email);
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
    ui->stackedWidget->setCurrentIndex(0); // Возврат на главную
}

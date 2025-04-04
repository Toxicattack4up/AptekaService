#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JsonManager.h"
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connect(ui->login_button, &QPushButton::clicked, this, &MainWindow::on_login_button_clicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_login_button_clicked()
{
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_pass->text();

    JsonManager json;
    QString role = json.ValidateUser(login, pass);

    if(role.isEmpty())
    {
        return;
    }

    QMessageBox::information(this, "Успех", "Вы успешно вошли!");

    if(role == "Администратор"){
    ui->stackedWidget->setCurrentIndex(2);
    } else if (role == "Продавец") {
        ui->stackedWidget->setCurrentIndex(3);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
    }
}


void MainWindow::on_registr_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_cancelToMain_Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_Registration_Button_clicked()
{
    QString login = ui->reglog_label->text().trimmed();
    if (login.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Логин не может быть пустым!");
        return;
    }

    QString password = ui->regpass_label->text().trimmed();
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пароль не может быть пустым!");
        return;
    }

    QString FIO = ui->FIO_label->text().trimmed();
    if (FIO.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "ФИО не может быть пустым!");
        return;
    }

    QString email = ui->email_label->text().trimmed();
    QRegularExpression emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Email не может быть пустым или имеет неверный формат");
        return;
    }


}


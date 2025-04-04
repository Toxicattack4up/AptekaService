#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JsonManager.h"
#include <QMessageBox>

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


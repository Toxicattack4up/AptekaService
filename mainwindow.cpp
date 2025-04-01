#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JsonManager.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->login_button, &QPushButton::clicked, this, &MainWindow::on_login_button_clicked);
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
    json.ValidateUser(login, pass);




}


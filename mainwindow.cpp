#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include "Employee.h"
#include "Buyer.h"
#include "Administrator.h"
#include "JsonManager.h"
#include "User.h"
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this); // Инициализация интерфейса
    loadEmployeesToTable(); // Загрузка таблицы на старте
}

MainWindow::~MainWindow() {
    delete ui; // Освобождаем память
}

//Загрузка таблицы пользователями
void MainWindow::loadEmployeesToTable() {
    JsonManager json;
    // Получаем указатель на QTableWidget (предполагается, что он добавлен в UI через дизайнер форм)
    QTableWidget *table = ui->table_employees;

    // Очищаем таблицу перед загрузкой новых данных
    table->clearContents();
    table->setRowCount(0);

    // Устанавливаем количество столбцов
    table->setColumnCount(5); // Логин, ФИО, Email, Дата регистрации, Роль

    // Устанавливаем заголовки столбцов
    table->setHorizontalHeaderLabels({
        "Логин",
        "ФИО",
        "Email",
        "Дата регистрации",
        "Роль"
    });

    // Получаем список сотрудников из JsonManager
    const QList<User> &employees = json.getEmployee(); // Предполагается, что есть метод getEmployees

    // Устанавливаем количество строк (равно количеству сотрудников)
    table->setRowCount(employees.size());

    // Заполняем таблицу данными
    for (int row = 0; row < employees.size(); ++row) {
        const User &user = employees[row];

        // Создаем элементы для каждой ячейки
        table->setItem(row, 0, new QTableWidgetItem(user.getLogin()));
        table->setItem(row, 1, new QTableWidgetItem(user.getFullName()));
        table->setItem(row, 2, new QTableWidgetItem(user.getEmail()));
        table->setItem(row, 3, new QTableWidgetItem(user.getRegistrationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(UserRoleHelper::toString(user.getRole())));

        // (Опционально) Делаем ячейки нередактируемыми
        for (int col = 0; col < 5; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    // Настраиваем таблицу
    table->resizeColumnsToContents(); // Автоматическая подстройка ширины столбцов
    table->horizontalHeader()->setStretchLastSection(true); // Растягиваем последний столбец
    table->setSelectionMode(QAbstractItemView::SingleSelection); // Выбор только одной строки
    table->setSelectionBehavior(QAbstractItemView::SelectRows); // Выбираем целую строку
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
        ui->stackedWidget->setCurrentIndex(12); // Переход на страницу продавца
    } else if (role == "Покупатель") {
        ui->stackedWidget->setCurrentIndex(0); // Переход на страницы для покупателя
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
    }
}
// Переход на страницу регистрации
void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(14); // Страница регистрации
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

    ui->reglog_lineEdit->clear();
    ui->regpass_lineEdit->clear();
    ui->FIO_lineEdit->clear();
    ui->email_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0); // Возврат на главную
}
void MainWindow::on_back_to_view_employee_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}
void MainWindow::on_back_to_menu_buyer_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_back_to_view_pharmacy_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void MainWindow::on_back_to_view_items_pharmacy_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void MainWindow::on_Back_to_login_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_back_to_menu_login_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_back_to_view_employees_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}
void MainWindow::on_back_to_menu_items_pharmacy_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_back_menu_admin_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_back_to_menu_login_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_back_to_admin_menu_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    loadEmployeesToTable();
}
void MainWindow::on_Admin_users_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}
void MainWindow::on_add_employees_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_delete_employees_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}
void MainWindow::on_add_pharmac_item_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
}
void MainWindow::on_remove_pharmacy_item_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(11);
}
void MainWindow::on_add_pharmacy_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}
void MainWindow::on_Admin_pharmacy_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void MainWindow::on_Admin_Item_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_add_item_pushButton_clicked()
{
    QString name_pharmacy = ui->item_name_lineEdit->text().trimmed();
    double price_pharmacy = ui->item_price_lineEdit->text().toDouble();
    int quality_pharmacy = ui->item_quantity_lineEdit->text().toInt();
    bool recipe = ui->checkBox->isChecked();
    QDate expirationDate = QDate::fromString(ui->dateEdit->text(), "dd-MM-yyyy");

    if(name_pharmacy.isEmpty() || price_pharmacy < 0 || quality_pharmacy < 0 || !expirationDate.isValid())
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте правильность введенных данных");
    }

    PharmacyItem item(name_pharmacy, price_pharmacy, recipe, expirationDate, quality_pharmacy);

    JsonManager json;
    json.addMedicine(item);
}
void MainWindow::on_add_pharmacy_pushButton_clicked()
{
    int id = ui->Id_pharmacy_lineEdit->text().toInt();
    QString adress = ui->adress_lineEdit->text().trimmed();
    double square = ui->square_lineEdit->text().toDouble();
    int cout = ui->countItems_lineEdit->text().toInt();

    if(adress.isEmpty() || square < 1 || cout < 1)
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте данные");
    }

    Pharmacy pharma(id, adress, square, cout);

    JsonManager json;
    json.addPharmacy(pharma);
}
void MainWindow::on_add_employee_pushButton_clicked()
{
    QString role = ui->role_comboBox_employee->currentText().trimmed();
    QString login = ui->login_lineEdit_employee->text().trimmed();
    QString pass = ui->password_lineEdit_employee->text().trimmed();
    QString FIO = ui->FIO_lineEdit_employee->text();
    QString email = ui->email_lineEdit_employee->text().trimmed();

    if(role.isEmpty() || login.isEmpty() || pass.isEmpty() || FIO.isEmpty() || email.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте данные");
    }

    JsonManager json;
    json.addEmployee(role, login, pass, FIO, email);

    ui->role_comboBox_employee->clear();
    ui->login_lineEdit_employee->clear();
    ui->password_lineEdit_employee->clear();
    ui->FIO_lineEdit_employee->clear();
    ui->email_lineEdit_employee->clear();

}
void MainWindow::on_remove_employees_Button_clicked()
{
    QString login = ui->remove_lineEdit->text().trimmed();
    JsonManager json;
    json.removeEmployee(login);
    QMessageBox::information(nullptr, "Успешно", "Пользователь успешно удален");
    ui->remove_lineEdit->clear();
}

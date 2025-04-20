#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include "JsonManager.h"
#include "User.h"
#include <QMessageBox>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    jsonManager = JsonManager();
    // Загрузка таблиц и ComboBox на старте
    loadEmployeesToTable();
    loadPharmacysToTable();
    loadMedicinesToTable();
    loadBuyesToTable();
    loadMedicinesToComboBox();
    loadSellerToTable();

    // Настройка QSpinBoxов
    ui->quantity_spinBox->setMinimum(1);
    ui->quantity_spinBox->setMaximum(1000);
    ui->quantity_spinBox->setValue(1);


}

MainWindow::~MainWindow() {
    delete ui;
}

// Загрузка таблицы пользователями
void MainWindow::loadEmployeesToTable() {
    QTableWidget *table = ui->table_employees;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Логин", "ФИО", "Email", "Дата регистрации", "Роль"});

    const QList<User> &employees = jsonManager.getEmployee();
    table->setRowCount(employees.size());

    for (int row = 0; row < employees.size(); ++row) {
        const User &user = employees[row];
        table->setItem(row, 0, new QTableWidgetItem(user.getLogin()));
        table->setItem(row, 1, new QTableWidgetItem(user.getFullName()));
        table->setItem(row, 2, new QTableWidgetItem(user.getEmail()));
        table->setItem(row, 3, new QTableWidgetItem(user.getRegistrationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(UserRoleHelper::toString(user.getRole())));

        for (int col = 0; col < 5; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadPharmacysToTable() {
    QTableWidget *table = ui->pharmacy_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Адрес", "Размер (кв.м.)", "Вместимость"});

    const QList<Pharmacy> &pharmacies = jsonManager.getPharmacy();
    table->setRowCount(pharmacies.size());

    for (int row = 0; row < pharmacies.size(); ++row) {
        const Pharmacy &pharmacy = pharmacies[row];
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pharmacy.getId())));
        table->setItem(row, 1, new QTableWidgetItem(pharmacy.getAddress()));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(pharmacy.getSize(), 'f', 2)));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(pharmacy.getMaxCapacity())));

        for (int col = 0; col < 4; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadMedicinesToTable() {
    QTableWidget *table = ui->items_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Срок годности", "Требуется рецепт"});

    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    table->setRowCount(medicines.size());

    for (int row = 0; row < medicines.size(); ++row) {
        const PharmacyItem &medicine = medicines[row];
        table->setItem(row, 0, new QTableWidgetItem(medicine.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(medicine.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(medicine.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(medicine.getExpirationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(medicine.isRecipeRequired() ? "Да" : "Нет"));

        for (int col = 0; col < 5; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadBuyesToTable() {
    QTableWidget *table = ui->table_pharmacy_items;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Срок годности", "Требуется рецепт"});

    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    qDebug() << "Загрузка медикаментов в таблицу table_pharmacy_items. Количество:" << medicines.size();
    for (const PharmacyItem &item : medicines) {
        qDebug() << "Медикамент:" << item.getTitle() << "Количество:" << item.getQuantity();
    }

    table->setRowCount(medicines.size());

    for (int row = 0; row < medicines.size(); ++row) {
        const PharmacyItem &medicine = medicines[row];
        table->setItem(row, 0, new QTableWidgetItem(medicine.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(medicine.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(medicine.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(medicine.getExpirationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(medicine.isRecipeRequired() ? "Да" : "Нет"));

        for (int col = 0; col < 5; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    if (medicines.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет медикаментов"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
    }
}

void MainWindow::loadMedicinesToComboBox() {
    ui->buy_comboBox->clear();
    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    for (const PharmacyItem &item : medicines) {
        ui->buy_comboBox->addItem(item.getTitle());
    }
    if (medicines.isEmpty()) {
        ui->buy_comboBox->addItem("Нет доступных медикаментов");
        ui->buy_comboBox->setEnabled(false);
    } else {
        ui->buy_comboBox->setEnabled(true);
    }
}

void MainWindow::loadSellerToTable() {
    QTableWidget *table = ui->seller_tableWidget;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Срок годности", "Требуется рецепт"});

    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    table->setRowCount(medicines.size());

    for (int row = 0; row < medicines.size(); ++row) {
        const PharmacyItem &medicine = medicines[row];
        table->setItem(row, 0, new QTableWidgetItem(medicine.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(medicine.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(medicine.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(medicine.getExpirationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(medicine.isRecipeRequired() ? "Да" : "Нет"));

        for (int col = 0; col < 5; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadSellerToComboBox() {
    ui->sell_comboBox->clear();
    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    for (const PharmacyItem &item : medicines) {
        ui->sell_comboBox->addItem(item.getTitle());
    }
    if (medicines.isEmpty()) {
        ui->sell_comboBox->addItem("Нет доступных медикаментов");
        ui->sell_comboBox->setEnabled(false);
    } else {
        ui->sell_comboBox->setEnabled(true);
    }
}

// АВТОРИЗАЦИЯ
void MainWindow::on_login_button_clicked() {
    QString login = ui->lineEdit_login->text().trimmed();
    QString pass = ui->lineEdit_pass->text().trimmed();

    QString role = jsonManager.validateUser(login, pass);
    if (role.isEmpty()) {
        return;
    }
    loadBuyesToTable();
    QMessageBox::information(this, "Успех", "Вы успешно вошли!");
    if (role == "Администратор") {
        loadEmployeesToTable();
        ui->stackedWidget->setCurrentIndex(1);
    } else if (role == "Продавец") {
        loadSellerToTable();
        ui->stackedWidget->setCurrentIndex(13);
    } else if (role == "Покупатель") {
        loadBuyesToTable();
        ui->stackedWidget->setCurrentIndex(6);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
    }

    ui->lineEdit_login->clear();
    ui->lineEdit_pass->clear();
}

void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(15);
}

void MainWindow::on_cancelToMain_Button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_Registration_Button_clicked() {
    QString login = ui->reglog_lineEdit->text().trimmed();
    QString password = ui->regpass_lineEdit->text().trimmed();
    QString fullName = ui->FIO_lineEdit->text().trimmed();
    QString email = ui->email_lineEdit->text().trimmed();

    QRegularExpression emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9-]+\.[a-zA-Z]{2,}$)");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Неверный формат email!");
        return;
    }

    if (password.length() < 8) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен быть не менее 8 символов!");
        return;
    }

    jsonManager.addEmployee("Покупатель", login, password, fullName, email);
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");

    ui->reglog_lineEdit->clear();
    ui->regpass_lineEdit->clear();
    ui->FIO_lineEdit->clear();
    ui->email_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_to_view_employee_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

void MainWindow::on_back_to_menu_buyer_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_to_view_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    loadMedicinesToTable();
}

void MainWindow::on_back_to_view_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    loadPharmacysToTable();
}

void MainWindow::on_Back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_to_menu_login_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_to_view_employees_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

void MainWindow::on_back_to_menu_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
}

void MainWindow::on_back_menu_admin_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_back_to_menu_login_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_back_to_admin_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadEmployeesToTable();
}

void MainWindow::on_Admin_users_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

void MainWindow::on_add_employees_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_delete_employees_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_add_pharmac_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(11);
    ui->dateEdit->setDate(QDate::currentDate());
}

void MainWindow::on_remove_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(12);
}

void MainWindow::on_add_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(9);
}

void MainWindow::on_Admin_pharmacy_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    loadPharmacysToTable();
}

void MainWindow::on_Admin_Item_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    loadMedicinesToTable();
}

void MainWindow::on_add_item_pushButton_clicked() {
    QString name_pharmacy = ui->item_name_lineEdit->text().trimmed();
    double price_pharmacy = ui->item_price_lineEdit->text().toDouble();
    int quality_pharmacy = ui->item_quantity_lineEdit->text().toInt();
    bool recipe = ui->checkBox->isChecked();
    QDate expirationDate = ui->dateEdit->date();

    if (name_pharmacy.isEmpty() || price_pharmacy < 0 || quality_pharmacy < 0) {
        QMessageBox::warning(this, "Ошибка", "Поля не могут быть пустыми или отрицательными");
        return;
    }
    if (!expirationDate.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Выберите корректную дату срока годности");
        return;
    }
    if (expirationDate < QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Срок годности не может быть в прошлом");
        return;
    }

    PharmacyItem item(name_pharmacy, price_pharmacy, recipe, expirationDate, quality_pharmacy);
    jsonManager.addMedicine(item);

    ui->item_name_lineEdit->clear();
    ui->item_price_lineEdit->clear();
    ui->item_quantity_lineEdit->clear();
    ui->checkBox->setChecked(false);
    ui->dateEdit->setDate(QDate::currentDate());

    loadMedicinesToTable();
    loadBuyesToTable(); // Обновляем обе таблицы
    loadMedicinesToComboBox(); // Обновляем ComboBox
}

void MainWindow::on_add_pharmacy_pushButton_clicked() {
    int id = ui->Id_pharmacy_lineEdit->text().toInt();
    QString adress = ui->adress_lineEdit->text().trimmed();
    double square = ui->square_lineEdit->text().toDouble();
    int cout = ui->countItems_lineEdit->text().toInt();

    if (adress.isEmpty() || square < 1 || cout < 1) {
        QMessageBox::warning(this, "Ошибка", "Не все поля заполнены, заполните поля");
        return;
    }

    Pharmacy pharma(id, adress, square, cout);
    jsonManager.addPharmacy(pharma);

    ui->Id_pharmacy_lineEdit->clear();
    ui->adress_lineEdit->clear();
    ui->square_lineEdit->clear();
    ui->countItems_lineEdit->clear();

    loadPharmacysToTable();
}

void MainWindow::on_add_employee_pushButton_clicked() {
    QString role = ui->role_comboBox_employee->currentText().trimmed();
    QString login = ui->login_lineEdit_employee->text().trimmed();
    QString pass = ui->password_lineEdit_employee->text().trimmed();
    QString FIO = ui->FIO_lineEdit_employee->text();
    QString email = ui->email_lineEdit_employee->text().trimmed();

    if (role.isEmpty() || login.isEmpty() || pass.isEmpty() || FIO.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Проверьте данные");
        return;
    }

    jsonManager.addEmployee(role, login, pass, FIO, email);

    ui->role_comboBox_employee->clear();
    ui->login_lineEdit_employee->clear();
    ui->password_lineEdit_employee->clear();
    ui->FIO_lineEdit_employee->clear();
    ui->email_lineEdit_employee->clear();

    loadEmployeesToTable();
}

void MainWindow::on_remove_employees_Button_clicked() {
    QString login = ui->remove_lineEdit->text().trimmed();
    jsonManager.removeEmployee(login);
    ui->remove_lineEdit->clear();
    QMessageBox::information(this, "Успешно", "Сотрудник успешно удален");
    loadEmployeesToTable();
}

void MainWindow::on_back_to_view_menu_admin_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_remove_pharmacy_pushButton_2_clicked() {
    int id = ui->removeID_lineEdit->text().toInt();
    jsonManager.removePharmacy(id);
    ui->removeID_lineEdit->clear();
    loadPharmacysToTable();
    ui->stackedWidget->setCurrentIndex(7);
}

void MainWindow::on_back_to_view_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
}

void MainWindow::on_remove_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
}

void MainWindow::on_remove_pharmacy_item_pushButton_2_clicked() {
    QString title_rm_medicine = ui->name_pharmacy_item_lineEdit->text().trimmed();
    jsonManager.removeMedicine(title_rm_medicine);
    ui->name_pharmacy_item_lineEdit->clear();
    loadMedicinesToTable();
    loadBuyesToTable();
    loadMedicinesToComboBox();
}

void MainWindow::on_Buy_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
}

// Кнопка покупки
void MainWindow::on_buy_pushButton_clicked() {
    QString name_medicine = ui->buy_comboBox->currentText().trimmed();
    if (name_medicine.isEmpty() || name_medicine == "Нет доступных медикаментов") {
        QMessageBox::warning(this, "Ошибка", "Выберите медикамент для покупки");
        return;
    }

    int quantity = ui->quantity_spinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть положительным");
        return;
    }

    if (jsonManager.makePurchase(name_medicine, quantity)) {
        loadMedicinesToTable();
        loadBuyesToTable();
        loadMedicinesToComboBox();
        loadSellerToTable();
        loadSellerToComboBox();
        ui->quantity_spinBox->setValue(1);
    }
}

void MainWindow::on_back_to_view_buys_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
    loadBuyesToTable();
}

void MainWindow::on_buy_pharmacy_item_pushButton_clicked() {
    ui->quantity_spinBox->setValue(0);
    ui->stackedWidget->setCurrentIndex(14);
}

void MainWindow::on_Cancel_seller_menu_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// Назад в меню продавца
void MainWindow::on_back_to_seller_menu_pushButton_clicked()
{
    loadSellerToTable();
    ui->stackedWidget->setCurrentIndex(13);
}

// Переход на страницу продажи
void MainWindow::on_Sell_button_clicked()
{
    loadSellerToComboBox();
    ui->stackedWidget->setCurrentIndex(16);
}

// Кнопка продажи лекарства
void MainWindow::on_sell_pushButton_clicked()
{
    QString name_medicine = ui->sell_comboBox->currentText().trimmed();
    if (name_medicine.isEmpty() || name_medicine == "Нет доступных медикаментов") {
        QMessageBox::warning(this, "Ошибка", "Выберите медикамент для покупки");
        return;
    }

    int quantity = ui->sell_quantity_spinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть положительным");
        return;
    }

    if (jsonManager.makePurchase(name_medicine, quantity)) {
        loadMedicinesToTable();
        loadBuyesToTable();
        loadMedicinesToComboBox();
        loadSellerToTable();
        loadSellerToComboBox();
        ui->sell_quantity_spinBox->setValue(1);
    }
}


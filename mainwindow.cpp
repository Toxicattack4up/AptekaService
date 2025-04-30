#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include "JsonManager.h"
#include "UserRoleHelper.h"
#include "User.h"
#include "HistoryManager.h"
#include <QMessageBox>
#include <QRegularExpression>
#include "logger.h"

// Конструктор: инициализация UI и начальных данных
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), jsonManager(), warehouse() {
    ui->setupUi(this);
    setWindowTitle("Аптечная система");
    Logger::instance().log("MainWindow", "Приложение запущено");

    ui->stackedWidget->setCurrentIndex(0);
    // Инициализация ролей
    initializeRoleComboBox();
    // Подключение сигнала для изменения роли
    connect(ui->role_comboBox_employee, &QComboBox::currentTextChanged,
            this, &MainWindow::on_role_comboBox_employee_currentIndexChanged);
    // Загрузка таблиц и ComboBox на старте
    loadEmployeesToTable();
    loadPharmacysToTable();
    loadMedicinesToTable();
    loadBuyesToTable();
    loadMedicinesToComboBox();
    loadSellerToTable();
    loadSellerToComboBox();
    loadWarehouseToTable(); // Загрузка таблицы склада

    // Настройка QSpinBoxов
    ui->quantity_spinBox->setMinimum(1);
    ui->quantity_spinBox->setMaximum(1000);
    ui->quantity_spinBox->setValue(1);
    ui->sell_quantity_spinBox->setMinimum(1);
    ui->sell_quantity_spinBox->setMaximum(1000);
    ui->sell_quantity_spinBox->setValue(1);

    // Подключение сигнала для обновления таблиц при переключении страниц
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        if (index == 1 || index == 2) { // Страница администратора или сотрудников
            loadEmployeesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица сотрудников");
        } else if (index == 7) { // Страница аптек
            loadPharmacysToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица аптек");
        } else if (index == 10) { // Страница лекарств
            loadMedicinesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица лекарств");
        } else if (index == 6) { // Страница покупателя
            loadBuyesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица покупок");
        } else if (index == 13) { // Страница продавца
            loadSellerToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица продавца");
        } else if (index == 5) { // Страница курьера
            loadCourierToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица курьера");
        }
    });
}

// Деструктор: освобождение ресурсов
MainWindow::~MainWindow() {
    delete ui;
}

// loadEmployeesToTable: загрузка списка сотрудников в таблицу
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
    Logger::instance().log("MainWindow", QString("Таблица сотрудников загружена: %1 записей").arg(employees.size()));
}

// loadPharmacysToTable: загрузка списка аптек в таблицу
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
    Logger::instance().log("MainWindow", QString("Таблица аптек загружена: %1 записей").arg(pharmacies.size()));
}

// loadMedicinesToTable: загрузка списка лекарств в таблицу
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
    Logger::instance().log("MainWindow", QString("Таблица лекарств загружена: %1 записей").arg(medicines.size()));
}

// loadBuyesToTable: загрузка списка лекарств для покупок
void MainWindow::loadBuyesToTable() {
    QTableWidget *table = ui->table_pharmacy_items;
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

    if (medicines.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет медикаментов"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
    }
    Logger::instance().log("MainWindow", QString("Таблица покупок загружена: %1 записей").arg(medicines.size()));
}

// loadMedicinesToComboBox: заполнение ComboBox лекарств для покупки
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
    Logger::instance().log("MainWindow", QString("ComboBox лекарств загружен: %1 элементов").arg(medicines.size()));
}

// loadSellerToTable: загрузка таблицы лекарств для продавца
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
    Logger::instance().log("MainWindow", QString("Таблица продавца загружена: %1 записей").arg(medicines.size()));
}

// loadSellerToComboBox: заполнение ComboBox лекарств для продажи
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
    Logger::instance().log("MainWindow", QString("ComboBox продавца загружен: %1 элементов").arg(medicines.size()));
}

// loadWarehouseToTable: загрузка таблицы общего склада
void MainWindow::loadWarehouseToTable() {
    QTableWidget *table = ui->view_medicine_item_warehouse;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Срок годности", "Требуется рецепт"});

    const QList<PharmacyItem> &items = warehouse.getMedicines();
    table->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const PharmacyItem &item = items[row];
        table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(item.getExpirationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));

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
    Logger::instance().log("MainWindow", QString("Таблица склада загружена: %1 записей").arg(items.size()));
}

// loadCourierToTable: загрузка таблицы для курьера
void MainWindow::loadCourierToTable() {
    QTableWidget *table = ui->view_medicine_item_warehouse;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Срок годности", "Требуется рецепт"});

    const QList<PharmacyItem> &items = warehouse.getMedicines();
    table->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const PharmacyItem &item = items[row];
        table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(item.getExpirationDate().toString("yyyy-MM-dd")));
        table->setItem(row, 4, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));

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
    Logger::instance().log("MainWindow", QString("Таблица курьера загружена: %1 записей").arg(items.size()));
}

// on_login_button_clicked: авторизация пользователя
void MainWindow::on_login_button_clicked() {
    QString login = ui->lineEdit_login->text().trimmed();
    QString pass = ui->lineEdit_pass->text().trimmed();

    QString role = jsonManager.validateUser(login, pass);
    if (role.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!");
        Logger::instance().log("MainWindow", QString("Ошибка авторизации: неверный логин %1").arg(login));
        return;
    }
    currentUserLogin = login;
    currentUserRole = UserRoleHelper::fromString(role);
    QMessageBox::information(this, "Успех", "Вы успешно вошли!");
    Logger::instance().log("MainWindow", QString("Пользователь %1 (%2) авторизован").arg(login).arg(role));

    if (role == "Администратор") {
        loadEmployeesToTable();
        ui->stackedWidget->setCurrentIndex(1);
    } else if (role == "Продавец") {
        loadSellerToTable();
        ui->stackedWidget->setCurrentIndex(13);
    } else if (role == "Покупатель") {
        loadBuyesToTable();
        ui->stackedWidget->setCurrentIndex(6);
    } else if (role == "Курьер") {
        loadCourierToTable();
        ui->stackedWidget->setCurrentIndex(5);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
        Logger::instance().log("MainWindow", QString("Ошибка: неизвестная роль %1 для %2").arg(role).arg(login));
    }

    ui->lineEdit_login->clear();
    ui->lineEdit_pass->clear();
}

// on_registr_button_clicked: переход к форме регистрации
void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(15);
}

// on_Registration_Button_clicked: регистрация нового покупателя
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

    jsonManager.addEmployee("Покупатель", login, password, fullName, email, 0);
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
    Logger::instance().log("MainWindow", QString("Зарегистрирован покупатель %1").arg(login));

    ui->reglog_lineEdit->clear();
    ui->regpass_lineEdit->clear();
    ui->FIO_lineEdit->clear();
    ui->email_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// on_cancelToMain_Button_clicked: возврат к главному меню
void MainWindow::on_cancelToMain_Button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_view_employee_pushButton_clicked: возврат к списку сотрудников
void MainWindow::on_back_to_view_employee_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    ui->pharmacyID_comboBox->setVisible(false);
    ui->pharmacyID_label->setVisible(false); // Скрываем метку
    initializeRoleComboBox();
    loadEmployeesToTable();
    Logger::instance().log("MainWindow", "Возврат к списку сотрудников");
}

// on_back_to_menu_buyer_pushButton_clicked: возврат к меню покупателя
void MainWindow::on_back_to_menu_buyer_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_view_pharmacy_pushButton_clicked: переход к списку лекарств
void MainWindow::on_back_to_view_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    loadMedicinesToTable();
}

// on_back_to_view_items_pharmacy_pushButton_clicked: возврат к списку аптек
void MainWindow::on_back_to_view_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    loadPharmacysToTable();
}

// on_Back_to_login_clicked: выход на экран авторизации
void MainWindow::on_Back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_menu_login_pushButton_clicked: возврат к главному меню
void MainWindow::on_back_to_menu_login_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_view_employees_clicked: переход к списку сотрудников
void MainWindow::on_back_to_view_employees_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// on_back_to_menu_items_pharmacy_pushButton_clicked: возврат к списку лекарств
void MainWindow::on_back_to_menu_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
}

// on_back_menu_admin_pushButton_clicked: возврат к меню администратора
void MainWindow::on_back_menu_admin_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// on_back_to_menu_login_pushButton_2_clicked: выход на экран авторизации
void MainWindow::on_back_to_menu_login_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_admin_menu_pushButton_clicked: возврат к меню администратора
void MainWindow::on_back_to_admin_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadEmployeesToTable();
}

// on_Admin_users_clicked: переход к управлению сотрудниками
void MainWindow::on_Admin_users_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// on_add_employees_pushButton_clicked: открытие формы добавления сотрудника
void MainWindow::on_add_employees_pushButton_clicked() {
    initializeRoleComboBox();
    ui->stackedWidget->setCurrentIndex(3);
    ui->pharmacyID_comboBox->setVisible(ui->role_comboBox_employee->currentText() == "Продавец");
    ui->pharmacyID_label->setVisible(ui->role_comboBox_employee->currentText() == "Продавец"); // Скрываем/показываем метку

    ui->pharmacyID_comboBox->clear();
    QList<Pharmacy> pharmacies = jsonManager.getPharmacy();
    if (pharmacies.isEmpty()) {
        ui->pharmacyID_comboBox->addItem("Нет доступных аптек");
        ui->add_employee_pushButton->setEnabled(false);
    } else {
        for (const Pharmacy& pharmacy : pharmacies) {
            ui->pharmacyID_comboBox->addItem(QString::number(pharmacy.getId()));
        }
        ui->add_employee_pushButton->setEnabled(true);
    }

    Logger::instance().log("MainWindow", "Открыта форма добавления сотрудника");
}

// on_delete_employees_pushButton_clicked: переход к удалению сотрудника
void MainWindow::on_delete_employees_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
}

// on_add_pharmac_item_pushButton_clicked: переход к добавлению лекарства
void MainWindow::on_add_pharmac_item_pushButton_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->item_name_lineEdit->clear();
    ui->item_price_lineEdit->clear();
    ui->item_quantity_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(11);
}

// on_remove_pharmacy_item_pushButton_clicked: переход к удалению лекарства
void MainWindow::on_remove_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(12);
}

// on_add_pharmacy_pushButton_2_clicked: переход к добавлению аптеки
void MainWindow::on_add_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(9);
    ui->Id_pharmacy_lineEdit->clear();
    ui->adress_lineEdit->clear();
    ui->square_lineEdit->clear();
    ui->countItems_lineEdit->clear();
}

// on_Admin_pharmacy_clicked: переход к управлению аптеками
void MainWindow::on_Admin_pharmacy_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    loadPharmacysToTable();
}

// on_Admin_Item_clicked: переход к управлению лекарствами
void MainWindow::on_Admin_Item_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    loadMedicinesToTable();
}

// on_add_item_pushButton_clicked: добавление нового лекарства
void MainWindow::on_add_item_pushButton_clicked() {
    QString name_pharmacy = ui->item_name_lineEdit->text().trimmed();
    QString priceText = ui->item_price_lineEdit->text().trimmed();
    QString quantityText = ui->item_quantity_lineEdit->text().trimmed();
    bool recipe = ui->checkBox->isChecked();
    QDate expirationDate = ui->dateEdit->date();

    bool priceOk, quantityOk;
    double price_pharmacy = priceText.toDouble(&priceOk);
    int quality_pharmacy = quantityText.toInt(&quantityOk);

    Logger::instance().log("MainWindow", QString("Добавление лекарства: name='%1', price=%2, quantity=%3")
                                             .arg(name_pharmacy).arg(price_pharmacy).arg(quality_pharmacy));

    if (!priceOk || !quantityOk || name_pharmacy.isEmpty() || price_pharmacy <= 0 || quality_pharmacy <= 0) {
        QString errorMsg = "Проверьте данные: ";
        if (name_pharmacy.isEmpty()) errorMsg += "название пустое, ";
        if (!priceOk || price_pharmacy <= 0) errorMsg += "цена некорректна, ";
        if (!quantityOk || quality_pharmacy <= 0) errorMsg += "количество некорректно";
        QMessageBox::warning(this, "Ошибка", errorMsg);
        Logger::instance().log("MainWindow", QString("Ошибка добавления лекарства: некорректные данные"));
        return;
    }
    if (!expirationDate.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Выберите корректную дату срока годности");
        Logger::instance().log("MainWindow", "Ошибка добавления лекарства: некорректная дата срока годности");
        return;
    }
    if (expirationDate < QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Срок годности не может быть в прошлом");
        Logger::instance().log("MainWindow", "Ошибка добавления лекарства: срок годности в прошлом");
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
    loadBuyesToTable();
    loadMedicinesToComboBox();
    loadSellerToTable();
    loadSellerToComboBox();
    Logger::instance().log("MainWindow", QString("Добавлено лекарство %1 на склад").arg(name_pharmacy));
    HistoryManager::instance().addOperation(currentUserLogin, "Добавление лекарства",
                                            QString("Добавлено %1 (%2 единиц) на склад").arg(name_pharmacy).arg(quality_pharmacy));
}

// on_add_pharmacy_pushButton_clicked: добавление новой аптеки
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
    Logger::instance().log("MainWindow", QString("Добавлена аптека ID %1").arg(id));
}

// on_add_employee_pushButton_clicked: добавление нового сотрудника
void MainWindow::on_add_employee_pushButton_clicked() {
    QString role = ui->role_comboBox_employee->currentText().trimmed();
    QString login = ui->login_lineEdit_employee->text().trimmed();
    QString password = ui->password_lineEdit_employee->text().trimmed();
    QString fullName = ui->FIO_lineEdit_employee->text().trimmed();
    QString email = ui->email_lineEdit_employee->text().trimmed();
    int pharmacyId = 0;

    if (role.isEmpty() || login.isEmpty() || password.isEmpty() || fullName.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля должны быть заполнены!");
        Logger::instance().log("MainWindow", "Ошибка: Поля не заполнены");
        return;
    }

    if (role == "Продавец") {
        if (ui->pharmacyID_comboBox->currentText() == "Нет доступных аптек") {
            QMessageBox::warning(this, "Ошибка", "Выберите аптеку для продавца!");
            Logger::instance().log("MainWindow", "Ошибка: Не выбрана аптека");
            return;
        }
        pharmacyId = ui->pharmacyID_comboBox->currentText().toInt();
    }

    QString savedLogin = ui->login_lineEdit_employee->text();
    QString savedPassword = ui->password_lineEdit_employee->text();
    QString savedFullName = ui->FIO_lineEdit_employee->text();
    QString savedEmail = ui->email_lineEdit_employee->text();
    QString savedRole = ui->role_comboBox_employee->currentText();

    jsonManager.addEmployee(role, login, password, fullName, email, pharmacyId);

    QList<User> users = jsonManager.searchEmployee(role, login, "", "");
    if (!users.isEmpty() && users.first().getLogin() == login) {
        HistoryManager::instance().addOperation(currentUserLogin, "Добавление сотрудника",
                                                QString("Добавлен %1 (%2), аптека ID %3").arg(login).arg(role).arg(pharmacyId));
        ui->login_lineEdit_employee->clear();
        ui->password_lineEdit_employee->clear();
        ui->FIO_lineEdit_employee->clear();
        ui->email_lineEdit_employee->clear();
        ui->pharmacyID_comboBox->setVisible(false);
        ui->pharmacyID_label->setVisible(false); // Скрываем метку
        ui->stackedWidget->setCurrentIndex(2);
        loadEmployeesToTable();
        Logger::instance().log("MainWindow", QString("Сотрудник %1 добавлен").arg(login));
    } else {
        ui->login_lineEdit_employee->setText(savedLogin);
        ui->password_lineEdit_employee->setText(savedPassword);
        ui->FIO_lineEdit_employee->setText(savedFullName);
        ui->email_lineEdit_employee->setText(savedEmail);
        ui->role_comboBox_employee->setCurrentText(savedRole);
        initializeRoleComboBox();
        Logger::instance().log("MainWindow", QString("Ошибка добавления сотрудника %1").arg(login));
    }
}

// on_remove_employees_Button_clicked: удаление сотрудника
void MainWindow::on_remove_employees_Button_clicked() {
    QString login = ui->remove_lineEdit->text().trimmed();
    if (jsonManager.removeEmployee(login)) {
        ui->remove_lineEdit->clear();
        QMessageBox::information(this, "Успех", "Сотрудник успешно удалён");
        loadEmployeesToTable();
        Logger::instance().log("MainWindow", QString("Удалён сотрудник %1").arg(login));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Сотрудник %1 не найден").arg(login));
        Logger::instance().log("MainWindow", QString("Ошибка: Сотрудник %1 не найден").arg(login));
    }
}

// on_back_to_view_menu_admin_pushButton_2_clicked: возврат к меню администратора
void MainWindow::on_back_to_view_menu_admin_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// on_remove_pharmacy_pushButton_2_clicked: удаление аптеки
void MainWindow::on_remove_pharmacy_pushButton_2_clicked() {
    int id = ui->removeID_lineEdit->text().toInt();
    if (jsonManager.removePharmacy(id)) {
        ui->removeID_lineEdit->clear();
        loadPharmacysToTable();
        ui->stackedWidget->setCurrentIndex(7);
        QMessageBox::information(this, "Успех", "Аптека успешно удалена");
        Logger::instance().log("MainWindow", QString("Удалена аптека ID %1").arg(id));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Аптека ID %1 не найдена").arg(id));
        Logger::instance().log("MainWindow", QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    }
}

// on_back_to_view_pharmacy_pushButton_2_clicked: возврат к списку аптек
void MainWindow::on_back_to_view_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    loadPharmacysToTable();
}

// on_remove_pharmacy_pushButton_clicked: переход к удалению аптеки
void MainWindow::on_remove_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
}

// on_remove_pharmacy_item_pushButton_2_clicked: удаление лекарства
void MainWindow::on_remove_pharmacy_item_pushButton_2_clicked() {
    QString title_rm_medicine = ui->name_pharmacy_item_lineEdit->text().trimmed();
    if (jsonManager.removeMedicine(title_rm_medicine)) {
        ui->name_pharmacy_item_lineEdit->clear();
        loadMedicinesToTable();
        loadBuyesToTable();
        loadMedicinesToComboBox();
        loadSellerToTable();
        loadSellerToComboBox();
        QMessageBox::information(this, "Успех", "Лекарство успешно удалено");
        Logger::instance().log("MainWindow", QString("Удалено лекарство %1").arg(title_rm_medicine));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Лекарство %1 не найдено").arg(title_rm_medicine));
        Logger::instance().log("MainWindow", QString("Ошибка: Лекарство %1 не найдено").arg(title_rm_medicine));
    }
}

// on_Buy_pharmacy_item_pushButton_clicked: переход к покупке лекарства
void MainWindow::on_Buy_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
}

// on_buy_pushButton_clicked: выполнение покупки лекарства
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

    int pharmacyId = ui->Id_pharmacy_lineEdit->text().toInt();
    if (pharmacyId <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный ID аптеки");
        return;
    }

    QList<Pharmacy> pharmacies = jsonManager.searchPharmacy(pharmacyId, "");
    if (pharmacies.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", QString("Аптека с ID %1 не найдена").arg(pharmacyId));
        return;
    }

    if (jsonManager.makePurchase(name_medicine, quantity, pharmacyId, currentUserLogin)) {
        loadMedicinesToTable();
        loadBuyesToTable();
        loadMedicinesToComboBox();
        loadSellerToTable();
        loadSellerToComboBox();
        ui->quantity_spinBox->setValue(1);
        ui->Id_pharmacy_lineEdit->clear();
        QMessageBox::information(this, "Успех", "Покупка успешно завершена");
        Logger::instance().log("MainWindow", QString("Покупка: %1 купил %2 (%3 единиц) в аптеке %4")
                                                 .arg(currentUserLogin).arg(name_medicine).arg(quantity).arg(pharmacyId));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось выполнить покупку");
        Logger::instance().log("MainWindow", QString("Ошибка покупки: %1, %2, %3 единиц, аптека %4")
                                                 .arg(currentUserLogin).arg(name_medicine).arg(quantity).arg(pharmacyId));
    }
}

// on_back_to_view_buys_menu_pushButton_clicked: возврат к меню покупок
void MainWindow::on_back_to_view_buys_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
    loadBuyesToTable();
}

// on_buy_pharmacy_item_pushButton_clicked: переход к форме покупки
void MainWindow::on_buy_pharmacy_item_pushButton_clicked() {
    ui->quantity_spinBox->setValue(0);
    ui->stackedWidget->setCurrentIndex(14);
}

// on_Cancel_seller_menu_clicked: выход из меню продавца
void MainWindow::on_Cancel_seller_menu_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// on_back_to_seller_menu_pushButton_clicked: возврат к меню продавца
void MainWindow::on_back_to_seller_menu_pushButton_clicked() {
    loadSellerToTable();
    ui->stackedWidget->setCurrentIndex(13);
}

// on_Sell_button_clicked: переход к форме продажи
void MainWindow::on_Sell_button_clicked() {
    loadSellerToComboBox();
    ui->stackedWidget->setCurrentIndex(16);
}

// on_sell_pushButton_clicked: выполнение продажи лекарства
void MainWindow::on_sell_pushButton_clicked() {
    QString name_medicine = ui->sell_comboBox->currentText().trimmed();
    if (name_medicine.isEmpty() || name_medicine == "Нет доступных медикаментов") {
        QMessageBox::warning(this, "Ошибка", "Выберите медикамент для продажи");
        return;
    }

    int quantity = ui->sell_quantity_spinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть положительным");
        return;
    }

    int pharmacyId = ui->Id_pharmacy_lineEdit->text().toInt();
    if (pharmacyId <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный ID аптеки");
        return;
    }

    QList<Pharmacy> pharmacies = jsonManager.searchPharmacy(pharmacyId, "");
    if (pharmacies.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", QString("Аптека с ID %1 не найдена").arg(pharmacyId));
        return;
    }

    if (jsonManager.makePurchase(name_medicine, quantity, pharmacyId, "")) {
        loadMedicinesToTable();
        loadBuyesToTable();
        loadMedicinesToComboBox();
        loadSellerToTable();
        loadSellerToComboBox();
        ui->sell_quantity_spinBox->setValue(1);
        ui->Id_pharmacy_lineEdit->clear();
        QMessageBox::information(this, "Успех", "Продажа успешно завершена");
        Logger::instance().log("MainWindow", QString("Продажа: %1 (%2 единиц) в аптеке %3")
                                                 .arg(name_medicine).arg(quantity).arg(pharmacyId));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось выполнить продажу");
        Logger::instance().log("MainWindow", QString("Ошибка продажи: %1, %2 единиц, аптека %3")
                                                 .arg(name_medicine).arg(quantity).arg(pharmacyId));
    }
}

// on_back_to_login_clicked: выход на экран авторизации
void MainWindow::on_back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// initializeRoleComboBox: инициализация списка ролей для сотрудника
void MainWindow::initializeRoleComboBox() {
    ui->role_comboBox_employee->clear();
    ui->role_comboBox_employee->addItems({"Продавец", "Курьер"});
    ui->role_comboBox_employee->setEnabled(true);
    Logger::instance().log("MainWindow", "Инициализирован role_comboBox_employee");
}

// on_role_comboBox_employee_currentIndexChanged: обработка изменения роли сотрудника
void MainWindow::on_role_comboBox_employee_currentIndexChanged(const QString& role) {
    bool isSeller = (role == "Продавец");
    ui->pharmacyID_comboBox->setVisible(isSeller);
    ui->pharmacyID_label->setVisible(isSeller); // Скрываем/показываем метку
    ui->add_employee_pushButton->setEnabled(isSeller ? !jsonManager.getPharmacy().isEmpty() : true);
    Logger::instance().log("MainWindow", QString("Выбрана роль: %1").arg(role));
}

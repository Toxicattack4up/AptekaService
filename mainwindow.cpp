#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Pharmacy.h"
#include "PharmacyItem.h"
#include "JsonManager.h"
#include "UserRoleHelper.h"
#include "User.h"
#include "historymanager.h"
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
    loadMedicinesToComboBox();
    loadWarehouseToTable();

    // Настройка QSpinBox
    ui->quantity_spinBox->setMinimum(1);
    ui->quantity_spinBox->setMaximum(1000);
    ui->quantity_spinBox->setValue(1);
    ui->transfer_quantity_spinBox->setMinimum(1);
    ui->transfer_quantity_spinBox->setMaximum(1000);
    ui->transfer_quantity_spinBox->setValue(1);

    // Подключение сигнала для обновления таблиц при переключении страниц
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        if (index == 1 || index == 2) { // Страница администратора или сотрудников
            loadEmployeesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица сотрудников");
        } else if (index == 6) { // Страница аптек
            loadPharmacysToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица аптек");
        } else if (index == 9) { // Страница лекарств
            loadMedicinesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица лекарств");
        } else if (index == 13) { // Страница продажи
            loadMedicinesToComboBox();
            Logger::instance().log("MainWindow", "Обновлен ComboBox продажи");
        } else if (index == 15) { // Страница курьера
            loadCourierToTable();
            loadPharmaciesToCourierTable();
            Logger::instance().log("MainWindow", "Обновлены таблицы курьера");
        }
    });
}

// Деструктор: освобождение ресурсов
MainWindow::~MainWindow() {
    delete ui;
}

// загрузка списка сотрудников в таблицу
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

// загрузка списка аптек в таблицу
void MainWindow::loadPharmacysToTable() {
    QTableWidget *table = ui->pharmacy_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Адрес", "Размер", "Текущий запас"});

    const QList<Pharmacy> &pharmacies = jsonManager.getPharmacy();
    table->setRowCount(pharmacies.size());

    if (pharmacies.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет аптек"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        table->resizeColumnsToContents();
        Logger::instance().log("MainWindow", "Таблица аптек: нет аптек");
        return;
    }

    for (int row = 0; row < pharmacies.size(); ++row) {
        const Pharmacy &pharmacy = pharmacies[row];
        table->setItem(row, 0, new QTableWidgetItem(pharmacy.getAddress()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(pharmacy.getSize(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(jsonManager.getPharmacyStock(pharmacy.getId()))));

        for (int col = 0; col < 3; ++col) {
            if (table->item(row, col)) {
                table->item(row, col)->setFlags(table->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->viewport()->update();
    Logger::instance().log("MainWindow", QString("Таблица аптек загружена: %1 записей").arg(pharmacies.size()));
}

// загрузка списка аптек для курьера
void MainWindow::loadPharmaciesToCourierTable() {
    QTableWidget *table = ui->pharmacy_view_item_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Адрес", "Товаров", "Вместимость"});

    const QList<Pharmacy> &pharmacies = jsonManager.getPharmacy();
    table->setRowCount(pharmacies.size());

    if (pharmacies.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет аптек"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        table->resizeColumnsToContents();
        ui->pharmacy_select_comboBox->clear();
        ui->pharmacy_select_comboBox->addItem("Нет доступных аптек");
        ui->transfer_button->setEnabled(false);
        Logger::instance().log("MainWindow", "Таблица аптек курьера: нет аптек");
        return;
    }

    // Заполнение ComboBox аптеками
    ui->pharmacy_select_comboBox->clear();
    for (const Pharmacy &pharmacy : pharmacies) {
        ui->pharmacy_select_comboBox->addItem(QString::number(pharmacy.getId()));
    }
    ui->transfer_button->setEnabled(true);

    for (int row = 0; row < pharmacies.size(); ++row) {
        const Pharmacy &pharmacy = pharmacies[row];
        int currentItems = jsonManager.getPharmacyStock(pharmacy.getId());
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pharmacy.getId())));
        table->setItem(row, 1, new QTableWidgetItem(pharmacy.getAddress()));
        table->setItem(row, 2, new QTableWidgetItem(currentItems == 0 ? "Аптека пуста, добавьте лекарства" : QString::number(currentItems)));
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
    Logger::instance().log("MainWindow", QString("Таблица аптек курьера загружена: %1 записей").arg(pharmacies.size()));
}

// загрузка списка лекарств в таблицу (для администратора)
void MainWindow::loadMedicinesToTable() {
    QTableWidget *table = ui->items_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager.getWarehouseItems();
    table->setRowCount(items.size());

    if (items.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет лекарств"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        table->resizeColumnsToContents();
        Logger::instance().log("MainWindow", "Таблица лекарств: нет лекарств");
        return;
    }

    for (int row = 0; row < items.size(); ++row) {
        const PharmacyItem &item = items[row];
        table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));
        table->setItem(row, 4, new QTableWidgetItem(item.getExpirationDate().toString("yyyy-MM-dd")));

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
    table->viewport()->update();
    Logger::instance().log("MainWindow", QString("Таблица лекарств загружена: %1 записей").arg(items.size()));
}

// заполнение ComboBox лекарств для покупки/продажи
void MainWindow::loadMedicinesToComboBox() {
    ui->buy_comboBox->clear();
    int pharmacyId = currentUser.getPharmacyId();

    // Проверка, если пользователь не связан с аптекой
    if (pharmacyId <= 0 && currentUserRole != UserRoleHelper::fromString("Покупатель")) {
        ui->buy_comboBox->addItem("Нет доступных медикаментов");
        ui->buy_comboBox->setEnabled(false);
        Logger::instance().log("MainWindow", "ComboBox лекарств: пользователь не связан с аптекой");
        return;
    }

    const QList<PharmacyItem> &medicines = jsonManager.getMedicine();
    bool hasItems = false;
    for (const PharmacyItem &item : medicines) {
        if (item.getPharmacyId() == pharmacyId) {
            ui->buy_comboBox->addItem(item.getTitle());
            hasItems = true;
        }
    }

    if (!hasItems) {
        ui->buy_comboBox->addItem("Нет доступных медикаментов");
        ui->buy_comboBox->setEnabled(false);
        Logger::instance().log("MainWindow", QString("ComboBox лекарств: нет медикаментов для аптеки ID %1").arg(pharmacyId));
    } else {
        ui->buy_comboBox->setEnabled(true);
        Logger::instance().log("MainWindow", QString("ComboBox лекарств загружен: медикаменты для аптеки ID %1").arg(pharmacyId));
    }
}

// загрузка таблицы общего склада
void MainWindow::loadWarehouseToTable() {
    QTableWidget *table = ui->view_warehouse_item;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager.getWarehouseItems();
    table->setRowCount(items.size());

    if (items.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет лекарств"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        table->resizeColumnsToContents();
        Logger::instance().log("MainWindow", "Таблица склада: нет лекарств");
        return;
    }

    for (int row = 0; row < items.size(); ++row) {
        const PharmacyItem &item = items[row];
        table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));
        table->setItem(row, 4, new QTableWidgetItem(item.getExpirationDate().toString("yyyy-MM-dd")));

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
    table->viewport()->update();
    Logger::instance().log("MainWindow", QString("Таблица склада загружена: %1 записей").arg(items.size()));
}

// загрузка таблицы для курьера
void MainWindow::loadCourierToTable() {
    QTableWidget *table = ui->view_warehouse_item;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager.getWarehouseItems();
    table->setRowCount(items.size());

    if (items.isEmpty()) {
        table->setRowCount(1);
        table->setItem(0, 0, new QTableWidgetItem("Нет лекарств"));
        table->item(0, 0)->setFlags(table->item(0, 0)->flags() & ~Qt::ItemIsEditable);
        table->resizeColumnsToContents();
        Logger::instance().log("MainWindow", "Таблица курьера: нет лекарств");
        return;
    }

    for (int row = 0; row < items.size(); ++row) {
        const PharmacyItem &item = items[row];
        table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(item.getQuantity())));
        table->setItem(row, 3, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));
        table->setItem(row, 4, new QTableWidgetItem(item.getExpirationDate().toString("yyyy-MM-dd")));

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
    table->viewport()->update();
    Logger::instance().log("MainWindow", QString("Таблица курьера загружена: %1 записей").arg(items.size()));
}

// авторизация пользователя
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
    // Устанавливаем currentUser
    for (const User& user : jsonManager.getEmployee()) {
        if (user.getLogin() == login) {
            currentUser = user;
            break;
        }
    }
    QMessageBox::information(this, "Успех", "Вы успешно вошли!");
    Logger::instance().log("MainWindow", QString("Пользователь %1 (%2) авторизован").arg(login).arg(role));

    if (role == "Администратор") {
        loadEmployeesToTable();
        ui->stackedWidget->setCurrentIndex(1);
    } else if (role == "Продавец") {
        ui->stackedWidget->setCurrentIndex(15);
    } else if (role == "Покупатель") {
        ui->stackedWidget->setCurrentIndex(5);
    } else if (role == "Курьер") {
        loadCourierToTable();
        ui->stackedWidget->setCurrentIndex(12);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
        Logger::instance().log("MainWindow", QString("Ошибка: неизвестная роль %1 для %2").arg(role).arg(login));
    }

    ui->lineEdit_login->clear();
    ui->lineEdit_pass->clear();
}

// переход к форме регистрации
void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(17);
}

// регистрация нового покупателя
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
    HistoryManager::instance().addOperation(login, "Регистрация", "Зарегистрирован как Покупатель", 0);
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
    Logger::instance().log("MainWindow", QString("Зарегистрирован покупатель %1").arg(login));

    ui->reglog_lineEdit->clear();
    ui->regpass_lineEdit->clear();
    ui->FIO_lineEdit->clear();
    ui->email_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// возврат к главному меню
void MainWindow::on_cancelToMain_Button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// возврат к списку сотрудников
void MainWindow::on_back_to_view_employee_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    ui->pharmacyID_comboBox->setVisible(false);
    ui->pharmacyID_label->setVisible(false);
    initializeRoleComboBox();
    loadEmployeesToTable();
    Logger::instance().log("MainWindow", "Возврат к списку сотрудников");
}

// возврат к меню покупателя
void MainWindow::on_back_to_view_buys_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(12);
}

// переход к списку лекарств
void MainWindow::on_back_to_view_pharmacy_pushButton_clicked() {
    loadMedicinesToTable();
    ui->stackedWidget->setCurrentIndex(11);
}

// возврат к списку аптек
void MainWindow::on_back_to_view_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// выход на экран авторизации
void MainWindow::on_Back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// возврат к главному меню
void MainWindow::on_back_to_menu_login_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// переход к списку сотрудников
void MainWindow::on_back_to_view_employees_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// возврат к списку лекарств
void MainWindow::on_back_to_menu_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(11);
}

// возврат к меню администратора
void MainWindow::on_back_menu_admin_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// возврат к меню администратора
void MainWindow::on_back_to_admin_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadEmployeesToTable();
}

// переход к управлению сотрудниками
void MainWindow::on_Admin_users_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// открытие формы добавления сотрудника
void MainWindow::on_add_employees_pushButton_clicked() {
    initializeRoleComboBox();
    ui->stackedWidget->setCurrentIndex(3);
    ui->pharmacyID_comboBox->setVisible(ui->role_comboBox_employee->currentText() == "Продавец");
    ui->pharmacyID_label->setVisible(ui->role_comboBox_employee->currentText() == "Продавец");

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

// переход к удалению сотрудника
void MainWindow::on_delete_employees_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
}

// переход к добавлению лекарства
void MainWindow::on_add_pharmac_item_pushButton_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->item_name_lineEdit->clear();
    ui->item_price_lineEdit->clear();
    ui->item_quantity_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(13);
}

// переход к удалению лекарства
void MainWindow::on_remove_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(14);
}

// переход к добавлению аптеки
void MainWindow::on_add_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    ui->Id_pharmacy_lineEdit->clear();
    ui->adress_lineEdit->clear();
    ui->square_lineEdit->clear();
    ui->countItems_lineEdit->clear();
}

// переход к управлению аптеками
void MainWindow::on_Admin_pharmacy_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// переход к управлению лекарствами
void MainWindow::on_Admin_Item_clicked() {
    ui->stackedWidget->setCurrentIndex(11);
    loadMedicinesToTable();
}

// добавление нового лекарства
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
        Logger::instance().log("MainWindow", QString("Ошибка добавления лекарства: некорректная дата срока годности"));
        return;
    }
    if (expirationDate < QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Срок годности не может быть в прошлом");
        Logger::instance().log("MainWindow", QString("Ошибка добавления лекарства: срок годности в прошлом"));
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
    loadWarehouseToTable();
    loadMedicinesToComboBox();
    loadPharmacysToTable();
    Logger::instance().log("MainWindow", QString("Добавлено лекарство %1 на склад").arg(name_pharmacy));
    HistoryManager::instance().addOperation(currentUserLogin, "Добавление лекарства",
                                            QString("Добавлено %1 (%2 единиц) на склад").arg(name_pharmacy).arg(quality_pharmacy), 0);

    ui->stackedWidget->setCurrentIndex(9);
}

// добавление новой аптеки
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

// добавление нового сотрудника
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

    jsonManager.addEmployee(role, login, password, fullName, email, pharmacyId);

    HistoryManager::instance().addOperation(currentUserLogin, "Добавление сотрудника",
                                            QString("Добавлен %1 (%2), аптека ID %3").arg(login).arg(role).arg(pharmacyId), pharmacyId);

    ui->login_lineEdit_employee->clear();
    ui->password_lineEdit_employee->clear();
    ui->FIO_lineEdit_employee->clear();
    ui->email_lineEdit_employee->clear();
    ui->pharmacyID_comboBox->setVisible(false);
    ui->pharmacyID_label->setVisible(false);
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
    Logger::instance().log("MainWindow", QString("Сотрудник %1 добавлен").arg(login));
}

// удаление сотрудника
void MainWindow::on_remove_employees_Button_clicked() {
    QString login = ui->remove_lineEdit->text().trimmed();
    if (jsonManager.removeEmployee(login)) {
        ui->remove_lineEdit->clear();
        HistoryManager::instance().addOperation(currentUserLogin, "Удаление сотрудника",
                                                QString("Удалён сотрудник %1").arg(login), 0);
        QMessageBox::information(this, "Успех", "Сотрудник успешно удалён");
        loadEmployeesToTable();
        Logger::instance().log("MainWindow", QString("Удалён сотрудник %1").arg(login));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Сотрудник %1 не найден").arg(login));
        Logger::instance().log("MainWindow", QString("Ошибка: Сотрудник %1 не найден").arg(login));
    }
}

// возврат к меню администратора
void MainWindow::on_back_to_view_menu_admin_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// удаление аптеки
void MainWindow::on_remove_pharmacy_pushButton_2_clicked() {
    int id = ui->removeID_lineEdit->text().toInt();
    if (jsonManager.removePharmacy(id)) {
        ui->removeID_lineEdit->clear();
        loadPharmacysToTable();
        ui->stackedWidget->setCurrentIndex(6);
        HistoryManager::instance().addOperation(currentUserLogin, "Удаление аптеки",
                                                QString("Удалена аптека ID %1").arg(id), id);
        QMessageBox::information(this, "Успех", "Аптека успешно удалена");
        Logger::instance().log("MainWindow", QString("Удалена аптека ID %1").arg(id));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Аптека ID %1 не найдена").arg(id));
        Logger::instance().log("MainWindow", QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    }
}

// возврат к списку аптек
void MainWindow::on_back_to_view_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// переход к удалению аптеки
void MainWindow::on_remove_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(9);
}

// удаление лекарства
void MainWindow::on_remove_pharmacy_item_pushButton_2_clicked() {
    QString title = ui->name_pharmacy_item_lineEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства для удаления");
        Logger::instance().log("MainWindow", "Ошибка удаления: пустое название");
        return;
    }

    if (jsonManager.removeMedicine(title, 0)) {
        ui->name_pharmacy_item_lineEdit->clear();
        loadMedicinesToTable();
        loadWarehouseToTable();
        loadMedicinesToComboBox();
        loadPharmacysToTable();
        Logger::instance().log("MainWindow", QString("Удалено лекарство %1 со склада").arg(title));
        HistoryManager::instance().addOperation(currentUserLogin, "Удаление лекарства",
                                                QString("Удалено %1 со склада").arg(title), 0);
        QMessageBox::information(this, "Успешно", QString("Лекарство %1 успешно удалено со склада").arg(title));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Лекарство %1 не найдено на складе").arg(title));
        Logger::instance().log("MainWindow", QString("Ошибка удаления: лекарство %1 не найдено").arg(title));
    }
}

// переход к форме покупки
void MainWindow::on_buy_pharmacy_item_pushButton_clicked() {
    ui->quantity_spinBox->setValue(1);
    loadMedicinesToComboBox();
    ui->stackedWidget->setCurrentIndex(13);
}

// выполнение покупки/продажи лекарства
void MainWindow::on_buy_pushButton_clicked() {
    QString name_medicine = ui->buy_comboBox->currentText().trimmed();
    if (name_medicine.isEmpty() || name_medicine == "Нет доступных медикаментов") {
        QMessageBox::warning(this, "Ошибка", "Выберите медикамент для покупки/продажи");
        Logger::instance().log("MainWindow", "Ошибка: медикамент не выбран");
        return;
    }

    int quantity = ui->quantity_spinBox->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть положительным");
        Logger::instance().log("MainWindow", "Ошибка: некорректное количество");
        return;
    }

    int pharmacyId = currentUser.getPharmacyId();
    if (pharmacyId <= 0 && currentUserRole != UserRoleHelper::fromString("Покупатель")) {
        QMessageBox::warning(this, "Ошибка", "У пользователя не указана аптека");
        Logger::instance().log("MainWindow", "Ошибка: у пользователя не указана аптека");
        return;
    }

    QString buyerLogin = (currentUserRole == UserRoleHelper::fromString("Покупатель")) ? currentUserLogin : "";
    if (jsonManager.makePurchase(name_medicine, quantity, pharmacyId, buyerLogin)) {
        loadMedicinesToTable();
        loadMedicinesToComboBox();
        loadPharmacysToTable();
        ui->quantity_spinBox->setValue(1);
        QString action = (currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "Покупка" : "Продажа";
        QMessageBox::information(this, "Успех", QString("%1 успешно завершена").arg(action));
        Logger::instance().log("MainWindow", QString("%1: %2 (%3 единиц) в аптеке %4, пользователь %5")
                                                 .arg(action).arg(name_medicine).arg(quantity).arg(pharmacyId).arg(currentUserLogin));
        HistoryManager::instance().addOperation(currentUserLogin, action,
                                                QString("%1 %2 (%3 единиц) в аптеке %4").arg(action).arg(name_medicine).arg(quantity).arg(pharmacyId), pharmacyId);
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Не удалось выполнить %1").arg((currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "покупку" : "продажу"));
        Logger::instance().log("MainWindow", QString("Ошибка %1: %2, %3 единиц, аптека %4")
                                                 .arg((currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "покупки" : "продажи")
                                                 .arg(name_medicine).arg(quantity).arg(pharmacyId));
    }
}

// переход в меню истории
void MainWindow::on_history_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(20);
}

// отображение истории операций
// void MainWindow::on_history_button_clicked() {
//     int pharmacyId = currentUser.getPharmacyId();
//     if (pharmacyId <= 0 && currentUserRole != UserRoleHelper::fromString("Администратор")) {
//         QMessageBox::warning(this, "Ошибка", "У пользователя не указана аптека");
//         Logger::instance().log("MainWindow", "Ошибка: у пользователя не указана аптека");
//         return;
//     }
//     QString historyText = "История операций:\n";
//     const auto& operations = HistoryManager::instance().getOperations(pharmacyId);
//     if (operations.isEmpty()) {
//         historyText += "Нет записей.\n";
//     } else {
//         for (const auto& op : operations) {
//             historyText += QString("Пользователь: %1, Действие: %2, Детали: %3, Время: %4\n")
//                                .arg(op.userLogin)
//                                .arg(op.action)
//                                .arg(op.details)
//                                .arg(op.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
//         }
//     }
//     // Статистика продаж по датам
//     historyText += "\nСтатистика продаж по датам:\n";
//     QMap<QDate, int> salesByDate = HistoryManager::instance().getSalesByDate(pharmacyId);
//     if (salesByDate.isEmpty()) {
//         historyText += "Нет продаж.\n";
//     } else {
//         for (auto it = salesByDate.constBegin(); it != salesByDate.constEnd(); ++it) {
//             historyText += QString("Дата: %1, Количество продаж: %2\n")
//                                .arg(it.key().toString("yyyy-MM-dd"))
//                                .arg(it.value());
//         }
//     }
//     QMessageBox::information(this, "История операций", historyText);
//     Logger::instance().log("MainWindow", "Отображена история операций");
// }

// отображение товаров на складе аптеки
void MainWindow::on_pharmacy_stock_button_clicked() {
    int pharmacyId = currentUser.getPharmacyId();
    if (pharmacyId <= 0) {
        QMessageBox::warning(this, "Ошибка", "У пользователя не указана аптека");
        Logger::instance().log("MainWindow", "Ошибка: у пользователя не указана аптека");
        return;
    }
    QString stockText = QString("Товары в аптеке ID %1:\n").arg(pharmacyId);
    const QList<PharmacyItem> &items = jsonManager.getMedicine();
    bool hasItems = false;
    for (const PharmacyItem &item : items) {
        if (item.getPharmacyId() == pharmacyId) {
            stockText += QString("Название: %1, Цена: %2, Количество: %3, Рецепт: %4, Срок годности: %5\n")
                             .arg(item.getTitle())
                             .arg(item.getPrice(), 0, 'f', 2)
                             .arg(item.getQuantity())
                             .arg(item.isRecipeRequired() ? "Да" : "Нет")
                             .arg(item.getExpirationDate().toString("yyyy-MM-dd"));
            hasItems = true;
        }
    }
    if (!hasItems) {
        stockText += "Нет лекарств в аптеке.";
    }
    QMessageBox::information(this, QString("Товары в аптеке ID %1").arg(pharmacyId), stockText);
    Logger::instance().log("MainWindow", QString("Отображены товары в аптеке ID %1").arg(pharmacyId));
}

// переход к форме продажи
void MainWindow::on_sell_button_clicked() {
    ui->quantity_spinBox->setValue(1);
    loadMedicinesToComboBox();
    ui->stackedWidget->setCurrentIndex(16);
}

// выход на экран авторизации
void MainWindow::on_logout_button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    Logger::instance().log("MainWindow", QString("Пользователь %1 вышел из аккаунта").arg(currentUserLogin));
}

// инициализация списка ролей для сотрудника
void MainWindow::initializeRoleComboBox() {
    ui->role_comboBox_employee->clear();
    ui->role_comboBox_employee->addItems({"Продавец", "Курьер"});
    ui->role_comboBox_employee->setEnabled(true);
    Logger::instance().log("MainWindow", "Инициализирован role_comboBox_employee");
}

// обработка изменения роли сотрудника
void MainWindow::on_role_comboBox_employee_currentIndexChanged(const QString& role) {
    bool isSeller = (role == "Продавец");
    ui->pharmacyID_comboBox->setVisible(isSeller);
    ui->pharmacyID_label->setVisible(isSeller);
    ui->add_employee_pushButton->setEnabled(isSeller ? !jsonManager.getPharmacy().isEmpty() : true);
    Logger::instance().log("MainWindow", QString("Выбрана роль: %1").arg(role));
}

// перемещение лекарства в аптеку
void MainWindow::on_transfer_button_clicked() {
    // Проверка, выбрано ли лекарство в таблице
    QTableWidget *warehouseTable = ui->view_warehouse_item;
    QList<QTableWidgetItem *> selectedItems = warehouseTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите лекарство из таблицы склада!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: лекарство не выбрано");
        return;
    }

    // Получение названия лекарства из выбранной строки
    int row = warehouseTable->currentRow();
    QString medicineTitle = warehouseTable->item(row, 0)->text();
    if (medicineTitle == "Нет лекарств") {
        QMessageBox::warning(this, "Ошибка", "Склад пуст, нет лекарств для перемещения!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: склад пуст");
        return;
    }

    // Получение ID аптеки из ComboBox
    QString pharmacyIdStr = ui->pharmacy_select_comboBox->currentText();
    bool ok;
    int pharmacyId = pharmacyIdStr.toInt(&ok);
    if (!ok || pharmacyIdStr == "Нет доступных аптек") {
        QMessageBox::warning(this, "Ошибка", "Выберите действительную аптеку!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: аптека не выбрана");
        return;
    }

    // Получение количества для перемещения
    int transferQuantity = ui->transfer_quantity_spinBox->value();

    // Проверка наличия лекарства на складе
    QList<PharmacyItem> warehouseItems = jsonManager.getWarehouseItems();
    PharmacyItem *selectedItem = nullptr;
    for (PharmacyItem &item : warehouseItems) {
        if (item.getTitle() == medicineTitle) {
            selectedItem = &item;
            break;
        }
    }

    if (!selectedItem || selectedItem->getQuantity() < transferQuantity) {
        QMessageBox::warning(this, "Ошибка", QString("Недостаточно лекарства на складе! Доступно: %1")
                                                       .arg(selectedItem ? selectedItem->getQuantity() : 0));
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: недостаточно лекарства %1 (требуется %2, доступно %3)")
                                                 .arg(medicineTitle).arg(transferQuantity)
                                                 .arg(selectedItem ? selectedItem->getQuantity() : 0));
        return;
    }

    // Проверка вместимости аптеки
    const QList<Pharmacy> &pharmacies = jsonManager.getPharmacy();
    Pharmacy selectedPharmacy;
    bool pharmacyFound = false;
    for (const Pharmacy &pharmacy : pharmacies) {
        if (pharmacy.getId() == pharmacyId) {
            selectedPharmacy = pharmacy;
            pharmacyFound = true;
            break;
        }
    }

    if (!pharmacyFound) {
        QMessageBox::warning(this, "Ошибка", "Аптека не найдена!");
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: аптека ID %1 не найдена").arg(pharmacyId));
        return;
    }

    int currentItems = jsonManager.getPharmacyStock(pharmacyId);
    int maxCapacity = selectedPharmacy.getMaxCapacity();
    if (currentItems + transferQuantity > maxCapacity) {
        QMessageBox::warning(this, "Ошибка", QString("Аптека не может вместить столько лекарств! Осталось места: %1")
                                                       .arg(maxCapacity - currentItems));
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: аптека ID %1 переполнена (требуется %2, доступно %3)")
                                                 .arg(pharmacyId).arg(transferQuantity).arg(maxCapacity - currentItems));
        return;
    }

    // Выполнение перемещения
    bool success = jsonManager.moveMedicineToPharmacy(medicineTitle, transferQuantity, pharmacyId);
    if (!success) {
        QMessageBox::warning(this, "Ошибка", "Не удалось переместить лекарство! Проверьте наличие на складе или существование аптеки.");
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: не удалось переместить %1 единиц %2 в аптеку ID %3")
                                                 .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
        return;
    }

    // Обновление таблиц
    loadCourierToTable();
    loadPharmaciesToCourierTable();

    HistoryManager::instance().addOperation(currentUserLogin, "Перемещение лекарства",
                                            QString("Перемещено %1 единиц '%2' в аптеку ID %3").arg(transferQuantity).arg(medicineTitle).arg(pharmacyId), pharmacyId);

    QMessageBox::information(this, "Успех", QString("Перемещено %1 единиц '%2' в аптеку ID %3")
                                                     .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
    Logger::instance().log("MainWindow", QString("Успешное перемещение: %1 единиц '%2' в аптеку ID %3")
                                             .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
}

// возвращение в главное меню из меню курьера
void MainWindow::on_back_to_login_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// выход из меню рюкзака в главное меню покупателя
void MainWindow::on_back_to_buyer_menu_from_backpack_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// меню пополнения баланса
void MainWindow::on_deposit_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(18);
}

// Меню покупки (выбора аптеки)
void MainWindow::on_buy_medicine_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

// переход в меню рюкзака
void MainWindow::on_backpack_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(19);
}

// выход из меню пополнения в главное меню покупателя
void MainWindow::on_back_to_buyer_menu_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// выход из меню истории в главное меню покупателя
void MainWindow::on_back_to_buyer_menu_from_history_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// переход в меню покупки из меню выбора аптеки в которой покупаем
void MainWindow::on_select_pharmacy_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);

    // Добавить логику обновления таблица лекарств определенной аптеки по ее id
}

// выход из меню выбора аптеки в главное меню покупателя
void MainWindow::on_back_to_buyer_menu_from_pharmacy_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

// выход из меню выбора лекарства для покупки в меню выбора аптеки
void MainWindow::on_back_to_pharmacy_selection_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

// кнопка совершения покупки покуптелем
void MainWindow::on_add_to_backpack_button_clicked()
{
    // Добавить логику покупки, чтобы когда пользователь делает покупку в листах аптеки уменьшалось количество
    // а в случае когда лекарство закончится, то обновить таблицу
}


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

// Конструктор главного окна: инициализирует интерфейс и подключает сигналы
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), jsonManager(new JsonManager()), warehouse()
{
    ui->setupUi(this);
    setWindowTitle("Аптечная система");
    Logger::instance().log("MainWindow", "Приложение запущено");

    ui->stackedWidget->setCurrentIndex(0);
    initializeRoleComboBox();
    connect(ui->role_comboBox_employee, &QComboBox::currentTextChanged,
            this, &MainWindow::on_role_comboBox_employee_currentIndexChanged);
    loadEmployeesToTable();
    loadPharmacysToTable();
    loadMedicinesToTable();
    loadMedicinesToComboBox();
    loadWarehouseToTable();

    ui->quantity_spinBox->setMinimum(1);
    ui->quantity_spinBox->setMaximum(1000);
    ui->quantity_spinBox->setValue(1);
    ui->transfer_quantity_spinBox->setMinimum(1);
    ui->transfer_quantity_spinBox->setMaximum(1000);
    ui->transfer_quantity_spinBox->setValue(1);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        if (index == 1 || index == 2) {
            loadEmployeesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица сотрудников");
        } else if (index == 6) {
            loadPharmaciesToTableForBuyer();
            Logger::instance().log("MainWindow", "Обновлена таблица аптек для покупателя");
        } else if (index == 7) {
            loadMedicinesToTableForBuyer(currentPharmacyId);
            Logger::instance().log("MainWindow", "Обновлена таблица лекарств для аптеки ID " + QString::number(currentPharmacyId));
        } else if (index == 9) {
            loadMedicinesToTable();
            Logger::instance().log("MainWindow", "Обновлена таблица лекарств");
        } else if (index == 13) {
            loadMedicinesToComboBox();
            Logger::instance().log("MainWindow", "Обновлен ComboBox продажи");
        } else if (index == 15) {
            loadCourierToTable();
            loadPharmaciesToCourierTable();
            Logger::instance().log("MainWindow", "Обновлены таблицы курьера");
        } else if (index == 19) {
            loadBackpack();
            Logger::instance().log("MainWindow", "Обновлен рюкзак покупателя");
        } else if (index == 20) {
            loadPurchaseHistory();
            Logger::instance().log("MainWindow", "Обновлена история покупок");
        }
    });
}

// Деструктор: сохраняет данные и освобождает ресурсы
MainWindow::~MainWindow() {
    jsonManager->saveAllToJson();
    delete jsonManager;
    delete ui;
}

// Загрузка сотрудников в таблицу: отображает список сотрудников в интерфейсе
void MainWindow::loadEmployeesToTable() {
    QTableWidget *table = ui->table_employees;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Логин", "ФИО", "Email", "Дата регистрации", "Роль"});

    const QList<User> &employees = jsonManager->getEmployee();
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

// Загрузка аптек в таблицу: отображает список аптек для администратора
void MainWindow::loadPharmacysToTable() {
    QTableWidget *table = ui->pharmacy_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Адрес", "Размер", "Текущий запас"});

    const QList<Pharmacy> &pharmacies = jsonManager->getPharmacy();
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
        table->setItem(row, 2, new QTableWidgetItem(QString::number(jsonManager->getPharmacyStock(pharmacy.getId()))));

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

// Загрузка аптек для покупателя: отображает список аптек для выбора
void MainWindow::loadPharmaciesToTableForBuyer() {
    QTableWidget *table = ui->pharmacy_selection_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"ID", "Адрес"});

    const QList<Pharmacy> &pharmacies = jsonManager->getPharmacy();
    table->setRowCount(pharmacies.size());

    for (int row = 0; row < pharmacies.size(); ++row) {
        const Pharmacy &pharmacy = pharmacies[row];
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pharmacy.getId())));
        table->setItem(row, 1, new QTableWidgetItem(pharmacy.getAddress()));
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Logger::instance().log("MainWindow", "Таблица аптек для покупателя загружена");
}

// Загрузка лекарств для покупателя: отображает доступные лекарства в выбранной аптеке
void MainWindow::loadMedicinesToTableForBuyer(int pharmacyId) {
    QTableWidget *table = ui->medicine_selection_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Рецепт"});

    const QList<PharmacyItem> &medicines = jsonManager->getMedicine();
    for (const PharmacyItem &item : medicines) {
        if (item.getPharmacyId() == pharmacyId && item.getQuantity() > 0) {
            int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(item.getTitle()));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(item.getPrice(), 'f', 2)));
            table->setItem(row, 2, new QTableWidgetItem(item.isRecipeRequired() ? "Да" : "Нет"));
        }
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Logger::instance().log("MainWindow", "Таблица лекарств для аптеки ID " + QString::number(pharmacyId) + " загружена");
}

// Загрузка лекарств в таблицу: отображает список лекарств для администратора
void MainWindow::loadMedicinesToTable() {
    QTableWidget *table = ui->items_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager->getWarehouseItems();
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

// Загрузка лекарств в ComboBox: заполняет выпадающий список доступными лекарствами
void MainWindow::loadMedicinesToComboBox() {
    ui->buy_comboBox->clear();
    int pharmacyId = currentUser.getPharmacyId();

    if (pharmacyId <= 0 && currentUserRole != UserRoleHelper::fromString("Покупатель")) {
        ui->buy_comboBox->addItem("Нет доступных медикаментов");
        ui->buy_comboBox->setEnabled(false);
        Logger::instance().log("MainWindow", "ComboBox лекарств: пользователь не связан с аптекой");
        return;
    }

    const QList<PharmacyItem> &medicines = jsonManager->getMedicine();
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

// Загрузка склада в таблицу: отображает содержимое центрального склада
void MainWindow::loadWarehouseToTable() {
    QTableWidget *table = ui->view_warehouse_item;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager->getWarehouseItems();
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

// Загрузка таблицы для курьера: отображает содержимое склада для курьера
void MainWindow::loadCourierToTable() {
    QTableWidget *table = ui->view_warehouse_item;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Название", "Цена", "Количество", "Рецепт", "Срок годности"});

    const QList<PharmacyItem> &items = jsonManager->getWarehouseItems();
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

// Загрузка аптек для курьера: отображает список аптек для курьера
void MainWindow::loadPharmaciesToCourierTable() {
    QTableWidget *table = ui->pharmacy_view_item_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"ID", "Адрес", "Товаров", "Вместимость"});

    const QList<Pharmacy> &pharmacies = jsonManager->getPharmacy();
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

    ui->pharmacy_select_comboBox->clear();
    for (const Pharmacy &pharmacy : pharmacies) {
        ui->pharmacy_select_comboBox->addItem(QString::number(pharmacy.getId()));
    }
    ui->transfer_button->setEnabled(true);

    for (int row = 0; row < pharmacies.size(); ++row) {
        const Pharmacy &pharmacy = pharmacies[row];
        int currentItems = jsonManager->getPharmacyStock(pharmacy.getId());
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pharmacy.getId())));
        table->setItem(row, 1, new QTableWidgetItem(pharmacy.getAddress()));
        table->setItem(row, 2, new QTableWidgetItem(currentItems == 0 ? "Аптека пуста" : QString::number(currentItems)));
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

// Загрузка истории покупок: отображает историю покупок пользователя
void MainWindow::loadPurchaseHistory() {
    QTableWidget *table = ui->history_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Дата", "Лекарство", "Сумма"});

    const QList<JsonManager::Purchase> &purchases = jsonManager->getPurchaseHistory(currentUserLogin);
    for (const JsonManager::Purchase &purchase : purchases) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(purchase.timestamp.toString("yyyy-MM-dd HH:mm:ss")));
        table->setItem(row, 1, new QTableWidgetItem(purchase.medicineTitle));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(purchase.amount, 'f', 2)));
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    Logger::instance().log("MainWindow", "История покупок загружена");
}

// Загрузка рюкзака: отображает содержимое рюкзака покупателя
void MainWindow::loadBackpack() {
    QTableWidget *table = ui->backpack_table;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(1);
    table->setHorizontalHeaderLabels({"Название"});

    const QStringList &backpack = jsonManager->getBackpack(currentUserLogin);
    for (const QString &medicine : backpack) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(medicine));
    }

    table->resizeColumnsToContents();
    table->horizontalHeader()->setStretchLastSection(true);
    Logger::instance().log("MainWindow", "Рюкзак загружен");
}

// Обработка входа: выполняет авторизацию пользователя
void MainWindow::on_login_button_clicked() {
    QString login = ui->lineEdit_login->text().trimmed();
    QString pass = ui->lineEdit_pass->text().trimmed();

    QString role = jsonManager->validateUser(login, pass);
    if (role.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!");
        Logger::instance().log("MainWindow", QString("Ошибка авторизации: неверный логин %1").arg(login));
        return;
    }
    currentUserLogin = login;
    currentUserRole = UserRoleHelper::fromString(role);
    for (const User& user : jsonManager->getEmployee()) {
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
        double balance = jsonManager->getBuyerBalance(login);
        ui->money_lcd->display(balance);
        ui->stackedWidget->setCurrentIndex(5);
    } else if (role == "Курьер") {
        loadCourierToTable();
        loadPharmaciesToCourierTable();
        ui->stackedWidget->setCurrentIndex(12);
    } else {
        QMessageBox::warning(this, "Ошибка", "Неизвестная роль");
        Logger::instance().log("MainWindow", QString("Ошибка: неизвестная роль %1 для %2").arg(role).arg(login));
    }

    ui->lineEdit_login->clear();
    ui->lineEdit_pass->clear();
}

// Переход к регистрации: открывает форму регистрации
void MainWindow::on_registr_button_clicked() {
    ui->stackedWidget->setCurrentIndex(17);
}

// Регистрация покупателя: добавляет нового покупателя в систему
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

    jsonManager->addEmployee("Покупатель", login, password, fullName, email, 0);
    HistoryManager::instance().addOperation(login, "Регистрация", "Зарегистрирован как Покупатель", 0);
    QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
    Logger::instance().log("MainWindow", QString("Зарегистрирован покупатель %1").arg(login));

    ui->reglog_lineEdit->clear();
    ui->regpass_lineEdit->clear();
    ui->FIO_lineEdit->clear();
    ui->email_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

// Возврат к главному меню: переключает интерфейс на начальную страницу
void MainWindow::on_cancelToMain_Button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// Возврат к списку сотрудников: переключает интерфейс на список сотрудников
void MainWindow::on_back_to_view_employee_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    ui->pharmacyID_comboBox->setVisible(false);
    ui->pharmacyID_label->setVisible(false);
    initializeRoleComboBox();
    loadEmployeesToTable();
    Logger::instance().log("MainWindow", "Возврат к списку сотрудников");
}

// Возврат к меню покупателя: переключает интерфейс на меню продавца
void MainWindow::on_back_to_view_buys_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(15);
}

// Переход к списку лекарств: переключает интерфейс на список лекарств
void MainWindow::on_back_to_view_pharmacy_pushButton_clicked() {
    loadMedicinesToTable();
    ui->stackedWidget->setCurrentIndex(11);
}

// Возврат к списку аптек: переключает интерфейс на список аптек
void MainWindow::on_back_to_view_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// Выход на экран авторизации: переключает интерфейс на страницу входа
void MainWindow::on_Back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// Возврат к главному меню: переключает интерфейс на начальную страницу
void MainWindow::on_back_to_menu_login_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// Переход к списку сотрудников: переключает интерфейс на список сотрудников
void MainWindow::on_back_to_view_employees_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// Возврат к списку лекарств: переключает интерфейс на список лекарств
void MainWindow::on_back_to_menu_items_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(11);
}

// Возврат к меню администратора: переключает интерфейс на меню администратора
void MainWindow::on_back_menu_admin_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// Возврат к меню администратора: переключает интерфейс на меню администратора
void MainWindow::on_back_to_admin_menu_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    loadEmployeesToTable();
}

// Переход к управлению сотрудниками: открывает интерфейс управления сотрудниками
void MainWindow::on_Admin_users_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadEmployeesToTable();
}

// Открытие формы добавления сотрудника: показывает форму для добавления сотрудника
void MainWindow::on_add_employees_pushButton_clicked() {
    initializeRoleComboBox();
    ui->stackedWidget->setCurrentIndex(3);
    ui->pharmacyID_comboBox->setVisible(ui->role_comboBox_employee->currentText() == "Продавец");
    ui->pharmacyID_label->setVisible(ui->role_comboBox_employee->currentText() == "Продавец");

    ui->pharmacyID_comboBox->clear();
    QList<Pharmacy> pharmacies = jsonManager->getPharmacy();
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

// Переход к удалению сотрудника: открывает форму удаления сотрудника
void MainWindow::on_delete_employees_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
}

// Переход к добавлению лекарства: открывает форму добавления лекарства
void MainWindow::on_add_pharmac_item_pushButton_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
    ui->item_name_lineEdit->clear();
    ui->item_price_lineEdit->clear();
    ui->item_quantity_lineEdit->clear();
    ui->stackedWidget->setCurrentIndex(13);
}

// Переход к удалению лекарства: открывает форму удаления лекарства
void MainWindow::on_remove_pharmacy_item_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(14);
}

// Переход к добавлению аптеки: открывает форму добавления аптеки
void MainWindow::on_add_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(10);
    ui->Id_pharmacy_lineEdit->clear();
    ui->adress_lineEdit->clear();
    ui->square_lineEdit->clear();
    ui->countItems_lineEdit->clear();
}

// Переход к управлению аптеками: открывает интерфейс управления аптеками
void MainWindow::on_Admin_pharmacy_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// Переход к управлению лекарствами: открывает интерфейс управления лекарствами
void MainWindow::on_Admin_Item_clicked() {
    ui->stackedWidget->setCurrentIndex(11);
    loadMedicinesToTable();
}

// Добавление нового лекарства: добавляет лекарство на склад
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

    // Проверка на дублирование лекарства
    for (const PharmacyItem& item : jsonManager->getWarehouseItems()) {
        if (item.getTitle() == name_pharmacy) {
            QMessageBox::warning(this, "Ошибка", QString("Лекарство '%1' уже существует на складе").arg(name_pharmacy));
            Logger::instance().log("MainWindow", QString("Ошибка добавления: лекарство '%1' уже существует").arg(name_pharmacy));
            return;
        }
    }

    PharmacyItem item(name_pharmacy, price_pharmacy, recipe, expirationDate, quality_pharmacy);
    item.setPharmacyId(0); // Устанавливаем pharmacyId = 0 для склада (если это не делается в addMedicine)
    jsonManager->addMedicine(item);
    Logger::instance().log("MainWindow", QString("Лекарство '%1' добавлено в jsonManager").arg(name_pharmacy));

    // Проверка содержимого перед сохранением
    QString itemsLog = "Текущие лекарства на складе: ";
    for (const PharmacyItem& warehouseItem : jsonManager->getWarehouseItems()) {
        itemsLog += QString("%1 (кол-во: %2), ").arg(warehouseItem.getTitle()).arg(warehouseItem.getQuantity());
    }
    Logger::instance().log("MainWindow", itemsLog);

    // Проверка успешности сохранения
    try {
        jsonManager->saveAllToJson();
        Logger::instance().log("MainWindow", QString("Данные сохранены в JSON для лекарства '%1'").arg(name_pharmacy));
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные лекарства");
        Logger::instance().log("MainWindow", QString("Ошибка сохранения JSON: %1").arg(e.what()));
        return;
    }

    ui->item_name_lineEdit->clear();
    ui->item_price_lineEdit->clear();
    ui->item_quantity_lineEdit->clear();
    ui->checkBox->setChecked(false);
    ui->dateEdit->setDate(QDate::currentDate());

    loadMedicinesToTable();
    loadWarehouseToTable();
    loadMedicinesToComboBox();
    loadPharmacysToTable();
    Logger::instance().log("MainWindow", QString("Добавлено лекарство '%1' на склад").arg(name_pharmacy));
    HistoryManager::instance().addOperation(currentUserLogin, "Добавление лекарства",
                                            QString("Добавлено %1 (%2 единиц) на склад").arg(name_pharmacy).arg(quality_pharmacy), 0);

    ui->stackedWidget->setCurrentIndex(11);
}

// Добавление новой аптеки: добавляет аптеку в систему
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
    jsonManager->addPharmacy(pharma);
    jsonManager->saveAllToJson();

    ui->Id_pharmacy_lineEdit->clear();
    ui->adress_lineEdit->clear();
    ui->square_lineEdit->clear();
    ui->countItems_lineEdit->clear();

    loadPharmacysToTable();
    Logger::instance().log("MainWindow", QString("Добавлена аптека ID %1").arg(id));
}

// Добавление нового сотрудника: добавляет сотрудника в систему
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

    jsonManager->addEmployee(role, login, password, fullName, email, pharmacyId);
    jsonManager->saveAllToJson();

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

// Удаление сотрудника: удаляет сотрудника по логину
void MainWindow::on_remove_employees_Button_clicked() {
    QString login = ui->remove_lineEdit->text().trimmed();
    if (jsonManager->removeEmployee(login)) {
        ui->remove_lineEdit->clear();
        jsonManager->saveAllToJson();
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

// Возврат к меню администратора: переключает интерфейс на меню администратора
void MainWindow::on_back_to_view_menu_admin_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

// Удаление аптеки: удаляет аптеку по ID
void MainWindow::on_remove_pharmacy_pushButton_2_clicked() {
    int id = ui->removeID_lineEdit->text().toInt();
    if (jsonManager->removePharmacy(id)) {
        ui->removeID_lineEdit->clear();
        jsonManager->saveAllToJson();
        loadPharmacysToTable();
        ui->stackedWidget->setCurrentIndex(8);
        HistoryManager::instance().addOperation(currentUserLogin, "Удаление аптеки",
                                                QString("Удалена аптека ID %1").arg(id), id);
        QMessageBox::information(this, "Успех", "Аптека успешно удалена");
        Logger::instance().log("MainWindow", QString("Удалена аптека ID %1").arg(id));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Аптека ID %1 не найдена").arg(id));
        Logger::instance().log("MainWindow", QString("Ошибка: Аптека ID %1 не найдена").arg(id));
    }
}

// Возврат к списку аптек: переключает интерфейс на список аптек
void MainWindow::on_back_to_view_pharmacy_pushButton_2_clicked() {
    ui->stackedWidget->setCurrentIndex(8);
    loadPharmacysToTable();
}

// Переход к удалению аптеки: открывает форму удаления аптеки
void MainWindow::on_remove_pharmacy_pushButton_clicked() {
    ui->stackedWidget->setCurrentIndex(9);
}

// Удаление лекарства: удаляет лекарство со склада
void MainWindow::on_remove_pharmacy_item_pushButton_2_clicked() {
    QString title = ui->name_pharmacy_item_lineEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства для удаления");
        Logger::instance().log("MainWindow", "Ошибка удаления: пустое название");
        return;
    }

    if (jsonManager->removeMedicine(title, 0)) {
        ui->name_pharmacy_item_lineEdit->clear();
        jsonManager->saveAllToJson();
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

// Переход к форме покупки: открывает форму покупки/продажи
void MainWindow::on_buy_pharmacy_item_pushButton_clicked() {
    ui->quantity_spinBox->setValue(1);
    loadMedicinesToComboBox();
    ui->stackedWidget->setCurrentIndex(13);
}

// Выполнение покупки/продажи: обрабатывает покупку или продажу лекарства
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
    if (jsonManager->makePurchase(name_medicine, quantity, pharmacyId, buyerLogin)) {
        jsonManager->saveAllToJson();
        loadMedicinesToTable();
        loadMedicinesToComboBox();
        loadPharmacysToTable();
        ui->quantity_spinBox->setValue(1);
        QString action = (currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "Покупка" : "Продажа";
        QMessageBox::information(this, "Успех", QString("%1 успешно завершена").arg(action));
        Logger::instance().log("MainWindow", QString("%1: %2 (%3 единиц) в аптеке %4, пользователь %5")
                                                 .arg(action).arg(name_medicine).arg(quantity).arg(pharmacyId).arg(currentUserLogin));
    } else {
        QMessageBox::warning(this, "Ошибка", QString("Не удалось выполнить %1").arg((currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "покупку" : "продажу"));
        Logger::instance().log("MainWindow", QString("Ошибка %1: %2, %3 единиц, аптека %4")
                                                 .arg((currentUserRole == UserRoleHelper::fromString("Покупатель")) ? "покупки" : "продажи")
                                                 .arg(name_medicine).arg(quantity).arg(pharmacyId));
    }
}

// Отображение истории: переключает интерфейс на историю операций
void MainWindow::on_history_button1_clicked() {
    // Отладочное логирование: проверяем, вызывается ли метод
    Logger::instance().log("MainWindow", "Кнопка истории нажата");

    // Получение ID текущей аптеки через currentUser
    int pharmacyId = currentUser.getPharmacyId();
    Logger::instance().log("MainWindow", QString("Попытка получить pharmacyId: %1").arg(pharmacyId));

    if (pharmacyId <= 0) {
        Logger::instance().log("MainWindow", "Ошибка: у пользователя не указана аптека");
        QMessageBox::warning(this, "Ошибка", "У пользователя не указана аптека");
        return;
    }

    // Формирование текста для отображения
    QString historyText = QString("История операций в аптеке ID %1:\n").arg(pharmacyId);
    const QList<Operation> operations = HistoryManager::instance().getOperations(pharmacyId);
    Logger::instance().log("MainWindow", QString("Получено операций для аптеки ID %1: %2").arg(pharmacyId).arg(operations.size()));

    bool hasOperations = false;
    for (const Operation &op : operations) {
        historyText += QString("Пользователь: %1, Действие: %2, Детали: %3, Время: %4\n")
                           .arg(op.userLogin)
                           .arg(op.action)
                           .arg(op.details)
                           .arg(op.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
        hasOperations = true;
    }

    if (!hasOperations) {
        historyText += "Нет операций в аптеке.";
    }

    // Отображение в QMessageBox
    Logger::instance().log("MainWindow", QString("Отображение истории для аптеки ID %1").arg(pharmacyId));
    QMessageBox::information(this, QString("История операций в аптеке ID %1").arg(pharmacyId), historyText);
    Logger::instance().log("MainWindow", QString("Отображена история операций в аптеке ID %1").arg(pharmacyId));
}

// Отображение товаров аптеки: показывает содержимое склада аптеки продавца
void MainWindow::on_pharmacy_stock_button_clicked() {
    int pharmacyId = currentUser.getPharmacyId();
    if (pharmacyId <= 0) {
        QMessageBox::warning(this, "Ошибка", "У пользователя не указана аптека");
        Logger::instance().log("MainWindow", "Ошибка: у пользователя не указана аптека");
        return;
    }
    QString stockText = QString("Товары в аптеке ID %1:\n").arg(pharmacyId);
    const QList<PharmacyItem> &items = jsonManager->getMedicine();
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

// Переход к форме продажи: открывает форму продажи
void MainWindow::on_sell_button_clicked() {
    ui->quantity_spinBox->setValue(1);
    loadMedicinesToComboBox();
    ui->stackedWidget->setCurrentIndex(16);
}

// Выход из аккаунта: переключает интерфейс на страницу входа
void MainWindow::on_logout_button_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
    Logger::instance().log("MainWindow", QString("Пользователь %1 вышел из аккаунта").arg(currentUserLogin));
}

// Инициализация списка ролей: заполняет ComboBox ролями сотрудников
void MainWindow::initializeRoleComboBox() {
    ui->role_comboBox_employee->clear();
    ui->role_comboBox_employee->addItems({"Продавец", "Курьер"});
    ui->role_comboBox_employee->setEnabled(true);
    Logger::instance().log("MainWindow", "Инициализирован role_comboBox_employee");
}

// Обработка изменения роли: обновляет видимость полей в зависимости от роли
void MainWindow::on_role_comboBox_employee_currentIndexChanged(const QString& role) {
    bool isSeller = (role == "Продавец");
    ui->pharmacyID_comboBox->setVisible(isSeller);
    ui->pharmacyID_label->setVisible(isSeller);
    ui->add_employee_pushButton->setEnabled(isSeller ? !jsonManager->getPharmacy().isEmpty() : true);
    Logger::instance().log("MainWindow", QString("Выбрана роль: %1").arg(role));
}

// Перемещение лекарства: перемещает лекарство из склада в аптеку
void MainWindow::on_transfer_button_clicked() {
    QTableWidget *warehouseTable = ui->view_warehouse_item;
    QList<QTableWidgetItem *> selectedItems = warehouseTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите лекарство из таблицы склада!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: лекарство не выбрано");
        return;
    }

    int row = warehouseTable->currentRow();
    QString medicineTitle = warehouseTable->item(row, 0)->text();
    if (medicineTitle == "Нет лекарств") {
        QMessageBox::warning(this, "Ошибка", "Склад пуст, нет лекарств для перемещения!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: склад пуст");
        return;
    }

    QString pharmacyIdStr = ui->pharmacy_select_comboBox->currentText();
    bool ok;
    int pharmacyId = pharmacyIdStr.toInt(&ok);
    if (!ok || pharmacyIdStr == "Нет доступных аптек") {
        QMessageBox::warning(this, "Ошибка", "Выберите действительную аптеку!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: аптека не выбрана");
        return;
    }

    int transferQuantity = ui->transfer_quantity_spinBox->value();
    if (transferQuantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество для перемещения должно быть больше 0!");
        Logger::instance().log("MainWindow", "Ошибка перемещения: недопустимое количество");
        return;
    }

    // Проверка доступного количества на складе
    QList<PharmacyItem> warehouseItems = jsonManager->getWarehouseItems();
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

    // Проверка аптеки
    const QList<Pharmacy> &pharmacies = jsonManager->getPharmacy();
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

    // Проверка вместимости аптеки
    int currentItems = jsonManager->getPharmacyStock(pharmacyId);
    int maxCapacity = selectedPharmacy.getMaxCapacity();
    Logger::instance().log("MainWindow", QString("Проверка вместимости аптеки ID %1: текущий запас=%2, перемещаемое количество=%3, максимальная вместимость=%4")
                                             .arg(pharmacyId).arg(currentItems).arg(transferQuantity).arg(maxCapacity));

    if (maxCapacity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Максимальная вместимость аптеки не указана или равна 0!");
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: аптека ID %1 имеет maxCapacity=%2").arg(pharmacyId).arg(maxCapacity));
        return;
    }

    if (currentItems + transferQuantity > maxCapacity) {
        QMessageBox::warning(this, "Ошибка", QString("Аптека не может вместить столько лекарств! Осталось места: %1")
                                                       .arg(maxCapacity - currentItems));
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: аптека ID %1 переполнена (требуется %2, доступно %3)")
                                                 .arg(pharmacyId).arg(transferQuantity).arg(maxCapacity - currentItems));
        return;
    }

    // Выполнение перемещения
    bool success = jsonManager->moveMedicineToPharmacy(medicineTitle, transferQuantity, pharmacyId);
    if (!success) {
        QMessageBox::warning(this, "Ошибка", "Не удалось переместить лекарство!");
        Logger::instance().log("MainWindow", QString("Ошибка перемещения: не удалось переместить %1 единиц %2 в аптеку ID %3")
                                                 .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
        return;
    }

    // Сохранение данных
    if (!jsonManager->saveAllToJson()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить данные после перемещения!");
        Logger::instance().log("MainWindow", QString("Ошибка сохранения после перемещения %1 единиц %2 в аптеку ID %3")
                                                 .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
        return;
    }

    // Обновление таблиц
    loadCourierToTable();
    loadPharmaciesToCourierTable();

    // Запись в историю
    HistoryManager::instance().addOperation(currentUserLogin, "Перемещение лекарства",
                                            QString("Перемещено %1 единиц '%2' в аптеку ID %3").arg(transferQuantity).arg(medicineTitle).arg(pharmacyId), pharmacyId);

    QMessageBox::information(this, "Успех", QString("Перемещено %1 единиц '%2' в аптеку ID %3")
                                                     .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
    Logger::instance().log("MainWindow", QString("Успешное перемещение: %1 единиц '%2' в аптеку ID %3")
                                             .arg(transferQuantity).arg(medicineTitle).arg(pharmacyId));
}

// Возврат к главному меню: переключает интерфейс на начальную страницу
void MainWindow::on_back_to_login_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

// Возврат к меню покупателя: переключает интерфейс на меню покупателя из рюкзака
void MainWindow::on_back_to_buyer_menu_from_backpack_button_clicked() {
    ui->stackedWidget->setCurrentIndex(5);
}

// Переход к пополнению счета: открывает форму пополнения баланса
void MainWindow::on_deposit_button_clicked() {
    ui->stackedWidget->setCurrentIndex(18);
}

// Переход к выбору аптеки: открывает интерфейс выбора аптеки для покупки
void MainWindow::on_buy_medicine_button_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
}

// Переход к рюкзаку: открывает интерфейс рюкзака
void MainWindow::on_backpack_button_clicked() {
    ui->stackedWidget->setCurrentIndex(19);
}

// Возврат к меню покупателя: переключает интерфейс на меню покупателя из пополнения
void MainWindow::on_back_to_buyer_menu_button_clicked() {
    ui->stackedWidget->setCurrentIndex(5);
}

// Возврат к меню покупателя: переключает интерфейс на меню покупателя из истории
void MainWindow::on_back_to_buyer_menu_from_history_button_clicked() {
    ui->stackedWidget->setCurrentIndex(5);
}

// Выбор аптеки для покупки: выбирает аптеку и переключает на выбор лекарств
void MainWindow::on_select_pharmacy_button_clicked() {
    int row = ui->pharmacy_selection_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите аптеку");
        return;
    }
    currentPharmacyId = ui->pharmacy_selection_table->item(row, 0)->text().toInt();
    loadMedicinesToTableForBuyer(currentPharmacyId);
    ui->stackedWidget->setCurrentIndex(7);
}

// Возврат к меню покупателя: переключает интерфейс на меню покупателя из выбора аптеки
void MainWindow::on_back_to_buyer_menu_from_pharmacy_button_clicked() {
    ui->stackedWidget->setCurrentIndex(5);
}

// Возврат к выбору аптеки: переключает интерфейс на выбор аптеки из выбора лекарства
void MainWindow::on_back_to_pharmacy_selection_button_clicked() {
    ui->stackedWidget->setCurrentIndex(6);
}

// Покупка лекарства: добавляет лекарство в рюкзак покупателя
void MainWindow::on_add_to_backpack_button_clicked() {
    int row = ui->medicine_selection_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите лекарство");
        return;
    }
    QString medicineTitle = ui->medicine_selection_table->item(row, 0)->text();
    double price = ui->medicine_selection_table->item(row, 1)->text().toDouble();
    double balance = jsonManager->getBuyerBalance(currentUserLogin);

    if (balance < price) {
        QMessageBox::warning(this, "Ошибка", "Недостаточно средств");
        return;
    }

    if (jsonManager->makePurchase(medicineTitle, 1, currentPharmacyId, currentUserLogin)) {
        jsonManager->addToBackpack(currentUserLogin, medicineTitle);
        jsonManager->saveAllToJson();
        double newBalance = jsonManager->getBuyerBalance(currentUserLogin);
        ui->money_lcd->display(newBalance);
        loadMedicinesToTableForBuyer(currentPharmacyId);
        QMessageBox::information(this, "Успех", "Лекарство куплено и добавлено в рюкзак");
        Logger::instance().log("MainWindow", "Лекарство " + medicineTitle + " куплено и добавлено в рюкзак");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось совершить покупку");
    }
}

// Пополнение баланса: увеличивает баланс покупателя
void MainWindow::on_deposit_confirm_button_clicked() {
    bool ok;
    double amount = ui->amount_lineEdit->text().toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "Ошибка", "Некорректная сумма");
        return;
    }
    jsonManager->depositBalance(currentUserLogin, amount);
    double newBalance = jsonManager->getBuyerBalance(currentUserLogin);
    ui->money_lcd->display(newBalance);
    ui->stackedWidget->setCurrentIndex(5);
    Logger::instance().log("MainWindow", "Баланс пополнен на " + QString::number(amount));
}

// Использование лекарства: удаляет лекарство из рюкзака
void MainWindow::on_accept_medicine_button_clicked() {
    int row = ui->backpack_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите лекарство");
        return;
    }
    QString medicineTitle = ui->backpack_table->item(row, 0)->text();
    jsonManager->removeFromBackpack(currentUserLogin, medicineTitle);
    jsonManager->saveAllToJson();
    loadBackpack();
    QMessageBox::information(this, "Успех", "Лекарство использовано");
    Logger::instance().log("MainWindow", "Лекарство " + medicineTitle + " использовано");
}

// переход в меню истории пользователя
void MainWindow::on_history_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(20);
}


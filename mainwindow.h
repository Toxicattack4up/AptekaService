#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "JsonManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_login_button_clicked();       // Обработчик кнопки авторизации
    void on_registr_button_clicked();     // Переход на страницу регистрации
    void on_cancelToMain_Button_clicked(); // Возврат на главную страницу
    void on_Registration_Button_clicked(); // Завершение регистрации
    void on_back_to_view_employee_pushButton_clicked();
    void on_back_to_menu_buyer_pushButton_clicked();
    void on_back_to_view_pharmacy_pushButton_clicked();
    void on_back_to_view_items_pharmacy_pushButton_clicked();
    void on_Back_to_login_clicked();
    void on_back_to_menu_login_pushButton_clicked();
    void on_back_to_view_employees_clicked();
    void on_back_to_menu_items_pharmacy_pushButton_clicked();
    void on_back_menu_admin_pushButton_clicked();
    void on_back_to_menu_login_pushButton_2_clicked();
    void on_back_to_admin_menu_pushButton_clicked();
    void on_Admin_users_clicked();
    void on_add_employees_pushButton_clicked();
    void on_delete_employees_pushButton_clicked();
    void on_add_pharmac_item_pushButton_clicked();
    void on_remove_pharmacy_item_pushButton_clicked();
    void on_add_pharmacy_pushButton_2_clicked();
    void on_Admin_pharmacy_clicked();
    void on_Admin_Item_clicked();
    void on_add_item_pushButton_clicked();
    void on_add_pharmacy_pushButton_clicked();
    void on_add_employee_pushButton_clicked();
    void on_remove_employees_Button_clicked();
    void on_back_to_view_menu_admin_pushButton_2_clicked();
    void on_remove_pharmacy_pushButton_2_clicked();
    void on_back_to_view_pharmacy_pushButton_2_clicked();
    void on_remove_pharmacy_pushButton_clicked();
    void on_remove_pharmacy_item_pushButton_2_clicked();
    void on_Buy_pharmacy_item_pushButton_clicked();
    void on_buy_pushButton_clicked();
    void on_back_to_view_buys_menu_pushButton_clicked();
    void on_buy_pharmacy_item_pushButton_clicked();
    void on_Cancel_seller_menu_clicked();
    void on_back_to_seller_menu_pushButton_clicked();
    void on_Sell_button_clicked();
    void on_sell_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    JsonManager jsonManager; // Объект для работы с данными

    void loadEmployeesToTable(); // Загрузка таблицы пользователей
    void loadPharmacysToTable(); // Загрузка таблицы аптек
    void loadMedicinesToTable(); // Загрузка таблицы лекарств
    void loadBuyesToTable(); // Загрузка таблицы лекарств для покупателя
    void loadMedicinesToComboBox(); // Загрузка названий в меню покупки
    void loadSellerToTable(); // Загрузка таблицы лекарств для продавца
    void loadSellerToComboBox(); // Загрузка названий в меню продажи
};

#endif // MAINWINDOW_H

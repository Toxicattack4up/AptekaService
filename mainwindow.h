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

private:
    Ui::MainWindow *ui;
    JsonManager jsonManager; // Объект для работы с данными
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "JsonManager.h"
#include "centralwarehouse.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_login_button_clicked();
    void on_registr_button_clicked();
    void on_cancelToMain_Button_clicked();
    void on_Registration_Button_clicked();
    void on_back_to_view_employee_pushButton_clicked();
    void on_back_to_view_pharmacy_pushButton_clicked();
    void on_back_to_view_items_pharmacy_pushButton_clicked();
    void on_Back_to_login_clicked();
    void on_back_to_menu_login_pushButton_clicked();
    void on_back_to_view_employees_clicked();
    void on_back_to_menu_items_pharmacy_pushButton_clicked();
    void on_back_menu_admin_pushButton_clicked();
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
    void on_buy_pharmacy_item_pushButton_clicked();
    void on_buy_pushButton_clicked();
    void on_back_to_view_buys_menu_pushButton_clicked();
    void on_role_comboBox_employee_currentIndexChanged(const QString& role);
    void on_transfer_button_clicked();
    void on_history_button1_clicked();
    void on_pharmacy_stock_button_clicked();
    void on_sell_button_clicked();
    void on_logout_button_clicked();
    void on_back_to_login_clicked();
    void on_back_to_buyer_menu_from_backpack_button_clicked();
    void on_deposit_button_clicked();
    void on_buy_medicine_button_clicked();
    void on_backpack_button_clicked();
    void on_back_to_buyer_menu_button_clicked();
    void on_back_to_buyer_menu_from_history_button_clicked();
    void on_select_pharmacy_button_clicked();
    void on_back_to_buyer_menu_from_pharmacy_button_clicked();
    void on_back_to_pharmacy_selection_button_clicked();
    void on_add_to_backpack_button_clicked();
    void on_deposit_confirm_button_clicked();
    void on_accept_medicine_button_clicked();

    void on_history_button_clicked();

private:
    void loadEmployeesToTable();
    void loadPharmacysToTable();
    void loadMedicinesToTable();
    void loadMedicinesToComboBox();
    void loadWarehouseToTable();
    void loadCourierToTable();
    void initializeRoleComboBox();
    void loadPharmaciesToCourierTable();
    void loadPharmaciesToTableForBuyer();
    void loadMedicinesToTableForBuyer(int pharmacyId);
    void loadBackpack();
    void loadPurchaseHistory();

    Ui::MainWindow *ui;
    JsonManager *jsonManager;
    CentralWarehouse *warehouse;

    QString currentUserLogin;
    UserRole currentUserRole;
    User currentUser;
    int currentPharmacyId;
};

#endif // MAINWINDOW_H

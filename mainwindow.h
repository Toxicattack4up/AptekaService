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

private:
    Ui::MainWindow *ui;
    JsonManager jsonManager; // Объект для работы с данными
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "JsonManager.h"
#include "User.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    QString log = "lineEdit";

    User user;


    return a.exec();


}

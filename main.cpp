#include "mainwindow.h"
#include "JsonManager.h"
#include "User.h"


#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Toxicattack4up");
    QCoreApplication::setApplicationName("AptekaService");
    MainWindow w;
    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
   // a.setWindowIcon(QIcon("D:\\GitHub\\Study\\MOBIL\\1lab\\qt\\BT_l\\bluetooth.png"));
    MainWindow w;
    w.show();

    return a.exec();
}

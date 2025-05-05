#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //setting app icon
    app.setWindowIcon(QIcon(":/resources/icon.png"));
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

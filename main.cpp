#include <QtGui/QApplication>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // Create the application
    QApplication a(argc, argv);

    // If we're on a X11 system (i.e. on a Linux system), we need to change the working directory
#ifdef Q_WS_X11
    if(!QDir::home().exists(".dalculator"))
        QDir::home().mkdir(".dalculator");
    QDir::setCurrent(QDir::homePath() + "/.dalculator");
#endif

    // Create the main window and show it
    MainWindow w;
    w.show();

    // Execute the application (i.e. start the event loop)
    return a.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    MainWindow w;
    w.show();

    return a.exec();    
}

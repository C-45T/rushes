#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    //QFile f("qdarkstyle/style.qss");
//    QFile f("QTDark.stylesheet");
//    if (!f.exists())
//    {
//        printf("Unable to set stylesheet, file not found\n");
//    }
//    else
//    {
//        f.open(QFile::ReadOnly | QFile::Text);
//        QTextStream ts(&f);
//        a.setStyleSheet(ts.readAll());
//    }

    MainWindow w;
    w.show();

    return a.exec();    
}

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "proc/ffmpegparser.h"

int main(int argc, char *argv[])
{
    //FFMpegParser parser;
    //qDebug() << parser.openVideo("test.MTS");

    //parser.transcode("test.MTS", "test.mp4");
    //parser.exportProres("test.MTS");

    //return 0;

    QApplication a(argc, argv);

    MainWindow w;
    w.resize(800, 600);
    w.show();

    return a.exec();    
}

#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QPalette>

//#include <signal.h>     // ::signal, ::raise
//#include <boost/stacktrace.hpp>
//#include <boost/filesystem.hpp>

//void my_signal_handler(int signum) {
//    ::signal(signum, SIG_DFL);
//    boost::stacktrace::safe_dump_to("./backtrace.dump");
//    ::raise(SIGABRT);
//}
////]

//void setup_handlers() {
////[getting_started_setup_handlers
//    ::signal(SIGSEGV, &my_signal_handler);
//    ::signal(SIGABRT, &my_signal_handler);
////]
//}


int main(int argc, char *argv[])
{
//    setup_handlers();

//    if (boost::filesystem::exists("./backtrace.dump")) {
//        // there is a backtrace
//        std::ifstream ifs("./backtrace.dump");

//        boost::stacktrace::stacktrace st = boost::stacktrace::stacktrace::from_dump(ifs);
//        std::cout << "Previous run crashed:\n" << st << std::endl;

//        // cleaning up
//        ifs.close();
//        boost::filesystem::remove("./backtrace.dump");
//    }

    QApplication app(argc, argv);
    app.setApplicationName("Rushs!");

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    QFile f("rr_theme.qss");
    //QFile f("QTDark.stylesheet");
    //QFile f("RRDark.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }

    MainWindow w;

    w.setWindowTitle(app.applicationName() + " v" + app.applicationVersion());

    w.show();

    return app.exec();
}

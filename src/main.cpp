/*
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2017 Remy Ruttner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

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
    app.setApplicationName("Rushes!");

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    QFile f("rr_theme.qss");
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



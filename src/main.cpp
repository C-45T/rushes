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

#include <QtGlobal>
#include <stdio.h>
#include <stdlib.h>

QString _log_file_name = "";

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();

    FILE *stream = stderr;
    QFile log_file(_log_file_name);

    if (!log_file.open(QFile::Append | QFile::WriteOnly))
    {
        QByteArray timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toLocal8Bit();
        switch (type) {
        case QtDebugMsg:
            fprintf(stream, "[D %s] %s (%s %s:%u)\n", timestamp.constData(), localMsg.constData(), context.function, context.file, context.line);
            break;
        case QtInfoMsg:
            fprintf(stream, "[I %s] %s (%s %s:%u)\n", timestamp.constData(), localMsg.constData(), context.function, context.file, context.line);
            break;
        case QtWarningMsg:
            fprintf(stream, "[W %s] %s (%s %s:%u)\n", timestamp.constData(), localMsg.constData(), context.function, context.file, context.line);
            break;
        case QtCriticalMsg:
            fprintf(stream, "[C %s] %s (%s %s:%u)\n", timestamp.constData(), localMsg.constData(), context.function, context.file, context.line);
            break;
        case QtFatalMsg:
            fprintf(stream, "[F %s] %s (%s %s:%u)\n", timestamp.constData(), localMsg.constData(), context.function, context.file, context.line);
            abort();
        }

        return;
    }

    QTextStream out(&log_file);
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm::ss.zzz");
    switch (type) {
    case QtDebugMsg:
        out << timestamp << " [D]" << localMsg.constData() << " (" << context.function << " " << context.file << ":" << context.line << ")\n";
        break;
    case QtInfoMsg:
        out << timestamp << " [I]" << localMsg.constData() << " (" << context.function << " " << context.file << ":" << context.line << ")\n";
        break;
    case QtWarningMsg:
        out << timestamp << " [W]" << localMsg.constData() << " (" << context.function << " " << context.file << ":" << context.line << ")\n";
        break;
    case QtCriticalMsg:
        out << timestamp << " [C]" << localMsg.constData() << " (" << context.function << " " << context.file << ":" << context.line << ")\n";
        break;
    case QtFatalMsg:
        out << timestamp << " [F]" << localMsg.constData() << " (" << context.function << " " << context.file << ":" << context.line << ")\n";
        abort();
    }

    log_file.flush();
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Rushes!");

#ifndef QT_DEBUG
    _log_file_name = "rushes_log_" + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".log";
    qInstallMessageHandler(myMessageHandler);
#endif

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



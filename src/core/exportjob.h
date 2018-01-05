/****************************************************************************
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
 ****************************************************************************/

#ifndef EXPORTJOB_H
#define EXPORTJOB_H

#include "core/jobsmaster.h"
#include "data/rush.h"

#include <QProcess>

class ExportJob : public Job
{
    Q_OBJECT
public:
    ExportJob(const Rush& rush, const QString& output_folder, const QString &ffmpeg_command_line);

    void run();

public slots:
    void checkProgress();

private:
    Rush m_rush;
    QString m_output_folder;
    QString m_command_line;

    QProcess *m_export_process;
    int m_total_frames;
};

#endif // EXPORTJOB_H

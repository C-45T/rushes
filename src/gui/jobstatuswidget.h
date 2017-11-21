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

#ifndef JOBSTATUSWIDGET_H
#define JOBSTATUSWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QMap>

#include "core/jobsmaster.h"

class JobStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit JobStatusWidget(const Job *job, QWidget *parent = nullptr);

signals:

public slots:
    void onProgressUpdate(int progress, int total);

private:
    QLabel *m_label;
    QLabel *m_status;
    QProgressBar *m_progress;
    QMap<Job::JobStatus, QString> m_status_texts;

    const Job* m_job;
};

#endif // JOBSTATUSWIDGET_H

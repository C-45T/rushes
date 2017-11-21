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

#ifndef JOBSWIDGET_H
#define JOBSWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>

#include "core/jobsmaster.h"
#include "gui/jobstatuswidget.h"

class JobsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit JobsWidget(const JobsMaster& job_master, bool small_widget = false, QWidget *parent = nullptr);

signals:

public slots:
    void onProgressUpdate(int progress, int total);
    void onJobUpdate(const QString& description);
    void onJobAdded();

private:
    QLabel *m_label;
    QProgressBar *m_progress;
    QVBoxLayout *m_jobs_layout;

    const JobsMaster &m_job_master;
};

#endif // JOBSWIDGET_H

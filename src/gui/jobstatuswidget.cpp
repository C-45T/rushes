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

#include "jobstatuswidget.h"

#include <QHBoxLayout>

JobStatusWidget::JobStatusWidget(const Job *job, QWidget *parent)
    : QWidget(parent),
      m_job(job)
{
    // init text map for status
    m_status_texts[Job::PENDING] = QString(tr("Waiting"));
    m_status_texts[Job::STARTED] = QString(tr("In progress"));
    m_status_texts[Job::FINISHED] = QString(tr("Finished"));

    // create layout
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_label = new QLabel(job->description(), this);
    m_status = new QLabel(m_status_texts[m_job->status()], this);
    m_progress = new QProgressBar(this);
    m_progress->setMaximum(1);
    m_progress->setValue(0);

    if (m_job->status() == Job::FINISHED)
        m_progress->setValue(1);

    layout->addWidget(m_status);
    layout->addWidget(m_label);
    layout->addWidget(m_progress);

    connect(job, SIGNAL(progress(int,int)), this, SLOT(onProgressUpdate(int, int)));
}

void JobStatusWidget::onProgressUpdate(int progress, int total)
{
    m_status->setText(m_status_texts[m_job->status()]);
    m_progress->setMaximum(total);
    m_progress->setValue(progress);
}

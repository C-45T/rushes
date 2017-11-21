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

#include "jobsmaster.h"

QString Job::description() const
{
    return m_description;
}

Job::JobStatus Job::status() const
{
    return m_status;
}

void Job::setStatus(JobStatus status)
{
    m_status = status;

    if (m_status == STARTED)
    {
        emit started();
    }

    if (m_status == FINISHED)
    {
        emit finished();
    }

    if (m_status == CANCELLED)
    {
        emit cancelled();
    }
}

void Job::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged(m_description);
}

JobsMaster::JobsMaster(QObject *parent) : QObject(parent)
{
    m_current_job = 0;
    m_current_progress = 0;
    m_total_estimation = 0;
}

void JobsMaster::addJob(Job *job)
{
    // append job
    m_queue.append(job);
    m_total_estimation += 100;

    // send signal
    emit jobAdded();

    // if first job then start execution loop
    if (m_queue.size() == 1)
        executeJobs();
}

const QList<Job *> &JobsMaster::jobs() const
{
    return m_queue;
}

const QList<Job *> &JobsMaster::finishedJobs() const
{
    return m_finished_jobs;
}

void JobsMaster::executeJobs()
{
    // if no jobs to execute reset estimation time and exit
    if (m_queue.isEmpty())
    {
        m_total_estimation = 0;
        m_current_progress = 0;
        m_finished_jobs.clear();
        emit finishedJob(tr("All jobs performed"));
        return;
    }

    // one job is already running
    if (m_current_job != 0)
        return;

    // get first job
    m_current_job = m_queue.first();

    // add connections
    connect(m_current_job, SIGNAL(finished()), this, SLOT(jobFinished()));
    connect(m_current_job, SIGNAL(progress(int,int)), this, SLOT(onJobProgress(int,int)));
    connect(m_current_job, SIGNAL(descriptionChanged(QString)), this, SIGNAL(runningJob(QString)));

    // start
    m_current_job->start();

    // send signal
    emit startingJob(m_current_job->description());
}

void JobsMaster::jobFinished()
{
    m_queue.pop_front();
    if (m_current_job)
    {
        emit finishedJob("");
        m_finished_jobs.append(m_current_job);
    }

    m_current_job = 0;
    m_current_progress += 100;

    executeJobs();
}

void JobsMaster::onJobProgress(int progress, int total)
{
    emit totalProgress(m_current_progress + progress * 100 / total, m_total_estimation);
}



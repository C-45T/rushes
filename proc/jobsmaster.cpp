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



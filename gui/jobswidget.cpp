#include "jobswidget.h"

#include <QHBoxLayout>

JobsWidget::JobsWidget(const JobsMaster &job_master, bool small_widget, QWidget *parent)
    : QWidget(parent),
      m_job_master(job_master)
{
    m_label = new QLabel( this);
    m_progress = new QProgressBar(this);

    connect(&job_master, SIGNAL(totalProgress(int,int)), this, SLOT(onProgressUpdate(int, int)));
    connect(&job_master, SIGNAL(startingJob(QString)), this, SLOT(onJobUpdate(QString)));
    connect(&job_master, SIGNAL(finishedJob(QString)), this, SLOT(onJobUpdate(QString)));
    connect(&job_master, SIGNAL(runningJob(QString)), this, SLOT(onJobUpdate(QString)));

    if (m_job_master.jobs().size() > 0)
        m_label->setText(m_job_master.jobs()[0]->description());
    else
        m_label->setText(tr("All jobs performed"));

    if (small_widget)
    {
        QHBoxLayout *jobs_layout = new QHBoxLayout(this);
        jobs_layout->addWidget(m_label);
        jobs_layout->addWidget(m_progress);
        return;
    }

    m_jobs_layout = new QVBoxLayout(this);
    m_jobs_layout->addWidget(m_label);
    m_jobs_layout->addWidget(m_progress);

    // display finished jobs
    foreach( Job *job, m_job_master.finishedJobs())
    {
        JobStatusWidget *widget = new JobStatusWidget(job, this);
        m_jobs_layout->addWidget(widget);
    }

    // display queued jobs
    foreach( Job *job, m_job_master.jobs())
    {
        JobStatusWidget *widget = new JobStatusWidget(job, this);
        m_jobs_layout->addWidget(widget);
    }

    connect(&job_master, SIGNAL(jobAdded()), this, SLOT(onJobAdded()));

}

void JobsWidget::onProgressUpdate(int progress, int total)
{
    m_progress->setMaximum(total);
    m_progress->setValue(progress);

    if (progress == total)
        m_label->setText(tr("Jobs done"));
}

void JobsWidget::onJobUpdate(const QString &description)
{
    m_label->setText(description);
}

void JobsWidget::onJobAdded()
{
    if (m_job_master.jobs().size() <= 0)
        return;

    JobStatusWidget *widget = new JobStatusWidget(m_job_master.jobs().last(), this);
    m_jobs_layout->addWidget(widget);
}

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

#ifndef JOBSMASTER_H
#define JOBSMASTER_H

#include <QObject>
#include <QThread>

class Job : public QThread
{
    Q_OBJECT

public:
    enum JobStatus {
        PENDING,
        STARTED,
        FINISHED,
        CANCELLED
    };

public:
    virtual void run() = 0;
    virtual QString description() const;
    virtual JobStatus status() const;

    void setStatus(JobStatus status);
    void setDescription(const QString& description);

signals:
    void progress(int actual, int total);
    void descriptionChanged(const QString& description);
    void started();
    void finished();
    void cancelled();

private:
    JobStatus m_status;
    QString m_description;
};

class JobsMaster : public QObject
{
    Q_OBJECT
public:
    explicit JobsMaster(QObject *parent = nullptr);

    void addJob(Job *job);
    const QList<Job*>& jobs() const;
    const QList<Job*>& finishedJobs() const;

signals:
    void jobAdded();
    void startingJob(const QString& name);
    void finishedJob(const QString& name);
    void runningJob(const QString& name);
    void totalProgress(int progress, int total);

private slots:
    void executeJobs();
    void jobFinished();
    void onJobProgress(int progress, int total);

private:
    QList<Job*> m_queue;
    QList<Job*> m_finished_jobs;
    QList<Job*> m_archived_jobs;
    Job *m_current_job;

    int m_current_progress;
    int m_total_estimation;
};

#endif // JOBSMASTER_H

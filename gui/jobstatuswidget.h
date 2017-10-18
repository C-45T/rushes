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

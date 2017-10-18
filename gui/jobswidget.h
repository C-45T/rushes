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

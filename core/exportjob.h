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
    int m_total_fps;
};

#endif // EXPORTJOB_H

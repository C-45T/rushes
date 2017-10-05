#ifndef EXPORTJOB_H
#define EXPORTJOB_H

#include "jobsmaster.h"
#include "mediainfo.h"

#include <QProcess>

class ExportJob : public Job
{
    Q_OBJECT
public:
    ExportJob(const MediaInfo& media, const QString& output_folder);

    void run();

public slots:
    void checkProgress();

private:
    MediaInfo m_media;
    QString m_output_folder;

    QProcess *m_export_process;
    int m_total_fps;
};

#endif // EXPORTJOB_H

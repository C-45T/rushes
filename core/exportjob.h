#ifndef EXPORTJOB_H
#define EXPORTJOB_H

#include "core/jobsmaster.h"
#include "data/mediainfo.h"

#include <QProcess>

class ExportJob : public Job
{
    Q_OBJECT
public:
    ExportJob(const MediaInfo& media, const QString& output_folder, const QString &ffmpeg_command_line);

    void run();

public slots:
    void checkProgress();

private:
    MediaInfo m_media;
    QString m_output_folder;
    QString m_command_line;

    QProcess *m_export_process;
    int m_total_fps;
};

#endif // EXPORTJOB_H

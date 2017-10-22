#include "exportjob.h"
#include "ffmpegparser.h"

#include <QDebug>
#include <QRegularExpression>

ExportJob::ExportJob(const Rush &rush, const QString &output_folder, const QString& ffmpeg_command_line)
    : m_rush(rush),
      m_output_folder(output_folder),
      m_command_line(ffmpeg_command_line)
{
    setDescription(QString("Export %1 to Prores. Output to %2").arg(rush.file_name, output_folder));
    m_export_process = 0;
    m_total_fps = rush.fps * rush.length;
    setStatus(Job::PENDING);
}

void ExportJob::run()
{
    setStatus(Job::STARTED);
    emit progress(0, m_total_fps);
    m_export_process = FFMpegParser::transcode(m_rush.file_name, m_output_folder, m_command_line);

    connect(m_export_process, SIGNAL(readyReadStandardOutput()), this, SLOT(checkProgress()));
    connect(m_export_process, SIGNAL(readyReadStandardError()), this, SLOT(checkProgress()));

    m_export_process->waitForFinished();

    // emit progress complete
    emit progress(m_total_fps, m_total_fps);
    setStatus(Job::FINISHED);
}

void ExportJob::checkProgress()
{
    if (!m_export_process)
        return;

    // parse output and error lines wich look like this :
    // frame=   30 fps=0.0 q=-0.0 size=    7494kB time=00:00:00.68 bitrate=89930.3kbits/s speed=1.36x
    // and find frame progress
    QByteArray output = m_export_process->readAllStandardOutput();
    output.append(m_export_process->readAllStandardError());

    QRegularExpression re("frame= *([0-9]*) fps=");
    QRegularExpressionMatch match = re.match(output);

    if (match.captured(1).toInt() > 0)
        emit progress(match.captured(1).toInt(), m_total_fps);

    qDebug() << output;
}

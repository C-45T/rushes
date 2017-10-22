#include "facedetectionjob.h"

#include <QDebug>
#include <QRegularExpression>

FaceDetectionJob::FaceDetectionJob(Database& db, Faces& f, const Rush& rush)
    : m_rush(rush),
      m_db(db),
      m_faces(f)
{
    setDescription(QString("Detect face in file %1").arg(rush.file_name));
    m_total_fps = rush.fps * rush.length;
    setStatus(Job::PENDING);
}

void FaceDetectionJob::run()
{
    setStatus(Job::STARTED);
    emit progress(0, m_total_fps);

    connect(&m_faces, SIGNAL(progress(int)), this, SLOT(onProgress(int)));

    try
    {
    QStringList tags = m_faces.parseVideo(m_rush.file_name);

    //tags.append(f.tagUnknwonFaces());

    qDebug() << tags;

    m_db.addTagToRush(m_rush, tags);
    }
    catch (cv::Exception e)
    {
        qDebug() << "exception while trying face detection" << QString::fromStdString(e.msg);
    }

    disconnect(&m_faces, SIGNAL(progress(int)), this, SLOT(onProgress(int)));

    // emit progress complete
    emit progress(m_total_fps, m_total_fps);
    setStatus(Job::FINISHED);
}

void FaceDetectionJob::onProgress(int frame)
{
    emit progress(frame, m_total_fps);
}


#include "facedetectionjob.h"

#include <QDebug>
#include <QRegularExpression>

FaceDetectionJob::FaceDetectionJob(Database& db, Faces& f, const MediaInfo& media)
    : m_media(media),
      m_db(db),
      m_faces(f)
{
    setDescription(QString("Detect face in file %1").arg(media.filename));
    m_total_fps = media.fps * media.length;
    setStatus(Job::PENDING);
}

void FaceDetectionJob::run()
{
    setStatus(Job::STARTED);
    emit progress(0, m_total_fps);

    connect(&m_faces, SIGNAL(progress(int)), this, SLOT(onProgress(int)));

    QStringList tags = m_faces.parseVideo(m_media.filename);

    //tags.append(f.tagUnknwonFaces());

    qDebug() << tags;

    m_db.addTagToRush(m_media, tags);

    disconnect(&m_faces, SIGNAL(progress(int)), this, SLOT(onProgress(int)));

    // emit progress complete
    emit progress(m_total_fps, m_total_fps);
    setStatus(Job::FINISHED);
}

void FaceDetectionJob::onProgress(int frame)
{
    emit progress(frame, m_total_fps);
}


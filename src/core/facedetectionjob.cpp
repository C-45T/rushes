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

#include "facedetectionjob.h"

#include <QDebug>
#include <QRegularExpression>

FaceDetectionJob::FaceDetectionJob(Database& db, Faces& f, const Rush& rush)
    : m_rush(rush),
      m_db(db),
      m_faces(f)
{
    setDescription(QString("Detect face in file %1").arg(rush.file_name));
    m_total_frames = rush.fps * (rush.length / 1000); // approximate but its ok
    setStatus(Job::PENDING);
}

void FaceDetectionJob::run()
{
    setStatus(Job::STARTED);
    emit progress(0, m_total_frames);

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
    emit progress(m_total_frames, m_total_frames);
    setStatus(Job::FINISHED);
}

void FaceDetectionJob::onProgress(int frame)
{
    emit progress(frame, m_total_frames);
}


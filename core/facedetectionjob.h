#ifndef FACEDETECTIONJOB_H
#define FACEDETECTIONJOB_H

#include "jobsmaster.h"
#include "mediainfo.h"
#include "database.h"
#include "faces.h"

class FaceDetectionJob : public Job
{
    Q_OBJECT
public:
    FaceDetectionJob(Database &db, Faces &f, const MediaInfo& media);

    void run();

protected slots:
    void onProgress(int frame);

private:
    MediaInfo m_media;
    int m_total_fps;
    Database& m_db;
    Faces& m_faces;
};

#endif // FACEDETECTIONJOB_H

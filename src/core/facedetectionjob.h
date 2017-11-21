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

#ifndef FACEDETECTIONJOB_H
#define FACEDETECTIONJOB_H

#include "core/jobsmaster.h"
#include "core/faces.h"

#include "data/rush.h"
#include "data/database.h"

class FaceDetectionJob : public Job
{
    Q_OBJECT
public:
    FaceDetectionJob(Database &db, Faces &f, const Rush& rush);

    void run();

protected slots:
    void onProgress(int frame);

private:
    Rush m_rush;
    int m_total_fps;
    Database& m_db;
    Faces& m_faces;
};

#endif // FACEDETECTIONJOB_H

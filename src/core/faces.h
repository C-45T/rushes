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

#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <QObject>
#include <QMap>
#include <QSize>
#include <QPair>
#include <QVector>
#include <QDir>

#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"

typedef QVector<QPair<cv::Mat,cv::Rect>> FaceD;

class Faces : public QObject
{
    Q_OBJECT
public:
    explicit Faces(QObject *parent = nullptr);

    QStringList parseVideo(const QString& filepath);
    void setTrainingSamplesFolder(const QString& path);
    void resetTrainingSamplesFolder();

    QMap<QString, QStringList> tagUnknwonFaces();
    QString whoIsThis(cv::Mat image, cv::Rect face);

    void clear();

protected:
    int getIdByName(const QString& name);

signals:
    void progress(int frame);

public slots:

private:
    std::vector<cv::Mat> m_images;
    std::vector<int> m_labels;

    std::vector<cv::Mat> m_strangers_imgs;
    std::vector<int> m_strangers_lbls;

    QMap<QString,int> m_peoples;
    QMap<int, QString> m_peoples_bimap;

    cv::Ptr<cv::face::FaceRecognizer> m_recognition_model;
    cv::Ptr<cv::face::FaceRecognizer> m_strangers_recognition_model;
    cv::CascadeClassifier m_haar_cascade;

    QSize m_samples_size;
    QMap<QString,FaceD> m_unknown_faces;
    QDir m_training_samples_path;
};

#endif // FACERECOGNIZER_H

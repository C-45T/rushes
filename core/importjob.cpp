#include "importjob.h"

#include <QImage>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QSettings>

#include "opencv2/opencv.hpp"
#include <iostream>

#include "core/ffmpegparser.h"


QImage getThumbnailFromFile(QString filename) {

    int default_image_width = 480;
    int default_image_height = 270;

    QImage emptyImage = QImage(default_image_width, default_image_height, QImage::Format_RGB888);

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    cv::VideoCapture cap(filename.toStdString());

    // Check if camera opened successfully
    if(!cap.isOpened()){
        qWarning() << "Error opening video stream or file" << filename;
        return emptyImage;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    cap.set(CV_CAP_PROP_POS_FRAMES,count/2); //Set index to middle frame

    cv::Mat frame;
    // Capture frame
    cap >> frame;

    // save image
    if (!frame.empty()) {
        QImage qImage = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        frame.release();
        return qImage.scaledToWidth(default_image_width);
    }

    return emptyImage;

}

ImportJob::ImportJob(Database& db, const QStringList &filenames, const QString &to_catalog)
    : m_db(db)
{
    m_import_list = filenames;
    m_catalog = to_catalog;
    setDescription(QString(tr("Import %1 files").arg(m_import_list.size())));

    setStatus(Job::PENDING);
}


void ImportJob::run()
{
    setStatus(Job::STARTED);

    for (int i=0; i<m_import_list.size(); i++)
    {
        emit progress(i+1, m_import_list.size());
        setDescription(QString(tr("Import file : %1").arg(m_import_list[i])));

        // init Rush struct and check if file is not already in database
        Rush rush = m_db.getVideo(m_import_list[i]);

        // if rush already in db skip
        if (!rush.filename.isEmpty())
        {
            qDebug() << m_import_list[i] << "already in database";
            continue;
        }

        // fill filename and build thumbnail
        rush.filename = m_import_list[i];
        QImage thumbnail = getThumbnailFromFile(rush.filename);

        // save thumbnail to cache
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "R2APPS", "aRticho");
        settings.beginGroup("App");
        QDir output_path = QDir(settings.value("thumbnailFolder").toString());
        settings.endGroup();

        rush.thumbnail_filename = output_path.filePath(QFileInfo(rush.filename).baseName() + ".png");

        // make sure no file with same name is arlready generated
        int suffix=0;
        while (QFile(rush.thumbnail_filename).exists()) {
            rush.thumbnail_filename = output_path.filePath(QFileInfo(rush.filename).baseName() + QString("_%1.png").arg(suffix));
            suffix++;
        }

        thumbnail.save(rush.thumbnail_filename);

        // get metadata
        FFMpegParser parser;
        parser.openVideo(rush.filename, rush);

        // add creation time
        QFileInfo file_info(rush.filename);
        rush.utc_creation_time = qMin(file_info.created().toSecsSinceEpoch(), file_info.lastModified().toSecsSinceEpoch());

        // add to database
        m_db.addVideo(rush);
    }

    setStatus(Job::FINISHED);
}

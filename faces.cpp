#include "faces.h"

#include <QImage>
#include <QDebug>
#include <QDir>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QTime>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "facetagwidget.h"

using namespace std;
using namespace cv;
using namespace cv::face;

Faces::Faces(QObject *parent) : QObject(parent)
{
    qDebug() << "create cascade classifier for face detection";
    //haar_cascade.load("haarcascade_frontalface_default.xml");
    m_haar_cascade.load("haarcascade_frontalface_alt.xml");


    qDebug() << "create recognizer";
    //m_recognition_model = FisherFaceRecognizer::create();
    m_recognition_model = LBPHFaceRecognizer::create();
    m_strangers_recognition_model = LBPHFaceRecognizer::create();

    setTrainingSamplesFolder("D:/Dev/Project/face_samples");
}


void Faces::clear()
{
    // release images
    for (unsigned int i=0; i< m_images.size(); i++)
        m_images[i].release();

    // release strangers
    for (unsigned int i=0; i< m_strangers_imgs.size(); i++)
        m_strangers_imgs[i].release();

    m_images.clear();
    m_labels.clear();

    m_strangers_imgs.clear();
    m_strangers_lbls.clear();

    m_peoples.clear();
    m_peoples_bimap.clear();
}

void Faces::setTrainingSamplesFolder(const QString &path)
{
    qDebug() << "Faces::setTrainingSamplesFolder" << path;

    m_training_samples_path = QDir(path);

    clear();

    // load every png files in dir
    QFileInfoList list = m_training_samples_path.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString filename = fileInfo.fileName();
        QString name = filename.split(".").first();
        QString extension = filename.split(".").last();

        if (extension.toLower() == "png")
        {
            m_images.push_back(imread(m_training_samples_path.absoluteFilePath(filename).toStdString(), IMREAD_UNCHANGED));
            QString people = name.split("_")[1];
            if (!m_peoples.contains(people))
            {
                int index = m_peoples.size();
                m_peoples[people]=index;
                m_peoples_bimap[index]=people;
            }
            m_labels.push_back(m_peoples[people]);
        }

    }

    m_samples_size = QSize(m_images[0].cols, m_images[0].rows);

    for (int i=0; i < m_peoples.size(); i++)
        qDebug() << m_peoples.keys()[i] << m_peoples.values()[i];

    qDebug() << "train it" << m_samples_size.width() << m_samples_size.height();
    m_recognition_model->train(m_images, m_labels);
}

QStringList Faces::tagUnknwonFaces()
{
    qDebug() << "Faces::tagUnknwonFaces" << m_unknown_faces.size();
    QSet<QString> newcomers;

    for (int i=0; i<m_unknown_faces.size(); i++)
    {
        QPair<cv::Mat, cv::Rect> current_face = m_unknown_faces[i];

        int predictedLabel = -1;
        double confidence = 0.0;
        cv::Mat extracted_face = current_face.first(current_face.second);
        cv::Mat gray_face;
        cvtColor(extracted_face, gray_face, CV_BGR2GRAY);
        cv::Mat face_resized;
        cv::resize(gray_face, face_resized, Size(m_samples_size.width(), m_samples_size.height()), 1.0, 1.0, INTER_CUBIC);
        m_recognition_model->predict(face_resized, predictedLabel, confidence);

        qDebug() << "new prediction" << predictedLabel << confidence;

        if (predictedLabel >= 0 && confidence > 50) {
            QString tagged = whoIsThis(current_face.first, current_face.second);
            if (!tagged.isEmpty())
                newcomers.insert(tagged);
        }
    }

    return newcomers.toList();
}

QString Faces::whoIsThis(Mat image, Rect face)
{
    QDialog dlg;

    QVBoxLayout *layout = new QVBoxLayout(qobject_cast<QWidget*>(&dlg));
    FaceTagWidget *widget = new FaceTagWidget(image, face);
    layout->addWidget(widget);

    dlg.exec();

    qDebug() << "Faces::whoIsThis" << widget->tag() << face.width << face.height;

    if (!widget->tag().isEmpty())
    {
        // save face to samples
        cv::Mat extracted_face = image(face);
        cv::Mat gray_face;
        cvtColor(extracted_face, gray_face, CV_BGR2GRAY);

        Mat face_resized;
        cv::resize(gray_face, face_resized, Size(m_samples_size.width(), m_samples_size.height()), 1.0, 1.0, INTER_CUBIC);

        QImage m_face = QImage((const unsigned char*)(face_resized.data), face_resized.cols, face_resized.rows, QImage::Format_Grayscale8).rgbSwapped();
        QString destination_filename = m_training_samples_path.absoluteFilePath(QString("f_%1_%2.png").arg(widget->tag(), QString::number(m_face.cacheKey())));

        qDebug() << "save new sample to" << destination_filename;

        m_face.save(destination_filename);

        // add to training samples
        //m_images.push_back(face_resized);
        //m_labels.push_back(m_peoples[widget->tag()]);

        setTrainingSamplesFolder("D:/Dev/Project/face_samples");
        //m_recognition_model->train(m_images, m_labels);

    }

    return widget->tag();
    //QImage qImage = QImage((const unsigned char*)(image.data), image.cols, image.rows, QImage::Format_Grayscale8).rgbSwapped();
}

QStringList Faces::parseVideo(const QString &filename)
{
    QSet<QString> people_in_video;

    qDebug() << "capture video";

    cv::VideoCapture cap(filename.toStdString());
    // Check if camera opened successfully
    if(!cap.isOpened()){
        qWarning() << "Error opening video stream or file" << filename;
        return QStringList(); // emptyImage;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count

    int frame_index = 0;
    cv::Mat frame;

    QTime timer;
    timer.start();

    while (frame_index < count)
    {
        timer.restart();

        qDebug() << timer.elapsed() << "capture frame" << frame_index << "/" << count;
        cap.set(CV_CAP_PROP_POS_FRAMES,frame_index); //Set index to middle frame
        cap >> frame;

        qDebug() << timer.elapsed() << "Clone the current frame";

        Mat original;
        Mat gray;

        // Clone the current frame and resize if necessary:
        if (frame.cols <= 1920)
            original = frame.clone();
        else
            cv::resize(frame, original, Size(1920, 1080), 1.0, 1.0, INTER_CUBIC);

        // Convert the current frame to grayscale:
        cvtColor(original, gray, CV_BGR2GRAY);

        qDebug() << timer.elapsed() << "detect faces";

        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        vector<double> weights;
        vector<int> levels;
        m_haar_cascade.detectMultiScale(gray, faces, levels, weights, 1.1, 6, 0, Size(64, 64), Size(), true);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?

        qDebug() << timer.elapsed() << "search faces" << faces.size();

        for(uint i = 0; i < faces.size(); i++) {

 //           qDebug() << timer.elapsed() << "analyze frame" << i;

            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray(face_i);
            Mat color_face = original(face_i);
            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:
            Mat face_resized;
            cv::resize(face, face_resized, Size(m_samples_size.width(), m_samples_size.height()), 1.0, 1.0, INTER_CUBIC);

            qDebug() << timer.elapsed() << "test skin color";

            // test if it has skin colors
            Mat hsv_image;
            Mat result_mask;
            cv::Scalar  hsv_min = cv::Scalar(0, 30, 60);
            cv::Scalar  hsv_max = cv::Scalar(20, 150, 255);
            cvtColor(color_face, hsv_image, CV_BGR2HSV);
            inRange (hsv_image, hsv_min, hsv_max, result_mask);

            //int total_pixels = color_face.cols * color_face.rows / 8;
            int number_skin_pixels = cv::countNonZero(result_mask);
            int face_pixel_percent = (number_skin_pixels * 100) / (color_face.cols * color_face.rows);

            qDebug() << timer.elapsed() << "test skin color done -" << face_pixel_percent ;

            if (face_pixel_percent > 10)
            {

                // Now perform the prediction, see how easy that is:
                int predictedLabel = -1;
                double confidence = 0.0;
                m_recognition_model->predict(face_resized, predictedLabel, confidence);

                 qDebug() << timer.elapsed() << "prediction -" << predictedLabel;

                //int prediction = m_recognition_model->predict(face_resized);
                // And finally write all we've found out to the original image!
                // First of all draw a green rectangle around the detected face:
 //               rectangle(original, face_i, CV_RGB(0, 255,0), 1);
                // Create the text we will annotate the box with:
 //               string box_text = format("%s - %f skin : %d", m_peoples_bimap[predictedLabel].toLatin1().data(), confidence, face_pixel_percent);
                // Calculate the position for annotated text (make sure we don't
                // put illegal values in there):
 //               int pos_x = std::max(face_i.tl().x - 10, 0);
 //               int pos_y = std::max(face_i.tl().y - 10, 0);
                // And now put it into the image:
 //               putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);

                qDebug() << timer.elapsed() << "save image";

                // save image
//                if (!face_resized.empty()) {
//                    QImage qImage = QImage((const unsigned char*)(face_resized.data), face_resized.cols, face_resized.rows, QImage::Format_Grayscale8).rgbSwapped();
//                    //return qImage.scaledToWidth(default_image_width);
//                    qDebug() << "save image" << face_pixel_percent;
//                    qImage.save(QString("face_detection_%1_%2.png").arg(QString::number(frame_index), QString::number(i)));

//                    QImage qOriginalImage = QImage((const unsigned char*)(original.data), original.cols, original.rows, QImage::Format_RGB888).rgbSwapped();
//                    qOriginalImage.save(QString("original_%1.png").arg(QString::number(frame_index)));
//                }

                if (confidence > 50) {

                    // compare with others strangers in the video if already spotted
                    if (m_strangers_imgs.size() > 0) {
                        m_strangers_recognition_model->predict(face_resized, predictedLabel, confidence);
                    }

                    // if still unknown add to people to tag and to stranger recognition model
                    if (confidence > 50) {
                        m_strangers_imgs.push_back(face_resized.clone());
                        m_strangers_lbls.push_back(m_strangers_imgs.size());
                        // retrain strangers model
                        m_strangers_recognition_model->train(m_strangers_imgs, m_strangers_lbls);
                        m_unknown_faces.push_back(QPair<cv::Mat, cv::Rect>(original.clone(), face_i));
                    }
                }
                else {
                    people_in_video.insert(m_peoples_bimap[predictedLabel]);
                }

            }

        }

        frame_index += 25;
        frame.release();
//        qDebug() << timer.elapsed() << "next frame";

    }


    // release strangers
    for (int i=0; i< m_strangers_imgs.size(); i++)
        m_strangers_imgs[i].release();



    return people_in_video.toList(); // emptyImage;
}


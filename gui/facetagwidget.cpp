#include "facetagwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

//#include "opencv2/face.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/objdetect/objdetect.hpp"

FaceTagWidget::FaceTagWidget(cv::Mat image, cv::Rect face, QWidget *parent) : QWidget(parent)
{
    cv::Mat display_image = image.clone();
    cv::rectangle(display_image, face, CV_RGB(0, 255,0), 1);


    QLabel *image_lbl = new QLabel(this);
    image_lbl->setScaledContents( true );
    m_tag_edit = new QLineEdit(this);
    QPushButton *tag_btn = new QPushButton("TAG", this);

    QImage full_image = QImage((const unsigned char*)(display_image.data), display_image.cols, display_image.rows, QImage::Format_RGB888).rgbSwapped();
    image_lbl->setPixmap(QPixmap::fromImage(full_image));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Qui est-ce ?"), 0, 0, 1, 2);
    layout->addWidget(image_lbl, 1, 0, 1, 2);
    layout->addWidget(m_tag_edit, 2, 0, 1, 1);
    layout->addWidget(tag_btn, 2, 1, 1, 1);

    connect(tag_btn, SIGNAL(clicked()), this, SLOT(close()));
}

QString FaceTagWidget::tag() const
{
    return m_tag_edit->text();
}

#ifndef FACETAGWIDGET_H
#define FACETAGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QImage>

#include "opencv2/opencv.hpp"

class FaceTagWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceTagWidget(cv::Mat image, cv::Rect face, QWidget *parent = nullptr);

    QString tag() const;

signals:

public slots:

private:
    QLineEdit *m_tag_edit;
};

#endif // FACETAGWIDGET_H

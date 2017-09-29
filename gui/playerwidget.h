#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QSlider>

#include <QtAV>
#include <QtAVWidgets>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event);

signals:

public slots:
    void openMedia(const QString& filename);
    void seekBySlider(int value);
    void seekBySlider();
    void playPause();

private slots:
    void updateSlider(qint64 value);
    void updateSlider();
    void updateSliderUnit();

private:
    QtAV::VideoOutput *m_vo;
    QtAV::AVPlayer *m_player;
    QSlider *m_slider;
    int m_unit;
};

#endif // PLAYERWIDGET_H

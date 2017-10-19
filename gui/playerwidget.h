#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QTimer>

#include <QtAV>
#include <QtAVWidgets>

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);

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
    void onVolumeChanged();

private:
    QtAV::VideoOutput *m_vo;
    QtAV::AVPlayer *m_player;
    QSlider *m_slider;
    int m_unit;
    int m_fps;

    QLabel *m_volume_label;
    QTimer m_hide_volume_label_timer;
};

#endif // PLAYERWIDGET_H

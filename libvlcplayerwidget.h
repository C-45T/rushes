#ifndef LIBVLCPLAYERWIDGET_H
#define LIBVLCPLAYERWIDGET_H

#include <vlc/vlc.h>

#include <QWidget>

class QVBoxLayout;
class QPushButton;
class QTimer;
class QFrame;
class QSlider;

#define POSITION_RESOLUTION 10000

class LibvlcPlayerWidget : public QWidget
{
    Q_OBJECT
    QSlider *_positionSlider;
    QSlider *_volumeSlider;

    QFrame *_videoWidget;

    QTimer *poller;
    bool _isPlaying;
    libvlc_instance_t *_vlcinstance;
    libvlc_media_player_t *_mp;
    libvlc_media_t *_m;

    FILE* m_file;

public:
    LibvlcPlayerWidget();
    ~LibvlcPlayerWidget();

private:
    void cleanup();

public slots:
    void playFile(QString file);
    void playPause();
    void updateInterface();
    void changeVolume(int newVolume);
    void changePosition(int newPosition);
};

#endif // LIBVLCPLAYERWIDGET_H

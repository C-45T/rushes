#include "libvlcplayerwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>
#include <QDebug>
#include <QFile>

#include <stdio.h>

using namespace std;

LibvlcPlayerWidget::LibvlcPlayerWidget()
: QWidget()
{
    //preparation of the vlc command
    const char * const vlc_args[] = {
              "--verbose=2", //be much more verbose then normal for debugging purpose
              "--plugin-path=C:\\vlc-0.9.9-win32\\plugins\\" };

    _videoWidget=new QFrame(this);
    _videoWidget->setMinimumHeight(320);
    _videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _volumeSlider=new QSlider(Qt::Horizontal,this);
    _volumeSlider->setMaximum(100); //the volume is between 0 and 100
    _volumeSlider->setToolTip("Audio slider");

    // Note: if you use streaming, there is no ability to use the position slider
    _positionSlider=new QSlider(Qt::Horizontal,this);
    _positionSlider->setMaximum(POSITION_RESOLUTION);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_videoWidget, 1);
    layout->addWidget(_positionSlider);
    layout->addWidget(_volumeSlider);
    setLayout(layout);

    _isPlaying=false;
    m_file = NULL;
    _m = NULL;
    _mp = NULL;
    poller=new QTimer(this);

    //Initialize an instance of vlc

    //create a new libvlc instance
    _vlcinstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);  //tricky calculation of the char space used

    // Create a media player playing environement
    //_mp = libvlc_media_player_new (_vlcinstance);

    //connect the two sliders to the corresponding slots (uses Qt's signal / slots technology)
    connect(poller, SIGNAL(timeout()), this, SLOT(updateInterface()));
    connect(_positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
    connect(_volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));

    poller->start(100); //start timer to trigger every 100 ms the updateInterface slot
}

//desctructor
LibvlcPlayerWidget::~LibvlcPlayerWidget()
{
    /* Stop playing */
    libvlc_media_player_stop (_mp);

    /* Free the media_player */
    libvlc_media_player_release (_mp);

    libvlc_release (_vlcinstance);
}

void LibvlcPlayerWidget::playFile(QString file)
{
    cleanup();

    //the file has to be in one of the following formats /perhaps a little bit outdated)
    /*
    [file://]filename              Plain media file
    http://ip:port/file            HTTP URL
    ftp://ip:port/file             FTP URL
    mms://ip:port/file             MMS URL
    screen://                      Screen capture
    [dvd://][device][@raw_device]  DVD device
    [vcd://][device]               VCD device
    [cdda://][device]              Audio CD device
    udp:[[<source address>]@[<bind address>][:<bind port>]]
    */

    /* Create a new LibVLC media descriptor */
    m_file = fopen(file.toStdString().data(), "r");

    if (m_file != NULL)
    {
         qDebug() << "open media" << m_file;

        _m = libvlc_media_new_fd( _vlcinstance, fileno(m_file));

        qDebug() << "create media player" << m_file;

        _mp = libvlc_media_player_new_from_media (_m);

        qDebug() << "set hwnd" << m_file;

        // /!\ Please note /!\
        //
        // passing the widget to the lib shows vlc at which position it should show up
        // vlc automatically resizes the video to the ´given size of the widget
        // and it even resizes it, if the size changes at the playing

        /* Get our media instance to use our window */
        libvlc_media_player_set_hwnd(_mp, reinterpret_cast<void*>(_videoWidget->winId()) );

        qDebug() << "play";

        /* Play */
        libvlc_media_player_play (_mp );

//        libvlc_time_t length = libvlc_media_player_get_length(_mp);
//        int width = libvlc_video_get_width(_mp);
//        int height = libvlc_video_get_height(_mp);

//        qDebug() << "media : " << width << height << length;

//    libvlc_media_release(_m);
//    fclose(video_file);

        _isPlaying=true;
    }
    else
        qDebug() << "failed to open" << file;
}

void LibvlcPlayerWidget::playPause()
{

}

void LibvlcPlayerWidget::cleanup()
{
    qDebug() << "cleanup";
    _isPlaying=false;

    if (m_file)
    {
        fclose(m_file);
        m_file=NULL;
    }

    if (_mp)
        /* Stop playing */
        libvlc_media_player_stop (_mp);

    /* Free the media_player */
    if (_m)
        libvlc_media_release(_m);

    if (_mp)
        libvlc_media_player_release (_mp);

    qDebug() << "clean";
}

void LibvlcPlayerWidget::changeVolume(int newVolume)
{
    libvlc_audio_set_volume (_mp,newVolume);
}

void LibvlcPlayerWidget::changePosition(int newPosition)
{
    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);

    if (curMedia == NULL)
        return;

    float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
    libvlc_media_player_set_position (_mp, pos);
}

void LibvlcPlayerWidget::updateInterface()
{
    if(!_isPlaying)
        return;

    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (_mp);
    if (curMedia == NULL)
        //cleanup();
        return;

    float pos=libvlc_media_player_get_position (_mp);
    int siderPos=(int)(pos*(float)(POSITION_RESOLUTION));
    _positionSlider->setValue(siderPos);
    int volume=libvlc_audio_get_volume (_mp);
    _volumeSlider->setValue(volume);
}

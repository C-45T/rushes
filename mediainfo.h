#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <iostream>
#include <QDebug>

class MediaInfo
{
public:
    MediaInfo();

public:
    // os data
    QString filename;
    QString thumbnail_filename;
    qint64 utc_creation_time;

    // user data
    int rating;

    // video data
    int length; // seconds
    int width;
    int height;
    int fps;
    int64_t bitrate; // Kb/s
    QString video_codec;

    // audio data
    QString audio_codec;
    int sample_rate; // Hz
    QString channel;
    int audio_bitrate; // kb/s

    void debug() const {
        qDebug() << "-----------------------";
        qDebug() << filename;
        qDebug() << "-----------------------";
        qDebug() << length << "s";
        qDebug() << video_codec;
        qDebug() << width << "x" << height << "@" << fps << "fps";
        qDebug() << bitrate << "kb/s";
        qDebug() << audio_codec;
        qDebug() << sample_rate << "Hz";
        qDebug() << channel;
        qDebug() << audio_bitrate << "kb/s";
        qDebug() << "-----------------------";
    }
};

#endif // MEDIAINFO_H

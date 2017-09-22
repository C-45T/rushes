#ifndef FFMPEGPARSER_H
#define FFMPEGPARSER_H

#include <QString>
#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

class Media
{
public:
    int length; // seconds
    int width;
    int height;
    int64_t bitrate; // Kb/s
    int fps;
    std::string video_codec;
    std::string audio_codec;
    int sample_rate; // Hz
    std::string channel;
    int audio_bitrate; // kb/s

    void debug() {
        qDebug() << length << "s";
        qDebug() << QString::fromStdString(video_codec);
        qDebug() << width << "x" << height << "@" << fps << "fps";
        qDebug() << bitrate << "kb/s";
        qDebug() << QString::fromStdString(audio_codec);
        qDebug() << sample_rate << "Hz";
        qDebug() << QString::fromStdString(channel);
        qDebug() << audio_bitrate << "kb/s";
    }

};

class FFMpegParser
{
public:
    FFMpegParser();

    int openVideo( const QString& filename );

    void metadata( AVFormatContext *context, AVCodecContext *codec_ctx);
    void transcode( const QString& src, const QString& destination );

    static void exportProres(const QString& filename, const QString& path );
};

#endif // FFMPEGPARSER_H

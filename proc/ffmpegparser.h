#ifndef FFMPEGPARSER_H
#define FFMPEGPARSER_H

#include <QString>
#include <QDebug>
#include <QProcess>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#include "mediainfo.h"

class FFMpegParser
{
public:
    FFMpegParser();

    int openVideo( const QString& filename, MediaInfo &media );

    void transcode( const QString& src, const QString& destination );

    static QProcess* exportProres(const QString& filename, const QString& path );

private:
    void metadata(AVFormatContext *context, AVCodecContext *codec_ctx, MediaInfo &media);   
};

#endif // FFMPEGPARSER_H

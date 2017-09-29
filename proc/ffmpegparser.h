#ifndef FFMPEGPARSER_H
#define FFMPEGPARSER_H

#include <QString>
#include <QDebug>

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

    void metadata(AVFormatContext *context, AVCodecContext *codec_ctx, MediaInfo &media);
    void transcode( const QString& src, const QString& destination );

    static void exportProres(const QString& filename, const QString& path );
};

#endif // FFMPEGPARSER_H

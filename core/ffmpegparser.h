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

#include "data/mediainfo.h"

class FFMpegParser
{
public:
    FFMpegParser();

    int openVideo( const QString& filename, MediaInfo &media );

    static QProcess* transcode(const QString& input_filename, const QString& destination_path , const QString &preset);

private:
    void metadata(AVFormatContext *context, AVCodecContext *codec_ctx, MediaInfo &media);   
};

#endif // FFMPEGPARSER_H

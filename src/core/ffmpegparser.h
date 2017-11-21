/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2017 Remy Ruttner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

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

#include "data/rush.h"

class FFMpegParser
{
public:
    FFMpegParser();

    static int getMetaData(const QString& filename, Rush &rush );
    static void init();

    static QProcess* transcode(const QString& input_filename, const QString& destination_path , const QString &preset);
    static QProcess* extractFrame(const QString& input_filename, const QString& destination_path, int seconds = 0);

private:
    static void metadata(AVFormatContext *context, AVCodecContext *codec_ctx, Rush &rush);
};

#endif // FFMPEGPARSER_H

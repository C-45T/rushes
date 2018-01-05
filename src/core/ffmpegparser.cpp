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

#include "ffmpegparser.h"

#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QTime>

void FFMpegParser::init()
{
    av_register_all();
}

FFMpegParser::FFMpegParser()
{
}

int FFMpegParser::getMetaData(const QString &filename, Rush &rush)
{
    AVFormatContext *pFormatCtx = NULL;
    AVDictionary *pDictionary = NULL;

    // Open video file
    if (avformat_open_input(&pFormatCtx, filename.toStdString().c_str(), NULL, &pDictionary)!=0)
      return -1; // Couldn't open file

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
      return -1; // Couldn't find stream information

    // Dump information about file onto standard error
    //av_dump_format(pFormatCtx, 0, filename.toStdString().c_str(), 0);

    AVCodecContext *pCodecCtxOrig = NULL;
    AVCodecContext *pCodecCtx = NULL;

    // Find the first video stream
//    int videoStream=-1;
//    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
//      if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
//        videoStream=i;
//        break;
//      }
//    if(videoStream==-1)
//      return -1; // Didn't find a video stream

//    // Get a pointer to the codec context for the video stream
//    pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;

//    AVCodec *pCodec = NULL;

//    // Find the decoder for the video stream
//    pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
//    if(pCodec==NULL) {
//      fprintf(stderr, "Unsupported codec!\n");
//      return -1; // Codec not found
//    }
//    // Copy context
//    pCodecCtx = avcodec_alloc_context3(pCodec);
//    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
//      fprintf(stderr, "Couldn't copy codec context");
//      return -1; // Error copying codec context
//    }
//    // Open codec
//    if(avcodec_open2(pCodecCtx, pCodec, &pDictionary)<0)
//      return -1; // Could not open codec

    metadata(pFormatCtx, pCodecCtx, rush);

    // Close the codecs
//    avcodec_close(pCodecCtx);
//    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return 0;
}

void FFMpegParser::metadata(AVFormatContext *context, AVCodecContext *, Rush& rush)
{
    if (context->duration != AV_NOPTS_VALUE)
    {
        int hours, mins, secs, us;
        int64_t duration = context->duration + (context->duration <= INT64_MAX - 5000 ? 5000 : 0);
        secs  = duration / AV_TIME_BASE;
        us    = duration % AV_TIME_BASE;
        mins  = secs / 60;
        secs %= 60;
        hours = mins / 60;
        mins %= 60;

        //qDebug() << "duration :" << hours << ":" << mins << ":" << secs;
        rush.length = ((duration / AV_TIME_BASE) * 1000) + (us / 1000);
    }

    if (context->bit_rate)
        rush.bitrate = (int64_t)context->bit_rate / 1000;

    for (uint i = 0; i < context->nb_streams; i++)
    {
        AVStream *st = context->streams[i];
        AVCodecContext *avctx;
        int ret;

        avctx = avcodec_alloc_context3(NULL);
        if (!avctx)
            return;
        ret = avcodec_parameters_to_context(avctx, st->codecpar);
        if (ret < 0) {
            avcodec_free_context(&avctx);
            return;
        }

        //char buf[256];
        //avcodec_string(buf, sizeof(buf), avctx, 1);
        if (avctx->codec_type == AVMEDIA_TYPE_SUBTITLE)
            continue;

        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            qDebug() << context->start_time << context->start_time_realtime << avctx->delay << avctx->timecode_frame_start;

            rush.video_codec = QString(/*av_get_media_type_string(avctx->codec_type)) + " " +*/ avcodec_get_name(avctx->codec_id)) + "(" + avcodec_profile_name(avctx->codec_id, avctx->profile) + ")";
        }
        else if (avctx->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            rush.audio_codec = QString(/*av_get_media_type_string(avctx->codec_type)) + " " +*/ avcodec_get_name(avctx->codec_id));
        }

//        if (avctx->field_order != AV_FIELD_UNKNOWN) {
//            const char *field_order = "progressive";
//            if (avctx->field_order == AV_FIELD_TT)
//                field_order = "top first";
//            else if (avctx->field_order == AV_FIELD_BB)
//                field_order = "bottom first";
//            else if (avctx->field_order == AV_FIELD_TB)
//                field_order = "top coded first (swapped)";
//            else if (avctx->field_order == AV_FIELD_BT)
//                field_order = "bottom coded first (swapped)";

//            av_strlcatf(detail, sizeof(detail), "%s, ", field_order);
//        }


        if (avctx->width) {
            rush.width = avctx->width;
            rush.height = avctx->height;
        }

        if (st->avg_frame_rate.den && st->avg_frame_rate.num)
            rush.fps = av_q2d(st->avg_frame_rate);

        if (avctx->sample_rate)
            rush.sample_rate = avctx->sample_rate;

        if (avctx->channel_layout) {
            char name_buff[32];
            av_get_channel_layout_string(name_buff, 32, 0, avctx->channel_layout);
            rush.channel = name_buff;
        }

        int64_t bitrate;
        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO)
            bitrate = avctx->bit_rate;
        if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            int bits_per_sample = av_get_bits_per_sample(avctx->codec_id);
            bitrate = bits_per_sample ? avctx->sample_rate * (int64_t)avctx->channels * bits_per_sample : avctx->bit_rate;
        }

        if (bitrate != 0)
            rush.audio_bitrate = bitrate / 1000;
        else if (avctx->rc_max_rate > 0)
            rush.audio_bitrate = (int64_t)avctx->rc_max_rate / 1000;

        avcodec_free_context(&avctx);
    }

}

QProcess* FFMpegParser::transcode(const QString &input_filename, const QString &destination_path, const QString& preset)
{
    // get output filename
    QFileInfo file_info = QFileInfo(input_filename);
    QString output_filename = QDir(destination_path).absoluteFilePath(file_info.baseName());

    // create transcoding command
    QString cmd = QString(preset).arg(input_filename, output_filename);
    qDebug() << "FFMpegParser::transcode :" << cmd;

    // start process and return
    QProcess *export_process = new QProcess();
    export_process->start(cmd);

    return export_process;
}

QProcess *FFMpegParser::extractFrame(const QString &input_filename, const QString &destination_file_name, int seconds)
{
    QString preset = "ffmpeg -ss %3 -i ""%1"" -vframes 1 -s 256x144 %2";

    QTime length(0, 0, 0);
    length = length.addSecs(seconds);

    // create transcoding command
    QString cmd = QString(preset).arg(input_filename, destination_file_name, length.toString("hh:mm:ss"));
    qDebug() << "FFMpegParser::extractFrame :" << cmd;

    // start process and return
    QProcess *extract_frame_process = new QProcess();
    extract_frame_process->start(cmd);

    return extract_frame_process;
}


#include "ffmpegparser.h"

#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

FFMpegParser::FFMpegParser()
{
    av_register_all();
}

int FFMpegParser::openVideo(const QString &filename, MediaInfo &media)
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
    av_dump_format(pFormatCtx, 0, filename.toStdString().c_str(), 0);

    AVCodecContext *pCodecCtxOrig = NULL;
    AVCodecContext *pCodecCtx = NULL;

    // Find the first video stream
    int videoStream=-1;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
      if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
        videoStream=i;
        break;
      }
    if(videoStream==-1)
      return -1; // Didn't find a video stream

    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;

    AVCodec *pCodec = NULL;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL) {
      fprintf(stderr, "Unsupported codec!\n");
      return -1; // Codec not found
    }
    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
      fprintf(stderr, "Couldn't copy codec context");
      return -1; // Error copying codec context
    }
    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, &pDictionary)<0)
      return -1; // Could not open codec

    metadata(pFormatCtx, pCodecCtx, media);

    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return 0;
}

void FFMpegParser::metadata(AVFormatContext *context, AVCodecContext *, MediaInfo& media)
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
        media.length = duration / AV_TIME_BASE;
    }

    if (context->bit_rate)
        media.bitrate = (int64_t)context->bit_rate / 1000;

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
            media.video_codec = QString(/*av_get_media_type_string(avctx->codec_type)) + " " +*/ avcodec_get_name(avctx->codec_id)) + "(" + avcodec_profile_name(avctx->codec_id, avctx->profile) + ")";
        else if (avctx->codec_type == AVMEDIA_TYPE_AUDIO)
            media.audio_codec = QString(/*av_get_media_type_string(avctx->codec_type)) + " " +*/ avcodec_get_name(avctx->codec_id));

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
            media.width = avctx->width;
            media.height = avctx->height;
        }

        if (st->avg_frame_rate.den && st->avg_frame_rate.num)
            media.fps = av_q2d(st->avg_frame_rate);

        if (avctx->sample_rate)
            media.sample_rate = avctx->sample_rate;

        if (avctx->channel_layout) {
            char name_buff[32];
            av_get_channel_layout_string(name_buff, 32, 0, avctx->channel_layout);
            media.channel = name_buff;
        }

        int64_t bitrate;
        if (avctx->codec_type == AVMEDIA_TYPE_VIDEO)
            bitrate = avctx->bit_rate;
        if (avctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            int bits_per_sample = av_get_bits_per_sample(avctx->codec_id);
            bitrate = bits_per_sample ? avctx->sample_rate * (int64_t)avctx->channels * bits_per_sample : avctx->bit_rate;
        }

        if (bitrate != 0)
            media.audio_bitrate = bitrate / 1000;
        else if (avctx->rc_max_rate > 0)
            media.audio_bitrate = (int64_t)avctx->rc_max_rate / 1000;

        avcodec_free_context(&avctx);
    }

    //media.debug();
}

void FFMpegParser::transcode(const QString &src, const QString &destination)
{
    QString cmd = QString("ffmpeg -y -i %1 %2").arg(src, destination);
    QProcess::execute(cmd);
}

QProcess* FFMpegParser::exportProres(const QString &filename, const QString &path)
{   
    // build output filename
    QFileInfo file_info = QFileInfo(filename);

    qDebug() << "transcode to prores" << filename << path << file_info.baseName();

    QString output_filename = QDir(path).absoluteFilePath(file_info.baseName() + ".mov");

    if (output_filename == filename)
        output_filename.replace(".mov", "_2.mov");

    qDebug() << "transcode to prores" << filename << output_filename;

    // call to ffmpeg
    QString cmd = QString("ffmpeg -y -i %1 -c:v prores -profile:v 1 %2").arg(filename, output_filename);
    QProcess *export_process = new QProcess();
    export_process->start(cmd);

    return export_process;
}

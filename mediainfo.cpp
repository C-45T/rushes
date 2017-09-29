#include "mediainfo.h"

MediaInfo::MediaInfo()
{
    // os data
    filename = QString();
    thumbnail_filename = QString();
    utc_creation_time = 0;

    // user data
    rating = -1;

    // video data
    length = -1; // seconds
    width = -1;
    height = -1;
    fps = -1;
    bitrate = -1; // Kb/s
    video_codec = QString();

    // audio data
    audio_codec = QString();
    sample_rate = -1; // Hz
    channel = QString();
    audio_bitrate = -1; // kb/s
}

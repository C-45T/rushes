#include "rush.h"

#include <QFileInfo>
#include <QDateTime>

Rush::Rush()
    : QObject()
{
    database_id = -1;

    // os data
    file_name = QString();
    thumbnail_file_name = QString();
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

Rush::Rush(const Rush &other)
    : QObject()
{
    *this = other;
}

Rush &Rush::operator=(const Rush &other)
{
    this->database_id = other.database_id;
    this->file_name = other.file_name;
    this->thumbnail_file_name = other.thumbnail_file_name;
    this->utc_creation_time = other.utc_creation_time;
    this->rating = other.rating;
    this->length = other.length;
    this->width = other.width;
    this->height = other.height;
    this->fps = other.fps;
    this->bitrate = other.bitrate;
    this->video_codec = other.video_codec;
    this->audio_codec = other.audio_codec;
    this->sample_rate = other.sample_rate;
    this->channel = other.channel;
    this->audio_bitrate = other.audio_bitrate;

    return *this;
}

QHash<QString, Rush*> Rush::m_rush_cache = QHash<QString, Rush*>();

Rush *Rush::getRush(const QString& file_name)
{
    if (m_rush_cache.contains(file_name))
        return m_rush_cache[file_name];

    Rush *rush = new Rush();
    rush->file_name = file_name;

    QFileInfo file_info(rush->file_name);
    if (file_info.isFile())
        rush->utc_creation_time = qMin(file_info.created().toSecsSinceEpoch(), file_info.lastModified().toSecsSinceEpoch());

    m_rush_cache[file_name] = rush;
    return rush;
}

bool Rush::hasMetadata()
{
    return (width != -1 && fps != -1 && length != -1);
}

bool Rush::isInDatabase()
{
    return database_id >= 0;
}

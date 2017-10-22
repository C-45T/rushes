#ifndef RUSH_H
#define RUSH_H

#include <QObject>
#include <QHash>

class Rush : public QObject
{
    friend class Database;

public:
    Rush();
    Rush(const Rush& other);

    Rush& operator=(const Rush& other);

    static Rush* getRush(const QString &file_name);

public:

    // TODO : move attributes to private

    // os data
    QString file_name;
    QString thumbnail_file_name;
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

    bool isValid() { return !file_name.isEmpty(); }

    qint64 database_id; // TODO : move this to private

private:

    static QHash<QString, Rush*> m_rush_cache;
};

#endif // RUSH_H

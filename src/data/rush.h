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

#ifndef RUSH_H
#define RUSH_H

#include <QObject>
#include <QHash>

class Extract;

class Rush : public QObject
{
    friend class Database;

public:
    Rush();
    Rush(const Rush& other);

    Rush& operator=(const Rush& other);

    static Rush* getRush(const QString &file_name);

    Extract *extract();

    bool hasMetadata();
    bool isValid() { return !file_name.isEmpty(); }
    bool isInDatabase();

public:

    // TODO : move attributes to private

    // os data
    QString file_name;
    QString thumbnail_file_name;
    qint64 utc_creation_time;

    // user data
    int rating;

    // video data
    int length; // mseconds
    int width;
    int height;
    int fps;
    int total_frames;
    int64_t bitrate; // Kb/s
    QString video_codec;

    // audio data
    QString audio_codec;
    int sample_rate; // Hz
    QString channel;
    int audio_bitrate; // kb/s

    qint64 database_id; // TODO : move this to private

private:

    static QHash<QString, Rush*> m_rush_cache;
};

#endif // RUSH_H

/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2018 Remy Ruttner
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
#ifndef EXTRACT_H
#define EXTRACT_H

#include <QObject>
#include "rush.h"

class Extract : public QObject
{
    friend class Database;

public:
    Extract();
    Extract(Rush *rush);
    Extract(const Extract& other);

    Extract& operator=(const Extract& other);

    void setStart(int start_time);
    void setStop(int stop_time);
    void setKeyMoment(int key_time);
    void setRating(int rating);
    void setComment(const QString &comment);
    void setDatabase_id(const qint64 &database_id);
    void setThumbnail(const QString &thumbnail);

    Rush *rush();

    int max_length() const;
    int start() const;
    int stop() const;
    int key_moment() const;
    int rating() const;
    qint64 database_id() const;
    QString comment() const;
    QString getThumbnail() const;

    static Extract *getExtract(int64_t id);




private:

    // rush data
    Rush* m_rush;

    // user data
    int m_rating;
    QString m_comment;

    // video data
    int m_start; // mseconds
    int m_stop; // mseconds
    int m_key_moment; // mseconds
    int m_max_length;
    QString m_thumbnail;

    qint64 m_database_id;

    static QHash<int64_t, Extract*> m_extract_cache;
};

#endif // EXTRACT_H

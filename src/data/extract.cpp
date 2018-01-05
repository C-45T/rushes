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
#include "extract.h"

QHash<int64_t, Extract*> Extract::m_extract_cache = QHash<int64_t, Extract*>();

Extract::Extract()
    : QObject()
{
    m_database_id = -1;

    // os data
    m_rush = 0;

    // user data
    m_rating = -1;

    // video data
    m_start = -1; // mseconds
    m_stop = -1; // mseconds
    m_key_moment = -1;
    m_max_length = -1;
}

Extract::Extract(Rush* rush)
    : QObject()
{
    m_database_id = -1;

    // os data
    m_rush = rush;

    // user data
    m_rating = rush->rating;

    // video data
    m_start = 0; // mseconds
    m_stop = rush->length; // mseconds
    m_key_moment = rush->length / 2;
    m_max_length = m_stop-m_start;
}


Extract::Extract(const Extract &other)
    : QObject()
{
    *this = other;
}

Extract &Extract::operator=(const Extract &other)
{
    this->m_rush = other.m_rush;
    this->m_database_id = other.m_database_id;
    this->m_rating = other.m_rating;
    this->m_start = other.m_start;
    this->m_stop = other.m_stop;
    this->m_key_moment = other.m_key_moment;
    this->m_max_length = other.m_max_length;

    return *this;
}

void Extract::setStart(int start_time)
{
    m_start = start_time;

    if (m_key_moment < m_start)
        setKeyMoment(m_start);

    if (m_start > m_stop)
        setStop(m_rush->length);

    m_max_length = m_stop - m_start;
}

void Extract::setStop(int stop_time)
{
    m_stop = stop_time;

    if (m_key_moment > m_stop)
        setKeyMoment(m_stop);

    if (m_stop < m_start)
        setStart(0);

    m_max_length = m_stop - m_start;
}

void Extract::setKeyMoment(int key_time)
{
    m_key_moment = key_time;

    if (m_key_moment < m_start)
        setStart(0);

    if (m_key_moment > m_stop)
        setStop(m_rush->length);
}

Extract *Extract::getExtract(int64_t id)
{
    if (m_extract_cache.contains(id))
        return m_extract_cache[id];

    Extract *extract = new Extract();
    extract->setDatabase_id(id);

    m_extract_cache[id] = extract;
    return extract;
}

QString Extract::getThumbnail() const
{
    return m_thumbnail;
}

void Extract::setThumbnail(const QString &thumbnail)
{
    m_thumbnail = thumbnail;
}

int Extract::max_length() const
{
    return m_max_length;
}

int Extract::start() const
{
    return m_start;
}

int Extract::stop() const
{
    return m_stop;
}

int Extract::key_moment() const
{
    return m_key_moment;
}

qint64 Extract::database_id() const
{
    return m_database_id;
}

QString Extract::comment() const
{
    return m_comment;
}

Rush *Extract::rush()
{
    return m_rush;
}

void Extract::setComment(const QString &comment)
{
    m_comment = comment;
}

void Extract::setRating(int rating)
{
    m_rating = rating;
}

int Extract::rating() const
{
    return m_rating;
}

void Extract::setDatabase_id(const qint64 &database_id)
{
    m_database_id = database_id;
}



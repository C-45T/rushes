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

#include "thumbnailrenderer.h"

#include <QPixmap>
#include <QPixmapCache>
#include <QDebug>
#include <QSettings>
#include <QFileInfo>

#include "thirdParty/singleton.h"

#include "ffmpegparser.h"
#include "opencv2/opencv.hpp"
#include <iostream>

ThumbnailRenderer* ThumbnailRenderer::m_instance = 0;

ThumbnailRenderer::ThumbnailRenderer()
    : QObject()
{
    // set cache to 1Go
    QPixmapCache::setCacheLimit(1000000);

    m_dummy_pixmap = QPixmap(256, 144);
    m_dummy_pixmap.fill(QColor(0, 0, 0));
    m_thread_count = 0;
}

ThumbnailRenderer::~ThumbnailRenderer()
{

}

ThumbnailRenderer* ThumbnailRenderer::instance()
{
    if (m_instance== 0)
        m_instance = new ThumbnailRenderer();

    return m_instance;
}

void ThumbnailRenderer::renderPixmap(Rush *rush)
{
    m_rush_queue.append(rush);
    m_queue.insert(rush->file_name);
    if (m_thread_count <8)
    {
        m_thread_count++;
        m_rush_queue.pop_back();
        m_queue.remove(rush->file_name);
        Renderer *thread = new Renderer(rush);
        connect(thread, SIGNAL(finished()), this, SLOT(threadReady()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), this, SIGNAL(thumbnailRendered()));
        thread->start();
    }

}

void ThumbnailRenderer::threadReady()
{
    m_thread_count--;

    if (m_rush_queue.isEmpty())
        return;

    m_thread_count++;
    Rush *next = m_rush_queue.first();
    m_rush_queue.pop_front();
    m_queue.remove(next->file_name);

    Renderer *thread = new Renderer(next);
    connect(thread, SIGNAL(next()), this, SLOT(threadReady()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), this, SIGNAL(thumbnailRendered()));
    thread->start();
}

QPixmap *ThumbnailRenderer::getThumbnail(Rush *rush, bool force_metadata_retrieval)
{
    QPixmap *cache_pixmap = QPixmapCache::find(rush->file_name);

    // if pixmap is in cache return it
    if (cache_pixmap != 0)
    {
        if (force_metadata_retrieval)
            renderPixmap(rush);
        return cache_pixmap;
    }

    //m_queue.append(file_name);
    if (!m_queue.contains(rush->file_name))
        renderPixmap(rush);

    // return a dummy pixmap while rendering it
    return &m_dummy_pixmap;
}

void ThumbnailRenderer::reset()
{
    m_rush_queue.clear();
    m_queue.clear();
}


QMutex Renderer::m_mutex;

Renderer::Renderer(Rush *rush)
    : m_rush(rush)
{
}

void Renderer::run()
{
    if (!m_rush)
        return;

    if (!m_rush->hasMetadata())
        FFMpegParser::getMetaData(m_rush->file_name, *m_rush);

    if (!QPixmapCache::find(m_rush->file_name))
    {
        QSettings settings("settings.ini", QSettings::IniFormat);

        QString output_dir = settings.value("App/thumbnailFolder", "").toString();

        QFileInfo file_info (m_rush->file_name);
        if (file_info.isFile())
        {
            qDebug() << "Renderer::run" << m_rush << m_rush->file_name;

            QString output_file_name = QString("%1/%2_%3%4.jpg").arg(output_dir, file_info.baseName().replace(" ", "_"), QString::number(m_rush->length), QString::number(m_rush->bitrate));

            if (!QFile(output_file_name).exists())
            {
                QProcess *m_extract_process = FFMpegParser::extractFrame(m_rush->file_name, output_file_name, m_rush->length / 2 );
                m_extract_process->waitForFinished(3000);
                qDebug() << m_extract_process->readAll();
            }

            QPixmap p(output_file_name);

            if (!p.isNull()) {
                qDebug() << "Renderer::run" << output_file_name << m_rush;
                QPixmapCache::insert(m_rush->file_name, p);
            }
        }
        else
        {
            QPixmap dummy_pixmap = QPixmap(256, 144);
            dummy_pixmap.fill(QColor(88, 0, 0));
            QPixmapCache::insert(m_rush->file_name, dummy_pixmap);
        }


    }

    emit next();
}

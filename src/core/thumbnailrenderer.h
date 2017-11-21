/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) %YEAR% Remy Ruttner
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

#ifndef THUMBNAILRENDERER_H
#define THUMBNAILRENDERER_H

#include <QVector>
#include <QThread>
#include <QPixmap>
#include <QMutex>

#include "thirdParty/singleton.h"
#include "core/ffmpegparser.h"

class Renderer : public QThread
{
    Q_OBJECT

public:
    Renderer(Rush *rush);
    void run();

private:
    Rush *m_rush;
    static QMutex m_mutex;

signals:
    void next();
};


class ThumbnailRenderer : public QObject
{
    Q_OBJECT
private:
    static ThumbnailRenderer* m_instance;

    ThumbnailRenderer();

public:
    ~ThumbnailRenderer();

    static ThumbnailRenderer* instance();

    void renderPixmap(Rush* rush);
    QPixmap *getThumbnail(Rush* rush, bool force_metadata_retrieval = false);
    void reset();

public slots:
    void threadReady();

signals:
    void thumbnailRendered();

private:

    QPixmap m_dummy_pixmap;
    int m_thread_count;
    QSet<QString> m_queue;
    QList<Rush*> m_rush_queue;
};

#endif // THUMBNAILRENDERER_H

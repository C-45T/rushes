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

#include "mediagraphicitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPixmapCache>

#include "core/thumbnailrenderer.h"

MediaGraphicItem::MediaGraphicItem(Rush *rush)
    : QGraphicsItem()
{
    m_rush = rush;
    m_rating.setMaxStarCount(5);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
}

MediaGraphicItem::~MediaGraphicItem()
{
}

QRectF MediaGraphicItem::boundingRect() const
{
    return QRectF(0, 0, 256, 168); // TODO : remove hardcoded values
}

void MediaGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // set backgroung color
    if (option->state & QStyle::State_Selected)
        painter->fillRect(option->rect, option->palette.highlight());
    else
    {
        if (m_rush->isInDatabase())
            painter->fillRect(option->rect, QColor(53, 53, 53));
        else
            painter->fillRect(option->rect, QColor(103, 103, 103));
    }

    if (m_rush->file_name.isEmpty())
        return;

    // paint thumbnail
    QPixmap *p  = ThumbnailRenderer::instance()->getThumbnail(m_rush);
    if (p)
        painter->drawPixmap(0, 0, *p ); // TODO : remove hardcoded values

    // draw filename
    QRect rectangle = option->rect.adjusted(0, 144, 0, 0); // TODO : remove hardcoded values
    painter->setFont(QFont("Arial", 10 /*, QFont::Bold*/ ));
    painter->setPen(Qt::white);
    painter->drawText(rectangle, Qt::AlignCenter, m_rush->file_name.split("/").last());

    // draw circle if rush in database
//    if (m_rush->database_id >= 0)
//    {
//        painter->setPen(Qt::green);
//        painter->drawText(option->rect.adjusted(224, 0, 0, -136), Qt::AlignCenter, "D");
//    }

    // draw stars
    m_rating.setStarCount(m_rush->rating);
    m_rating.paint(painter, option->rect.adjusted(5, 0, 0, -134), option->palette, StarRating::ReadOnly);
}

Rush *MediaGraphicItem::rush()
{
     return m_rush;
}

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

#ifndef MEDIAGRAPHICITEM_H
#define MEDIAGRAPHICITEM_H

#include <QGraphicsItem>

#include "thirdParty/starrating.h"
#include "data/rush.h"

class MediaGraphicItem : public QGraphicsItem
{
public:
    MediaGraphicItem(Rush *rush);
    ~MediaGraphicItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

    Rush *rush();

private:
    Rush *m_rush;
    StarRating m_rating;
};

#endif // MEDIAGRAPHICITEM_H

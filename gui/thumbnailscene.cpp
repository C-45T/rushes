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

#include "thumbnailscene.h"

#include <QDebug>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>

#include "gui/mediagraphicitem.h"

// TODO :  set as class attributes ?
#define ITEMS_WIDTH 480
#define ITEMS_HEIGHT 320
#define HORIZONTAL_MARGIN 5
#define VERTICAL_MARGIN 5

ThumbnailScene::ThumbnailScene(QObject *parent)
    : QGraphicsScene(parent)
{
    m_last_selected_item = 0;

    connect(this, SIGNAL(focusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)), this, SLOT(onFocusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)));
}

void ThumbnailScene::onFocusItemChanged(QGraphicsItem *, QGraphicsItem *oldFocusItem, Qt::FocusReason)
{
    m_last_selected_item = oldFocusItem;
}

void ThumbnailScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(event);
}

void ThumbnailScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    MediaGraphicItem *thumbnail_item = qgraphicsitem_cast<MediaGraphicItem*>(focusItem());
    if (thumbnail_item)
    {
        emit itemDoubleClicked(thumbnail_item->rush()->file_name);
    }

}

Rush *ThumbnailScene::focusedItem() const
{
    MediaGraphicItem *thumbnail_item = qgraphicsitem_cast<MediaGraphicItem*>(focusItem());
    if (thumbnail_item)
    {
        return thumbnail_item->rush();
    }
    else
    {
        MediaGraphicItem *old_item = qgraphicsitem_cast<MediaGraphicItem*>(m_last_selected_item);
        if (old_item)
            return old_item->rush();
    }

    return 0;
}

QGraphicsItem *ThumbnailScene::lastSelectedItem() const
{
    return m_last_selected_item;
}

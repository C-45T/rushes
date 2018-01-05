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

#ifndef THUMBNAILSCENE_H
#define THUMBNAILSCENE_H

#include <QGraphicsScene>
#include "data/rush.h"
#include "gui/mediagraphicitem.h"

class ThumbnailScene : public QGraphicsScene
{
    Q_OBJECT

public:
    ThumbnailScene(QObject *parent);

public slots:
    void onFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    Rush* focusedItem() const;
    QGraphicsItem *lastSelectedItem() const;

signals:
    void itemDoubleClicked(MediaGraphicItem*);

private:
    int m_scene_width;

    QGraphicsItem *m_last_selected_item;
};

#endif // THUMBNAILSCENE_H

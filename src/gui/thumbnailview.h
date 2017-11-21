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

#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QGraphicsView>
#include <QDir>
#include <QMenu>

#include "thumbnailscene.h"
#include "data/database.h"

class ThumbnailView : public QGraphicsView
{
    Q_OBJECT
public:
    ThumbnailView(Database &db, QWidget *parent = 0);

    void setContextMenu(QMenu *menu);

    QList<Rush *> selectedRush() const;
    Rush *focusedItem() const;

public slots:
    void onScrollToFocusedItem();
    void setFiles(QStringList files, QDir path = QDir(), bool prepend_path = false);

protected:
    void placeItems();

    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void selectNearItem(int right, int down);
    void shiftSelect(QGraphicsItem *item);

signals:
    void itemDoubleClicked(QString);
    void selectionChanged();

private:
    QList<QGraphicsItem*> m_items;
    ThumbnailScene *m_scene;
    Database &m_db;

    int m_item_size;
    QVector<int> m_thumbnail_sizes;

    QMenu *m_context_menu;
};

#endif // THUMBNAILVIEW_H

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

#include "thumbnailview.h"

#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QDir>
#include <QMouseEvent>
#include <QApplication>

#include "core/thumbnailrenderer.h"
#include "mediagraphicitem.h"

#define ITEM_WIDTH 256
#define ITEM_HEIGHT 168
#define MARGIN 4
#define HORIZONTAL_MARGIN 4
#define VERTICAL_MARGIN 4

ThumbnailView::ThumbnailView(Database &db, QWidget *parent)
    : QGraphicsView(parent),
      m_db(db)
{
    // insert sizes
    m_thumbnail_sizes.push_back(64);
    m_thumbnail_sizes.push_back(96);
    m_thumbnail_sizes.push_back(128);
    m_thumbnail_sizes.push_back(192);
    m_thumbnail_sizes.push_back(256);

    m_item_size = 2;

    m_context_menu = 0;

    m_scene = new ThumbnailScene(this);
    setScene(m_scene);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);

    connect(ThumbnailRenderer::instance(), SIGNAL(thumbnailRendered()), this, SLOT(update()));
    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(onScrollToFocusedItem()));
    connect(m_scene, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
    connect(m_scene, SIGNAL(itemDoubleClicked(QString)), this, SIGNAL(itemDoubleClicked(QString)));
}

void ThumbnailView::setContextMenu(QMenu *menu)
{
    m_context_menu = menu;
}

void ThumbnailView::setFiles(QStringList files, QDir path, bool prepend_path)
{
    // delete previous items
    m_items.clear();
    m_scene->clear();

    // create new ones
    foreach (QString file, files)
    {
        QString file_path = file;
        if (prepend_path)
            file_path = path.absoluteFilePath(file);
        Rush *r = Rush::getRush(file_path);

        qDebug() << "ThumbnailView::Rush" << r->file_name << r->database_id << r->width;

        if (r->database_id == -1)
        {
            Rush db_rush = m_db.getRush(file_path);

            if (db_rush.database_id != -1 && !r->hasMetadata())
            {
                *r = db_rush;
            }
        }
        ThumbnailRenderer::instance()->getThumbnail(r);
        QGraphicsItem *item = new MediaGraphicItem(r);
        m_scene->addItem(item);
        m_items.append(item);
    }

    // arrange them
    placeItems();
}

void ThumbnailView::onScrollToFocusedItem()
{
    QGraphicsItem *focused_item = m_scene->focusItem();
    if (focused_item)
    {
        QRect portRect = viewport()->rect();
        QRectF sceneRect = mapToScene(portRect).boundingRect();
        QRectF itemRect = focused_item->mapRectFromScene(sceneRect);

        if (!itemRect.contains(focused_item->boundingRect()))
            ensureVisible(focused_item);
    }
}

void ThumbnailView::placeItems()
{
    int x = 1;
    int y = 0;

    int item_by_line = width() / (m_thumbnail_sizes[m_item_size] + MARGIN);

    foreach (QGraphicsItem *item, m_items)
    {
        item->setPos((x-1) * (ITEM_WIDTH + MARGIN), y * (ITEM_HEIGHT + MARGIN));
        x+= 1;
        if (x > item_by_line)
        {
            x = 1;
            y += 1;
        }
    }

    QRectF scene_rect = m_scene->itemsBoundingRect();
    scene_rect.setWidth(qMax(scene_rect.width(), qreal(item_by_line * (ITEM_WIDTH + MARGIN))));

    qDebug() << scene_rect.width();

    setSceneRect(scene_rect);

    float ratio = float(width()) / float(height());

    if (ratio > 1.0)
        fitInView(0, 0, scene_rect.width(), (scene_rect.width()) * ratio, Qt::KeepAspectRatioByExpanding);
    else
        fitInView(0, 0, scene_rect.width(), (scene_rect.width()) * ratio, Qt::KeepAspectRatio);

}

void ThumbnailView::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_context_menu)
        m_context_menu->exec(event->globalPos());
}

void ThumbnailView::resizeEvent(QResizeEvent *)
{
    placeItems();
}

void ThumbnailView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier ) //any other condition
        event->setModifiers(Qt::ControlModifier);

    QGraphicsView::mousePressEvent(event);

    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
        shiftSelect(m_scene->itemAt(mapToScene(event->pos()), QTransform()));
}

void ThumbnailView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier ) //any other condition
        event->setModifiers(Qt::ControlModifier);

    QMouseEvent dummy_event = QMouseEvent(QEvent::GraphicsSceneMouseRelease, QPointF(-1.0, -1.0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);

    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
        return QGraphicsView::mouseReleaseEvent(&dummy_event);

    return QGraphicsView::mouseReleaseEvent(event);

}

void ThumbnailView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    {
        selectNearItem(-1, 0);
        return;
    }
    case Qt::Key_Right:
    {
        selectNearItem(1, 0);
        return;
    }
    case Qt::Key_Up:
    {
        selectNearItem(0, -1);
        return;
    }
    case Qt::Key_Down:
    {
        selectNearItem(0, 1);
        return;
    }
    case Qt::Key_Minus:
    {
        m_item_size--;
        m_item_size=qMax(0, m_item_size);
        placeItems();
        update();
        return;
    }
    case Qt::Key_Plus:
    {
        m_item_size++;
        m_item_size=qMin(4, m_item_size);
        placeItems();
        update();
        return;
    }
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    {
        foreach (Rush *rush, selectedRush())
            m_db.setRushRating(rush, event->text().toInt());
        update();
        break;
    }
    case Qt::Key_Delete:
    {
        foreach (QGraphicsItem *item, m_scene->selectedItems())
        {
            MediaGraphicItem *thumbnail_item = qgraphicsitem_cast<MediaGraphicItem*>(item);
            if (thumbnail_item)
            {
                m_db.deleteRush(thumbnail_item->rush());
                m_scene->removeItem(thumbnail_item);
                delete thumbnail_item;
                m_items.removeOne(thumbnail_item);
            }
        }

        placeItems();
        break;
    }

    }

    return QGraphicsView::keyPressEvent(event);
}

void ThumbnailView::selectNearItem(int right, int down)
{
    QGraphicsItem *selected_item = m_scene->focusItem();
    if (!selected_item)
        return;

    float targetted_x = selected_item->pos().x() + (right * (ITEM_WIDTH + HORIZONTAL_MARGIN));
    float targetted_y = selected_item->pos().y() + (down * (ITEM_HEIGHT + VERTICAL_MARGIN));

    if (targetted_x > m_scene->itemsBoundingRect().width())
    {
        targetted_x = 0.0;
        targetted_y += ITEM_HEIGHT + VERTICAL_MARGIN;
    }

    if (targetted_x < 0.0)
    {
        targetted_x = m_scene->itemsBoundingRect().width()-1;
        targetted_y -= ITEM_HEIGHT + VERTICAL_MARGIN;
    }

    QGraphicsItem *item = m_scene->itemAt(QPointF(targetted_x, targetted_y), QTransform());
    if (item) {

        //m_last_selected_item = selected_item;
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
        {
            shiftSelect(item);
        }
        else
        {
            m_scene->clearSelection();
            item->setFocus();
            item->setSelected(true);
        }
    }
}

void ThumbnailView::shiftSelect(QGraphicsItem *item)
{
    QGraphicsItem *last_selected_item = m_scene->lastSelectedItem();
    if (item && last_selected_item)
    {

        // get upper left and bottom right points
        QPointF upper_left = last_selected_item->pos();
        QPointF bottom_right = item->pos();
        if ((item->pos().y() < last_selected_item->pos().y()) ||
                ((item->pos().y() == last_selected_item->pos().y()) && (item->pos().x() < last_selected_item->pos().x())))
        {
            upper_left = item->pos();
            bottom_right = last_selected_item->pos();
        }

        // cycle from upper left to bottom right and select all
        for (float cycle_x = upper_left.x(), cycle_y = upper_left.y() ;
             cycle_x < bottom_right.x() || cycle_y < bottom_right.y() ;
            )
        {
            // select item at position cycle_x, cycle_y
            QGraphicsItem *item = m_scene->itemAt(QPointF(cycle_x, cycle_y), QTransform());
            if (item)
            {
                item->setSelected(true);
            }

            cycle_x += ITEM_WIDTH + HORIZONTAL_MARGIN;

            // if end of line, go to next line
            if (cycle_x > m_scene->itemsBoundingRect().width())
            {
                cycle_x = 0.0;
                cycle_y += ITEM_HEIGHT + VERTICAL_MARGIN;
            }

        }

        QGraphicsItem *item = m_scene->itemAt(bottom_right, QTransform());
        if (item)
        {
            item->setFocus();
            item->setSelected(true);
        }
    }
}

QList<Rush*> ThumbnailView::selectedRush() const
{
    QList<Rush*> selection;

    if (!m_scene)
        return selection;

    foreach (QGraphicsItem *item, m_scene->selectedItems())
    {
        MediaGraphicItem *thumbnail_item = qgraphicsitem_cast<MediaGraphicItem*>(item);
        if (thumbnail_item)
        {
            selection.append(thumbnail_item->rush());
        }
    }

    return selection;
}

Rush *ThumbnailView::focusedItem() const
{
    return m_scene->focusedItem();
}

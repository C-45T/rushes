#include "cataloggraphicsscene.h"
#include "videothumbnailgraphicitem.h"

#include <QDebug>
#include <QApplication>

// TODO :  set as class attributes ?
#define ITEMS_WIDTH 480
#define ITEMS_HEIGHT 320
#define HORIZONTAL_MARGIN 10
#define VERTICAL_MARGIN 20

CatalogGraphicsScene::CatalogGraphicsScene(CatalogModel *model)
    : QGraphicsScene(),
      m_model(model)
{
    // insert sizes
    m_thumbnail_sizes.push_back(120);
    m_thumbnail_sizes.push_back(180);
    m_thumbnail_sizes.push_back(240);
    m_thumbnail_sizes.push_back(360);
    m_thumbnail_sizes.push_back(480);

    m_item_size = m_thumbnail_sizes[2];
    m_last_selected_item = 0;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(this, SIGNAL(focusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)), this, SLOT(onFocusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)));
}

void CatalogGraphicsScene::updateScene()
{
    if (!m_model)
        return;

    clear();

    int item_by_line = qMax(1, m_scene_width / (m_item_size + HORIZONTAL_MARGIN));

    //qDebug() << "CatalogGraphicsScene::updateScene - item by line" << item_by_line;

    for (int i=0; i<m_model->itemCount(); i++)
    {
        VideoThumbnailGraphicItem *item = new VideoThumbnailGraphicItem(m_model->record(i));
        item->setPos((i % item_by_line) * (ITEMS_WIDTH + HORIZONTAL_MARGIN), (i / item_by_line) * (ITEMS_HEIGHT + VERTICAL_MARGIN));
        addItem(item);
    }
}

void CatalogGraphicsScene::onSelectionChanged()
{
    qDebug() << "selection changed" << selectedItems().size();

    foreach (QGraphicsItem *item, selectedItems())
    {
        VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(item);
        if (thumbnail_item)
        {
            qDebug() << "selection" << thumbnail_item->itemData();
        }
    }

    VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(focusItem());
    if (thumbnail_item)
    {
        qDebug() << "focused item" << thumbnail_item->itemData();
        CatalogModel *catalog_model = model();
        if (catalog_model)
        {
            qDebug() << "get tags" << thumbnail_item->itemData() << catalog_model->getVideoTags(thumbnail_item->itemData());
            //catalog_model->getMediaInfo(thumbnail_item->itemData()).debug();
        }
    }
}

void CatalogGraphicsScene::onFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason)
{
    m_last_selected_item = oldFocusItem;

    VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(newFocusItem);
    if (thumbnail_item)
    {
        qDebug() << "CatalogGraphicsScene::onFocusItemChanged" << thumbnail_item->itemData() << reason;
    }

//    if (thumbnail_item && (QApplication::keyboardModifiers() & Qt::ShiftModifier))
//    {
//        // get upper left and bottom right points
//        QPointF upper_left = oldFocusItem->pos();
//        QPointF bottom_right = newFocusItem->pos();
//        if ((newFocusItem->pos().y() < oldFocusItem->pos().y()) ||
//            ((newFocusItem->pos().y() == oldFocusItem->pos().y()) && (newFocusItem->pos().x() < oldFocusItem->pos().x())))
//        {
//                upper_left = newFocusItem->pos();
//                bottom_right = oldFocusItem->pos();
//        }

//        // cycle from upper left to bottom right and select all
//        for (float cycle_x = upper_left.x(), cycle_y = upper_left.y() ;
//             cycle_x <= bottom_right.x() || cycle_y <= bottom_right.y() ;
//             cycle_x += ITEMS_WIDTH + HORIZONTAL_MARGIN )
//        {
//            // if end of line, go to next line
//            if (cycle_x > m_scene_width)
//            {
//                cycle_x == 0.0;
//                cycle_y += ITEMS_HEIGHT + VERTICAL_MARGIN;
//            }

//            // select item at position cycle_x, cycle_y
//            QGraphicsItem *item = itemAt(QPointF(cycle_x, cycle_y), QTransform());
//            if (item)
//            {
//                VideoThumbnailGraphicItem *selected_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(item);
//                qDebug() << "select" << selected_item->itemData();
//                item->setSelected(true);
//            }
//        }
//    }
}

void CatalogGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(focusItem());
    if (thumbnail_item)
    {
        emit itemDoubleClicked(thumbnail_item->itemData());
    }

}

void CatalogGraphicsScene::setItemSize(int value)
{
    m_item_size = m_thumbnail_sizes[value-1];

    qDebug() << "CatalogGraphicsScene::setItemSize" << m_item_size;

    updateScene();
}

void CatalogGraphicsScene::setSceneWidth(int value)
{
    m_scene_width = value;
    updateScene();
}

void CatalogGraphicsScene::shiftSelect(QGraphicsItem *item)
{

    qDebug() << "CatalogGraphicsScene::shiftSelect" << item << m_last_selected_item << itemsBoundingRect().width();

    if (item && m_last_selected_item)
    {
        qDebug() << "CatalogGraphicsScene::shiftSelect" << item->pos() << m_last_selected_item->pos();

        QVector<QPointF> selected_points;

        // get upper left and bottom right points
        QPointF upper_left = m_last_selected_item->pos();
        QPointF bottom_right = item->pos();
        if ((item->pos().y() < m_last_selected_item->pos().y()) ||
                ((item->pos().y() == m_last_selected_item->pos().y()) && (item->pos().x() < m_last_selected_item->pos().x())))
        {
            upper_left = item->pos();
            bottom_right = m_last_selected_item->pos();
        }

        // cycle from upper left to bottom right and select all
        for (float cycle_x = upper_left.x(), cycle_y = upper_left.y() ;
             cycle_x < bottom_right.x() || cycle_y < bottom_right.y() ;
            )
        {
            qDebug() << "CatalogGraphicsScene::shiftSelect" << QPointF(cycle_x, cycle_y);

            // select item at position cycle_x, cycle_y
            QGraphicsItem *item = itemAt(QPointF(cycle_x, cycle_y), QTransform());
            if (item)
            {
                selected_points.append(QPointF(cycle_x, cycle_y));
                VideoThumbnailGraphicItem *selected_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(item);
                qDebug() << "CatalogGraphicsScene::shiftSelect" << selected_item->itemData();
                item->setSelected(true);
            }

            cycle_x += ITEMS_WIDTH + HORIZONTAL_MARGIN;

            // if end of line, go to next line
            if (cycle_x > itemsBoundingRect().width())
            {
                cycle_x = 0.0;
                cycle_y += ITEMS_HEIGHT + VERTICAL_MARGIN;
            }

        }

        QGraphicsItem *item = itemAt(bottom_right, QTransform());
        if (item)
            item->setSelected(true);
    }
}

MediaInfo CatalogGraphicsScene::focusedItem() const
{
    VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(focusItem());
    if (thumbnail_item)
    {
        qDebug() << "focused item" << thumbnail_item->itemData();
        CatalogModel *catalog_model = model();
        if (catalog_model)
        {
            qDebug() << "get tags" << thumbnail_item->itemData() << catalog_model->getVideoTags(thumbnail_item->itemData());
            return catalog_model->getMediaInfo(thumbnail_item->itemData());
        }
    }

    return MediaInfo();
}


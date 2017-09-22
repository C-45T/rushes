#include "cataloggraphicsscene.h"
#include "videothumbnailgraphicitem.h"

#include <QDebug>

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

    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(this, SIGNAL(focusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)), this, SLOT(onFocusItemChanged(QGraphicsItem*,QGraphicsItem*,Qt::FocusReason)));
}

void CatalogGraphicsScene::updateScene()
{
    if (!m_model)
        return;

    clear();

    int item_by_line = qMax(1, m_scene_width / (m_item_size + 10)); // TODO : hardcoded value for margin

    //qDebug() << "CatalogGraphicsScene::updateScene - item by line" << item_by_line;

    for (int i=0; i<m_model->itemCount(); i++)
    {
        VideoThumbnailGraphicItem *item = new VideoThumbnailGraphicItem(m_model->record(i));
        item->setPos((i % item_by_line) * 490, (i / item_by_line) * 340); // TODO : hardcoded value
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
        }
    }
}

void CatalogGraphicsScene::onFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason)
{
    VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(newFocusItem);
    if (thumbnail_item)
    {
        qDebug() << "CatalogGraphicsScene::onFocusItemChanged" << thumbnail_item->itemData() << reason;
    }
}

void CatalogGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
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


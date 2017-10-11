#include "cataloggraphicsview.h"

#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QGraphicsItem>

CatalogGraphicsView::CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    m_scene = scene;

    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

void CatalogGraphicsView::setContextMenu(QMenu *menu)
{
    m_context_menu = menu;
}

void CatalogGraphicsView::setContextActions(const QList<QAction *> &actions)
{
    m_context_actions = actions;
}

void CatalogGraphicsView::onSelectionChanged()
{
    emit selectionChanged();
}

void CatalogGraphicsView::onScrollToFocusedItem()
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


void CatalogGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier ) //any other condition
        event->setModifiers(Qt::ControlModifier);

    QGraphicsView::mousePressEvent(event);

    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
        m_scene->shiftSelect(m_scene->itemAt(mapToScene(event->pos()), QTransform()));
}

void CatalogGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier ) //any other condition
        event->setModifiers(Qt::ControlModifier);

    QMouseEvent dummy_event = QMouseEvent(QEvent::GraphicsSceneMouseRelease, QPointF(-1.0, -1.0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);

    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
        return QGraphicsView::mouseReleaseEvent(&dummy_event);

    return QGraphicsView::mouseReleaseEvent(event);

}

void CatalogGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    m_context_menu->exec(event->globalPos());
}

void CatalogGraphicsView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    {
        m_scene->selectCloseItem(-1, 0);
        return;
    }
    case Qt::Key_Right:
    {
        m_scene->selectCloseItem(1, 0);
        return;
    }
    case Qt::Key_Up:
    {
        m_scene->selectCloseItem(0, -1);
        return;
    }
    case Qt::Key_Down:
    {
        m_scene->selectCloseItem(0, 1);
        return;
    }
    }

    return QGraphicsView::keyPressEvent(event);
}

#include "cataloggraphicsview.h"

#include <QMouseEvent>
#include <QApplication>

CatalogGraphicsView::CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    m_scene = scene;

    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

void CatalogGraphicsView::onSelectionChanged()
{
    emit selectionChanged();
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

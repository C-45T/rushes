#ifndef CATALOGGRAPHICSSCENE_H
#define CATALOGGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "core/catalogmodel.h"
#include "data/rush.h"

class CatalogGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:   
    CatalogGraphicsScene(CatalogModel *model);

public slots:
    void updateScene();
    void onSelectionChanged();
    void onFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void setItemSize(int value);
    void setSceneWidth(int value);

    void selectCloseItem(int right, int down);
    void shiftSelect(QGraphicsItem *item);

    CatalogModel* model() { return m_model; }
    CatalogModel* model() const { return m_model; }

    Rush focusedItem() const;

signals:
    void itemDoubleClicked(QString);

private:
    CatalogModel *m_model;
    int m_item_size;
    int m_scene_width;

    QVector<int> m_thumbnail_sizes;
    QGraphicsItem *m_last_selected_item;
};

#endif // CATALOGGRAPHICSSCENE_H

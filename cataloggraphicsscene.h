#ifndef CATALOGGRAPHICSSCENE_H
#define CATALOGGRAPHICSSCENE_H

#include <QGraphicsScene>
#include "catalogmodel.h"

class CatalogGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:   
    CatalogGraphicsScene(CatalogModel *model);

public slots:
    void updateScene();
    void onSelectionChanged();
    void onFocusItemChanged(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void setItemSize(int value);
    void setSceneWidth(int value);

    CatalogModel* model() { return m_model; }

signals:
    void itemDoubleClicked(QString);

private:
    CatalogModel *m_model;
    int m_item_size;
    int m_scene_width;

    QVector<int> m_thumbnail_sizes;
};

#endif // CATALOGGRAPHICSSCENE_H

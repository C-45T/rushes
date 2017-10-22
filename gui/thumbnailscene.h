#ifndef THUMBNAILSCENE_H
#define THUMBNAILSCENE_H

#include <QGraphicsScene>
#include "data/rush.h"

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
    void itemDoubleClicked(QString);

private:
    int m_scene_width;

    QGraphicsItem *m_last_selected_item;
};

#endif // THUMBNAILSCENE_H

#ifndef CATALOGGRAPHICSVIEW_H
#define CATALOGGRAPHICSVIEW_H

#include <QGraphicsView>

#include "cataloggraphicsscene.h"

class CatalogGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent = 0);
    
public slots:
    void onSelectionChanged();

signals:
    void selectionChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    CatalogGraphicsScene *m_scene;
};

#endif // CATALOGGRAPHICSVIEW_H

#ifndef CATALOGGRAPHICSVIEW_H
#define CATALOGGRAPHICSVIEW_H

#include <QGraphicsView>

#include "cataloggraphicsscene.h"

class CatalogGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent = 0);

    void setContextActions(const QList<QAction *>& actions);
    
public slots:
    void onSelectionChanged();
    void onScrollToFocusedItem();

signals:
    void selectionChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    CatalogGraphicsScene *m_scene;
    QList<QAction *> m_context_actions;
};

#endif // CATALOGGRAPHICSVIEW_H

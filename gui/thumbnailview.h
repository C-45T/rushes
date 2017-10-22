#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QGraphicsView>
#include <QDir>
#include <QMenu>

#include "thumbnailscene.h"
#include "data/database.h"

class ThumbnailView : public QGraphicsView
{
    Q_OBJECT
public:
    ThumbnailView(Database &db, QWidget *parent = 0);

    void setContextMenu(QMenu *menu);

    QList<Rush *> selectedRush() const;
    Rush *focusedItem() const;

public slots:
    void onScrollToFocusedItem();
    void setFiles(QStringList files, QDir path = QDir(), bool prepend_path = false);

protected:
    void placeItems();

    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    void selectNearItem(int right, int down);
    void shiftSelect(QGraphicsItem *item);

signals:
    void itemDoubleClicked(QString);
    void selectionChanged();

private:
    QList<QGraphicsItem*> m_items;
    ThumbnailScene *m_scene;
    Database &m_db;

    int m_item_size;
    QVector<int> m_thumbnail_sizes;

    QMenu *m_context_menu;
};

#endif // THUMBNAILVIEW_H

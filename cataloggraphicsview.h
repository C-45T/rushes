#ifndef CATALOGGRAPHICSVIEW_H
#define CATALOGGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QSlider>
#include <QResizeEvent>

#include "cataloggraphicsscene.h"

class CatalogGraphicsView : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent = nullptr);

    QGraphicsView *view();

    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QStringList selectedFiles() const;

signals:

public slots:
    void updateView();
    void onThumbnailSizeChanged(int);

private:
    QGraphicsView *m_graphics_view;
    QSlider *m_thumbnail_size_slider;
    CatalogGraphicsScene *m_scene;
};

#endif // CATALOGGRAPHICSVIEW_H

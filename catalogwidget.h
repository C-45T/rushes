#ifndef CATALOGWIDGET_H
#define CATALOGWIDGET_H

#include <QWidget>
#include <gui/cataloggraphicsview.h>
#include <QSlider>
#include <QResizeEvent>

#include "cataloggraphicsscene.h"
#include "mediainfo.h"

class CatalogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogWidget(CatalogGraphicsScene *scene, QWidget *parent = nullptr);

    CatalogGraphicsView *view();

    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    QStringList selectedFiles() const;
    QList<MediaInfo> selectedMedia() const;

    MediaInfo focusedItem() const;

signals:

public slots:
    void updateView();
    void onThumbnailSizeChanged(int);

private:
    CatalogGraphicsView *m_graphics_view;
    QSlider *m_thumbnail_size_slider;
    CatalogGraphicsScene *m_scene;
};

#endif // CATALOGWIDGET_H

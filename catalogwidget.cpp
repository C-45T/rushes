#include "catalogwidget.h"

#include <QVBoxLayout>
#include <QGuiApplication>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>

#include "faces.h"
#include "videothumbnailgraphicitem.h"

CatalogWidget::CatalogWidget(CatalogGraphicsScene *scene, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_graphics_view = new CatalogGraphicsView(scene, this);
    m_thumbnail_size_slider = new QSlider(this);

    m_scene = scene;

    layout->addWidget(m_graphics_view);
    layout->addWidget(m_thumbnail_size_slider, Qt::AlignRight);

    m_thumbnail_size_slider->setOrientation(Qt::Horizontal);
    m_thumbnail_size_slider->setRange(1, 5);
    m_thumbnail_size_slider->setValue(3);
    m_thumbnail_size_slider->hide();
    m_graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphics_view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_graphics_view->setContentsMargins(10, 10, 10, 10);

    connect(m_thumbnail_size_slider, SIGNAL(valueChanged(int)), this, SLOT(onThumbnailSizeChanged(int)));

    onThumbnailSizeChanged(m_thumbnail_size_slider->value());
}

CatalogGraphicsView *CatalogWidget::view()
{
    return m_graphics_view;
}

void CatalogWidget::resizeEvent(QResizeEvent *event)
{
    m_scene->setSceneWidth(event->size().width());

    QWidget::resizeEvent(event);

    updateView();
}

void CatalogWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Delete:
        {
            CatalogModel *catalog_model = m_scene->model();
            if (catalog_model)
            {
                QStringList files_to_delete = selectedFiles();
                catalog_model->deleteFromCatalog(files_to_delete);
            }
        }
            break;
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        {
            CatalogModel *catalog_model = m_scene->model();
            if (catalog_model)
            {
                QStringList files_to_update = selectedFiles();
                catalog_model->updateRating(files_to_update, event->text().toInt());
            }
        }
            break;
        case Qt::Key_Plus:
        {
            m_thumbnail_size_slider->setValue(m_thumbnail_size_slider->value() + 1);
            break;
        }
        case Qt::Key_Minus:
        {
            m_thumbnail_size_slider->setValue(m_thumbnail_size_slider->value() - 1);
            break;
        }
    }

    return QWidget::keyPressEvent(event);

}

void CatalogWidget::wheelEvent(QWheelEvent *event)
{
//    QPoint numDegrees = event->angleDelta() / 8;

//    if (!numDegrees.isNull()) {
//        QPoint numSteps = numDegrees / 15;
//        m_thumbnail_size_slider->setValue(m_thumbnail_size_slider->value() + (1 * numSteps.y()));
//    }

//    event->accept();
}

void CatalogWidget::updateView()
{
    float ratio = float(m_graphics_view->width()) / float(m_graphics_view->height());
    //qDebug() << "update view - ratio" << ratio;
    m_graphics_view->setSceneRect(m_scene->itemsBoundingRect());
    if (ratio > 1.0)
        m_graphics_view->fitInView(0, 0, m_scene->itemsBoundingRect().width(), (m_scene->itemsBoundingRect().width()) * ratio, Qt::KeepAspectRatioByExpanding);
    else
        m_graphics_view->fitInView(0, 0, m_scene->itemsBoundingRect().width(), (m_scene->itemsBoundingRect().width()) * ratio, Qt::KeepAspectRatio);
}

QStringList CatalogWidget::selectedFiles() const
{
    QStringList selection;

    if (!m_scene)
        return selection;

    foreach (QGraphicsItem *item, m_scene->selectedItems())
    {
        VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(item);
        if (thumbnail_item)
        {
            selection.append(thumbnail_item->itemData().filename);
        }
    }

    return selection;
}

QList<MediaInfo> CatalogWidget::selectedMedia() const
{
    QList<MediaInfo> selection;

    if (!m_scene)
        return selection;

    foreach (QGraphicsItem *item, m_scene->selectedItems())
    {
        VideoThumbnailGraphicItem *thumbnail_item = qgraphicsitem_cast<VideoThumbnailGraphicItem*>(item);
        if (thumbnail_item)
        {
            selection.append(thumbnail_item->itemData());
        }
    }

    return selection;
}

MediaInfo CatalogWidget::focusedItem() const
{
    if (!m_scene)
        return MediaInfo();

    return m_scene->focusedItem();
}

void CatalogWidget::onThumbnailSizeChanged(int value)
{
    m_scene->setItemSize(value);

    updateView();
}

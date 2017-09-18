#include "cataloggraphicsview.h"

#include <QVBoxLayout>
#include <QGuiApplication>
#include <QDebug>

#include "faces.h"
#include "videothumbnailgraphicitem.h"

CatalogGraphicsView::CatalogGraphicsView(CatalogGraphicsScene *scene, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_graphics_view = new QGraphicsView(scene, this);
    m_thumbnail_size_slider = new QSlider(this);

    m_scene = scene;

    layout->addWidget(m_graphics_view);
    layout->addWidget(m_thumbnail_size_slider, Qt::AlignRight);

    m_thumbnail_size_slider->setOrientation(Qt::Horizontal);
    m_thumbnail_size_slider->setRange(1, 5);

    connect(m_thumbnail_size_slider, SIGNAL(valueChanged(int)), this, SLOT(onThumbnailSizeChanged(int)));

    onThumbnailSizeChanged(m_thumbnail_size_slider->value());
}

QGraphicsView *CatalogGraphicsView::view()
{
    return m_graphics_view;
}

void CatalogGraphicsView::resizeEvent(QResizeEvent *event)
{
    m_scene->setSceneWidth(event->size().width());

    QWidget::resizeEvent(event);

    updateView();
}

void CatalogGraphicsView::keyPressEvent(QKeyEvent *event)
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
        case Qt::Key_T:
        {
            if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
            {
                CatalogModel *catalog_model = m_scene->model();
                if (catalog_model)
                {
                    //QStringList files_to_update = selectedFiles();
                    foreach (QString filename, selectedFiles())
                    {
                        Faces f;
                        QStringList tags = f.parseVideo(filename);
                        tags.append(f.tagUnknwonFaces());

                        qDebug() << tags;

                        catalog_model->addTags(filename, tags);
                    }
                }
            }
        }

    }

    return QWidget::keyPressEvent(event);
}

void CatalogGraphicsView::updateView()
{
    int margin = 20;
    float ratio = m_graphics_view->width() / m_graphics_view->height();
    m_graphics_view->setSceneRect(m_scene->itemsBoundingRect());
    m_graphics_view->fitInView(-margin, 0, m_scene->itemsBoundingRect().width() + margin, (m_scene->itemsBoundingRect().width() + 2*margin) * ratio, Qt::KeepAspectRatioByExpanding);
}

QStringList CatalogGraphicsView::selectedFiles() const
{
    QStringList selection;

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

void CatalogGraphicsView::onThumbnailSizeChanged(int value)
{
    m_scene->setItemSize(value);

    updateView();
}

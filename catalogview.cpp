#include "catalogview.h"

#include <QKeyEvent>
#include <QDebug>
#include <QHeaderView>
#include <QResizeEvent>
#include <QGuiApplication>

#include "catalogmodel.h"
#include "catalogitemdelegate.h"
#include "faces.h"

CatalogView::CatalogView(QWidget* parent)
    : QTableView(parent)
{
    CatalogItemDelegate *delegate = new CatalogItemDelegate(this);
    setItemDelegate(delegate);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    resizeRowsToContents();
}

void CatalogView::resizeEvent(QResizeEvent *event)
{
    CatalogModel *catalog_model = qobject_cast<CatalogModel *>(model());
    if (catalog_model)
    {
        //qDebug() << event->size().width() << event->size().width() / 320;
        catalog_model->setThumbnailColumnNumber(qMax(event->size().width() / 320, 2));
    }

    QTableView::resizeEvent(event);
}

void CatalogView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    CatalogModel *catalog_model = qobject_cast<CatalogModel *>(model());
    if (catalog_model)
    {
        qDebug() << "get tags" << current.data().toString() << catalog_model->getVideoTags(current.data().toString());
    }
}

void CatalogView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Delete:
        {
            CatalogModel *catalog_model = qobject_cast<CatalogModel *>(model());
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
            CatalogModel *catalog_model = qobject_cast<CatalogModel *>(model());
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
                CatalogModel *catalog_model = qobject_cast<CatalogModel *>(model());
                if (catalog_model)
                {
                    QStringList files_to_update = selectedFiles();
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

    return QTableView::keyPressEvent(event);
}

QStringList CatalogView::selectedFiles() const
{
    QStringList result;
    foreach (QModelIndex index, selectedIndexes()) {
        result.append(index.data().toString());
    }

    return result;
}

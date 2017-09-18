#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QStandardItemModel>
#include <QSqlQueryModel>
#include "catalogfilter.h"

class CatalogModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    CatalogModel(QSqlDatabase &db, QObject *parent = 0);

    QString catalog() const;

    QVariant data(const QModelIndex &index, int role) const;
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int itemCount() const;

    void setThumbnailColumnNumber(int nb);

    void setFilter(CatalogFilter *filter);
    void setCatalog(const QString &catalog);
    void addToCatalog(const QString &catalog, const QStringList &filenames);
    void deleteFromCatalog(const QStringList &files);

    void updateRating(const QStringList &files, int rating);
    void addTags(const QString &filename, const QStringList &tags);
    int getVideoId(const QString &filename) const;
    QStringList getVideoTags(const QString &filename) const;

    void processQueue();

signals:
    void catalogChanged();

public slots:
    void onVideoFrameExtracted(/*const QtAV::VideoFrame& frame*/);
    void updateCatalog();

private:
    QSqlDatabase &m_db;
    QString m_catalog;
    QStringList m_queue;
    QString m_processingFile;
    qlonglong m_rows;
    int m_thumbnail_column_number;
    CatalogFilter *m_selection_filter;

};

#endif // CATALOGMODEL_H

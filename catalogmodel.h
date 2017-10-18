#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QStandardItemModel>
#include <QSqlQueryModel>
#include "catalogfilter.h"
#include "database.h"

#include "mediainfo.h"

class CatalogModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    CatalogModel(Database &db, QObject *parent = 0);

    QString catalog() const;

    QVariant data(const QModelIndex &index, int role) const;
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    int rowCount(const QModelIndex &index = QModelIndex()) const;
    int itemCount() const;

    void setThumbnailColumnNumber(int nb);

    void setFilter(CatalogFilter *filter);
    void deleteFromCatalog(const QStringList &files);

    void updateRating(const QStringList &files, int rating);
    int getVideoId(const QString &filename) const;
    MediaInfo getMediaInfo(const QString &filename) const;

signals:
    void catalogChanged();

public slots:
    void setCatalog(const QString &catalog);
    void updateCatalog();

private:
    Database &m_db;
    QString m_catalog;
    QString m_processingFile;
    qlonglong m_rows;
    int m_thumbnail_column_number;
    CatalogFilter *m_selection_filter;

};

#endif // CATALOGMODEL_H

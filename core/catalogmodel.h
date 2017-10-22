#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QStandardItemModel>
#include <QSqlQueryModel>

#include "core/catalogfilter.h"

#include "data/database.h"
#include "data/rush.h"

class CatalogModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    CatalogModel(Database &db, QObject *parent = 0);

    QString catalog() const;

    QVariant data(const QModelIndex &index, int role) const;
    int itemCount() const;

    void setFilter(CatalogFilter *filter);
    void deleteFromCatalog(const QStringList &files);

    void updateRating(const QStringList &files, int rating);
    int getVideoId(const QString &filename) const;
    Rush getRush(const QString &filename) const;

signals:
    void catalogChanged();

public slots:
    void setBin(const QString &bin_name);
    void updateCatalog();

private:
    Database &m_db;
    QString m_bin_name;
    QString m_processingFile;
    qlonglong m_rows;
    int m_thumbnail_column_number;
    CatalogFilter *m_selection_filter;

};

#endif // CATALOGMODEL_H

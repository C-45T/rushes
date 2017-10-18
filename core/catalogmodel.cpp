#include "catalogmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTimer>
#include <QSqlRecord>


CatalogModel::CatalogModel(Database &db, QObject *parent) :
    QSqlQueryModel(parent),
    m_db(db)
{
    m_rows = 0;
    m_thumbnail_column_number = 4;
    m_selection_filter = NULL;
    setCatalog("");
}


QString CatalogModel::catalog() const
{
    return m_catalog;
}

void CatalogModel::setCatalog(const QString &catalog)
{
    qDebug() << "CatalogModel::setCatalog" << catalog;

    m_rows = 0;
    m_catalog = catalog;

    updateCatalog();
}

void CatalogModel::updateCatalog()
{

    QString filterRatingStr = "";
    QString filterTagStr = "";
    QString filterCatalogStr = "";
    int catalog_id = m_db.getIdFromAttributeValue("Catalog", "name", m_catalog);

    if (m_selection_filter) {
        filterRatingStr = m_selection_filter->sqlRatingCondition();
        filterTagStr = m_selection_filter->sqlTagCondition();
    }

    if (!filterRatingStr.isEmpty())
        filterRatingStr.prepend(" AND ");

    if (!filterTagStr.isEmpty())
        filterTagStr.prepend(" AND t.name IN ");

    if (m_catalog != "All" and !m_catalog.isEmpty())
        filterCatalogStr = QString(" AND rc.catalog_id IN ('%1')").arg(m_db.getCatalogChildren(catalog_id).join("','"));

    QString select_part = "SELECT DISTINCT r.* ";
    QString from_part = "FROM Rush r "
                        "LEFT JOIN Tag t ON (r.id = t.rush_id) ";

   if (!filterCatalogStr.isEmpty())
       from_part += "LEFT JOIN RushCatalog rc ON (r.id = rc.rush_id) ";

    QString cond_part = "WHERE 1=1" + filterRatingStr + filterTagStr + filterCatalogStr;
    QString order_part = " ORDER BY r.utc_creation_time" + filterRatingStr + filterTagStr;

    QSqlQuery countQuery(m_db.sqlDatabase());
    countQuery.exec("SELECT count(DISTINCT r.id) " + from_part + cond_part );
    if (countQuery.lastError().isValid()) {
        qDebug() << "error on" <<countQuery.lastQuery() << countQuery.lastError().text();
        m_rows = 0;
    }
    else {
        countQuery.first();
        m_rows = countQuery.value(0).toLongLong();
    }

    QString querystr = select_part + from_part + cond_part + order_part;

    setQuery(querystr, m_db.sqlDatabase());
    if (lastError().isValid())
        qDebug() << "error on" << query().lastQuery() << lastError().text();

    qDebug() << "CatalogModel::setCatalog" << m_rows << query().lastQuery() << lastError().text();

    emit catalogChanged();
}

void CatalogModel::setFilter(CatalogFilter *filter)
{
    m_selection_filter = filter;
}

void CatalogModel::setThumbnailColumnNumber(int new_columns)
{
    if (new_columns == m_thumbnail_column_number)
        return;

    int old_columns = m_thumbnail_column_number;
    int old_rows = rowCount(QModelIndex());
    int new_rows = ((m_rows-1) / new_columns ) + 1;

    if (old_columns < new_columns)
        beginInsertColumns(QModelIndex(), 0, new_columns - old_columns - 1);
    else
        beginRemoveColumns(QModelIndex(), 0, old_columns - new_columns - 1);

    if (old_rows < new_rows)
        beginInsertRows(QModelIndex(), 0, new_rows - old_rows - 1);
    if (old_rows > new_rows)
        beginRemoveRows(QModelIndex(), 0, old_rows - new_rows - 1);

    //qDebug() << old_columns << new_columns;
    m_thumbnail_column_number = new_columns;

    if (old_columns < new_columns)
        endInsertColumns();
    else
        endRemoveColumns();

    if (old_rows < new_rows)
        endInsertRows();
    if (old_rows > new_rows)
        endRemoveRows();

    emit catalogChanged();
}

int CatalogModel::columnCount(const QModelIndex &) const
{
    if (m_rows < m_thumbnail_column_number)
        return m_rows;

    return m_thumbnail_column_number; // TODO change this
}

int CatalogModel::rowCount(const QModelIndex &) const
{
    //return QSqlTableModel::rowCount(index);

    if (m_rows < 1)
        return 0;

    return ((m_rows-1) / m_thumbnail_column_number ) + 1; // TODO remove hardcoded value
}

int CatalogModel::itemCount() const
{
    return m_rows;
}

QVariant CatalogModel::data(const QModelIndex &index, int role) const
{
    //qDebug() << "CatalogModel::data" << index << role;
    if (role == Qt::DisplayRole)
        return record(index.row() * columnCount(index) + index.column()).value(2); // TODO : hardcoded values !!! fix this

    if (role == Qt::UserRole + 1)
        return record(index.row() * columnCount(index) + index.column()).value(3); // TODO : hardcoded values !!! fix this

    if (role == Qt::UserRole + 2)
        return record(index.row() * columnCount(index) + index.column()).value(4); // TODO : hardcoded values !!! fix this

    return QSqlQueryModel::data(index, role);
}

void CatalogModel::updateRating(const QStringList &files, int rating)
{
    QString file_list = "";
    for (int i=0; i< files.size(); i++)
        file_list += QString("'%1',").arg(files[i]);
    file_list.chop(1);

    // update database
    QSqlQuery update_query(m_db.sqlDatabase());
    QString querystr = QString("UPDATE Rush SET rating=%1 WHERE filename in (%2)").arg(QString::number(rating), file_list);
    update_query.exec(querystr);

    qDebug() << "CatalogModel::updateRating" << update_query.lastQuery() << update_query.lastError().text();

    // TODO : optimize, update catalog will do a select query, overkill for a rating change... cache ratings or something
    updateCatalog();
}

int CatalogModel::getVideoId(const QString &filename) const
{
    qint64 rush_id = -1;
    QSqlQuery get_id_query(m_db.sqlDatabase());
    get_id_query.exec(QString("SELECT id FROM Rush WHERE filename='%1' LIMIT 1").arg(filename));
    if (get_id_query.lastError().isValid())
            qDebug() << "error on" <<get_id_query.lastQuery() << get_id_query.lastError().text();
    else {
        get_id_query.first();
        rush_id = get_id_query.value(0).toLongLong();
    }

    return rush_id;
}

Rush CatalogModel::getRush(const QString &filename) const
{
    return m_db.getVideo(filename);
}

void CatalogModel::deleteFromCatalog(const QStringList &files)
{
    QString rush_ids = "";
    for (int i=0; i< files.size(); i++)
        rush_ids += QString("'%1',").arg(getVideoId(files[i]));
    rush_ids.chop(1);

    // delete thumbnails
    // TODO

    // delete from database
    // TODO : handle catalog ?
    QSqlQuery delete_query(m_db.sqlDatabase());

    QString querystr = QString("DELETE FROM Tag WHERE rush_id in (%1)").arg(rush_ids);
    delete_query.exec(querystr);
    qDebug() << "CatalogModel::deleteFromCatalog" << delete_query.lastQuery() << delete_query.lastError().text();

    querystr = QString("DELETE FROM Rush WHERE id in (%1)").arg(rush_ids);
    delete_query.exec(querystr);

    qDebug() << "CatalogModel::deleteFromCatalog" << delete_query.lastQuery() << delete_query.lastError().text();

    updateCatalog();
}

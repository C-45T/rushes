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
    setBin("");
}


QString CatalogModel::catalog() const
{
    return m_bin_name;
}

void CatalogModel::setBin(const QString &bin_name)
{
    qDebug() << "CatalogModel::setBin" << bin_name;

    m_rows = 0;
    m_bin_name = bin_name;

    updateCatalog();
}

void CatalogModel::updateCatalog()
{

    QString filterRatingStr = "";
    QString filterTagStr = "";
    QString filterBinStr = "";
    int bin_id = m_db.getIdFromAttributeValue("Bin", "name", m_bin_name);

    if (m_selection_filter) {
        filterRatingStr = m_selection_filter->sqlRatingCondition();
        filterTagStr = m_selection_filter->sqlTagCondition();
    }

    if (!filterRatingStr.isEmpty())
        filterRatingStr.prepend(" AND ");

    if (!filterTagStr.isEmpty())
        filterTagStr.prepend(" AND t.name IN ");

    if (m_bin_name != "All" and !m_bin_name.isEmpty())
        filterBinStr = QString(" AND rb.bin_id IN ('%1')").arg(m_db.getBinChildren(bin_id).join("','"));

    QString select_part = "SELECT DISTINCT r.* ";
    QString from_part = "FROM Rush r "
                        "LEFT JOIN Tag t ON (r.id = t.rush_id) ";

   if (!filterBinStr.isEmpty())
       from_part += "LEFT JOIN RushBin rb ON (r.id = rb.rush_id) ";

    QString cond_part = "WHERE 1=1" + filterRatingStr + filterTagStr + filterBinStr;
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

    qDebug() << "CatalogModel::setBin" << m_rows << query().lastQuery() << lastError().text();

    emit catalogChanged();
}

void CatalogModel::setFilter(CatalogFilter *filter)
{
    m_selection_filter = filter;
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
    return m_db.getRush(filename);
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


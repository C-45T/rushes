#include "catalogfilter.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "data/database.h"

CatalogFilter::CatalogFilter(Database &db, QObject *parent)
    : QObject(parent),
      m_db(db)
{
    m_min_rating=0;
    m_rows=0;
    m_max_rating=Database::MAX_RATING;
    m_bin_name = "All";

    connect(this, SIGNAL(valueChanged()), this, SLOT(querySelection()));
}

QString CatalogFilter::sqlRatingCondition() const
{
    if ((m_min_rating == 0) && (m_max_rating == Database::MAX_RATING))
        return "";

    return QString("((rating >= %1) AND (rating <= %2))").arg(QString::number(m_min_rating), QString::number(m_max_rating));
}

QString CatalogFilter::sqlTagCondition() const
{
    QString condition = "";

    if (m_tag_filter.isEmpty())
        return "";

    foreach (QString tag, m_tag_filter.split(",")) {
        condition += QString("'%1',").arg(tag);
    }

    if (!condition.isEmpty())
        condition.chop(1);

    return QString("(%1)").arg(condition);
}

void CatalogFilter::setMinRating(int rating)
{
    m_min_rating = rating;

    emit valueChanged();
}

void CatalogFilter::setMaxRating(int rating)
{
    m_max_rating = rating;

    emit valueChanged();
}

void CatalogFilter::setBin(const QString &bin_name)
{
    m_bin_name = bin_name;

    emit valueChanged();
}

void CatalogFilter::setTagFilter(const QString &filter)
{
    m_tag_filter = filter;

    emit valueChanged();
}

void CatalogFilter::querySelection()
{
    QString filterRatingStr = sqlRatingCondition();
    QString filterTagStr = sqlTagCondition();
    QString filterBinStr = "";
    int bin_id = m_db.getIdFromAttributeValue("Bin", "name", m_bin_name);

    if (!filterRatingStr.isEmpty())
        filterRatingStr.prepend(" AND ");

    if (!filterTagStr.isEmpty())
        filterTagStr.prepend(" AND t.name IN ");

    if (m_bin_name != "All" and !m_bin_name.isEmpty())
        filterBinStr = QString(" AND rb.bin_id IN ('%1')").arg(m_db.getBinChildren(bin_id).join("','"));

    QString select_part = "SELECT DISTINCT r.filename ";
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

    QSqlQuery selectQuery(m_db.sqlDatabase());
    selectQuery.exec(querystr);
    if (selectQuery.lastError().isValid())
        qDebug() << "error on" << selectQuery.lastQuery() << selectQuery.lastError().text();
    else
    {
        qDebug() << "CatalogFilter::querySelection()" << m_rows <<selectQuery.lastQuery() << selectQuery.lastError().text();
        QStringList selection;
        while (selectQuery.next())
            selection.append(selectQuery.value(0).toString());

        emit selectionChanged(selection);
    }
}


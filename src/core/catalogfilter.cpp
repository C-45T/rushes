/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2017 Remy Ruttner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

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

    return QString("((e.rating >= %1) AND (e.rating <= %2))").arg(QString::number(m_min_rating), QString::number(m_max_rating));
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

QString CatalogFilter::bin() const
{
    return m_bin_name;
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
        filterBinStr = QString(" AND eb.bin_id IN ('%1')").arg(m_db.getBinChildren(bin_id).join("','"));

    QString select_part = "SELECT DISTINCT r.file_name, e.id ";
    QString from_part = "FROM Rush r "
                        "JOIN Extract e ON (e.rush_id = r.id) "
                        "LEFT JOIN Tag t ON (e.id = t.extract_id) ";

   if (!filterBinStr.isEmpty())
       from_part += "LEFT JOIN ExtractBin eb ON (e.id = eb.extract_id) ";

    QString cond_part = "WHERE 1=1" + filterRatingStr + filterTagStr + filterBinStr;
    QString order_part = " ORDER BY r.utc_creation_time" + filterRatingStr + filterTagStr;

    QSqlQuery countQuery(m_db.sqlDatabase());
    countQuery.exec("SELECT count(DISTINCT e.id) " + from_part + cond_part );
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
        QList<qint64> extract_selection;
        while (selectQuery.next())
        {
            QString rush_file_name = selectQuery.value(0).toString();
            qint64 extract_id = selectQuery.value(1).toInt();

            if (!selection.contains(rush_file_name))
                selection.append(rush_file_name);

            extract_selection.append(extract_id);
        }

        emit selectionChanged(selection, extract_selection);
    }
}


#include "catalogfilter.h"

#include "database.h"

CatalogFilter::CatalogFilter(QObject *parent)
    : QObject(parent)
{
    m_min_rating=0;
    m_max_rating=Database::MAX_RATING;
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

void CatalogFilter::setTagFilter(const QString &filter)
{
    m_tag_filter = filter;

    emit valueChanged();
}


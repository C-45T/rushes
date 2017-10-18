#ifndef CATALOGFILTER_H
#define CATALOGFILTER_H

#include <QObject>
#include <QString>

class CatalogFilter : public QObject
{
    Q_OBJECT

public:
    CatalogFilter(QObject *parent);

    QString sqlRatingCondition() const;
    QString sqlTagCondition() const;

public slots:
    void setMinRating(int rating);
    void setMaxRating(int rating);

    void setTagFilter(const QString& filter);

signals:
    void valueChanged();

private:
    int m_min_rating;
    int m_max_rating;

    QString m_tag_filter;
};

#endif // CATALOGFILTER_H

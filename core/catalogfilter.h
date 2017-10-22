#ifndef CATALOGFILTER_H
#define CATALOGFILTER_H

#include <QObject>
#include <QString>

#include "data/database.h"

class CatalogFilter : public QObject
{
    Q_OBJECT

public:
    CatalogFilter(Database &db, QObject *parent);

    QString sqlRatingCondition() const;
    QString sqlTagCondition() const;
    QString bin() const;

public slots:
    void setMinRating(int rating);
    void setMaxRating(int rating);
    void setBin(const QString& bin_name);

    void setTagFilter(const QString& filter);

    void querySelection();

signals:
    void valueChanged();
    void selectionChanged(const QStringList& files);

private:
    int m_min_rating;
    int m_max_rating;

    QString m_tag_filter;
    QString m_bin_name;
    Database &m_db;
    int m_rows;
};

#endif // CATALOGFILTER_H

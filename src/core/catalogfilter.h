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

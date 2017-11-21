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

#ifndef CATALOGFILTERWIDGET_H
#define CATALOGFILTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>

#include "core/catalogfilter.h"
#include "thirdParty/stareditor.h"

class CatalogFilterWidget : public QWidget
{
public:
    CatalogFilterWidget(QWidget *parent = 0);

    void setFilter(CatalogFilter *filter);

private:
    StarEditor *m_min_rating_editor;
    StarEditor *m_max_rating_editor;
    QLineEdit *m_tags_edit;
    CatalogFilter *m_filter;
};

#endif // CATALOGFILTERWIDGET_H

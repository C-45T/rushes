/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) %YEAR% Remy Ruttner
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

#include "catalogfilterwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

#include "data/database.h"

CatalogFilterWidget::CatalogFilterWidget(QWidget *parent)
    : QWidget(parent)
{
    m_min_rating_editor = new StarEditor(this);
    m_min_rating_editor->setStarRating(0);
    m_max_rating_editor = new StarEditor(this);
    m_max_rating_editor->setStarRating(5);

    m_tags_edit = new QLineEdit(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel(tr("Min Rating")));
    layout->addWidget(m_min_rating_editor);
    layout->addWidget(new QLabel(tr("Max Rating")));
    layout->addWidget(m_max_rating_editor);
    layout->addWidget(new QLabel(tr("Tags")));
    layout->addWidget(m_tags_edit);
    //layout->addStretch(1);
}

void CatalogFilterWidget::setFilter(CatalogFilter *filter)
{
    m_filter = filter;

    connect(m_min_rating_editor, SIGNAL(ratingChanged(int)), m_filter, SLOT(setMinRating(int)));
    connect(m_max_rating_editor, SIGNAL(ratingChanged(int)), m_filter, SLOT(setMaxRating(int)));
    connect(m_tags_edit, SIGNAL(textChanged(QString)), m_filter, SLOT(setTagFilter(QString)));
}

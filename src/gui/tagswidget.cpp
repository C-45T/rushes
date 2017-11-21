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

#include "tagswidget.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>

TagsWidget::TagsWidget(QWidget *parent) : QWidget(parent)
{
    m_layout = new FlowLayout(this);
}

void TagsWidget::setTags(const QStringList &tags)
{
    //QString label_text = "";
    //m_layout->clear();

    foreach (QLabel *label, m_labels)
        label->deleteLater();
    m_labels.clear();
    m_layout->clear();

    foreach (QString tag, tags)
    {
        QLabel *tag_lbl = new QLabel();
        tag_lbl->setText(tag);
        tag_lbl->setProperty("isTag", true);

        m_layout->addWidget(tag_lbl);
        m_labels.append(tag_lbl);
        //tag_lbl->setTextFormat(Qt::RichText);

        //label_text .append("<span style=""font-size:14px;padding:10px;color:white;font-weight:bold;background-color:green"">");
        //label_text .append(tag);
        //label_text .append("</span> ");
    }

    //m_tags_lbl->setText(label_text);
}

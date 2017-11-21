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

#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QLabel>

#include "thirdParty/flowlayout.h"

class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QWidget *parent = nullptr);

    void setTags(const QStringList& tags);

signals:

public slots:

private:
    FlowLayout *m_layout;
    QList<QLabel*> m_labels;
};

#endif // TAGSWIDGET_H

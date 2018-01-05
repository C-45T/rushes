/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2018 Remy Ruttner
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
#ifndef MARKEDSLIDER_H
#define MARKEDSLIDER_H

#include <QSLider>

#include "data/extract.h"

class MarkedSlider : public QSlider
{
    Q_OBJECT
public:
    explicit MarkedSlider(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *);

    void setExtract(Extract *extract);

signals:

public slots:

private:
    Extract *m_extract;

};

#endif // MARKEDSLIDER_H

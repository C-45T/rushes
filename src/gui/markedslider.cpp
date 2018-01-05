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
#include "markedslider.h"

#include <QPainter>
#include <QStyleOptionSlider>

MarkedSlider::MarkedSlider(QWidget *parent) : QSlider(parent)
{
    m_extract = 0;
}

void MarkedSlider::paintEvent(QPaintEvent *e)
{
    QSlider::paintEvent(e);

    if (m_extract != 0 and m_extract->rush() != 0)
    {
        int start_time_x = m_extract->start() * width() / m_extract->rush()->length;
        int stop_time_x = m_extract->stop() * width() / m_extract->rush()->length;;
        int key_time_x = m_extract->key_moment() * width() / m_extract->rush()->length;;

        QPainter painter(this);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 50));
        painter.drawRect(start_time_x, 0, stop_time_x - start_time_x, height());

        painter.setPen(QColor(Qt::blue));
        painter.drawLine(QPoint(start_time_x,0), QPoint(start_time_x,height()));

        painter.setPen(QColor(Qt::green));
        painter.drawLine(QPoint(stop_time_x,0), QPoint(stop_time_x,height()));

        painter.setPen(QColor(Qt::yellow));
        painter.drawLine(QPoint(key_time_x,0), QPoint(key_time_x,height()));
    }
}

void MarkedSlider::setExtract(Extract *extract)
{
    m_extract = extract;
}

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

#ifndef RUSHINFOWIDGET_H
#define RUSHINFOWIDGET_H

#include <QWidget>
#include <QLabel>

#include "data/rush.h"

class RushInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RushInfoWidget(QWidget *parent = nullptr);

    void setRush(const Rush& rush);

private:
    QLabel *m_name_lbl;
    QLabel *m_path_lbl;
    QLabel *m_length_lbl;
    QLabel *m_resolution_lbl;
    QLabel *m_vcodec_lbl;
    QLabel *m_acodec_lbl;
    QLabel *m_date_lbl;

};

#endif // RUSHINFOWIDGET_H

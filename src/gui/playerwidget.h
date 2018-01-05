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

#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

#include <QtAV>
#include <QtAVWidgets>

#include "gui/markedslider.h"

#include "data/extract.h"

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);

    Extract *extract() const;
    void setExtract(Extract *extract);

signals:
    void keyTimeChanged(Extract *extract);

public slots:
    void openMedia(const QString& filename);
    void openMedia(Extract *extract);
    void openMedia(Rush *rush);
    void seekBySlider(int value);
    void seekBySlider();
    void playPause();

private slots:
    void updateSlider(qint64 value);
    void updateSlider();
    void updateSliderUnit();
    void onVolumeChanged();

private:
    QtAV::VideoOutput *m_vo;
    QtAV::AVPlayer *m_player;
    MarkedSlider *m_slider;
    int m_unit;
    int m_fps;

    QLabel *m_play_time;
    QLabel *m_volume_label;
    QTimer m_hide_volume_label_timer;

    Extract *m_extract;
};

#endif // PLAYERWIDGET_H

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

#include "playerwidget.h"

#include <QVBoxLayout>
#include <QMessageBox>

using namespace QtAV;

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
    QtAV::Widgets::registerRenderers();

    m_unit = 1000;
    m_fps = 25;

    m_player = new AVPlayer(this);
    QVBoxLayout *vl = new QVBoxLayout();
    setLayout(vl);
    m_vo = new VideoOutput(this);
    if (!m_vo->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
        return;
    }
    m_player->setRenderer(m_vo);

    vl->addWidget(m_vo->widget());
    m_slider = new QSlider();
    m_slider->setOrientation(Qt::Horizontal);
    connect(m_slider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(m_slider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));
    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(m_player, SIGNAL(started()), SLOT(updateSlider()));  

    vl->addWidget(m_slider);

    QFont volume_font("Tahoma", 20, QFont::Bold);
    QFontMetrics fm(volume_font);

    m_volume_label = new QLabel(this);
    m_volume_label->move(20, 20);
    m_volume_label->resize(fm.width("Volume : 100"), fm.height());
    m_volume_label->setText("Volume : " + QString::number(m_player->audio()->volume()*100));
    m_volume_label->setFont(volume_font);
    m_volume_label->setStyleSheet("color:white");
    m_volume_label->hide();
    connect(m_player->audio(), SIGNAL(volumeChanged(qreal)), this, SLOT(onVolumeChanged()));

    m_hide_volume_label_timer.setSingleShot(true);
    connect(&m_hide_volume_label_timer, SIGNAL(timeout()), m_volume_label, SLOT(hide()));

    setMinimumHeight(150);

}

void PlayerWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "key pressed" << m_player->isPlaying();

    if (event->key() == Qt::Key_Space)
    {
        qDebug() << "play pause";
        playPause();
    }

    if (m_player->isPaused())
    {
        int one_frame_time = 1000 / m_fps;

        if (event->key() == Qt::Key_Right)
            m_player->seek(m_player->position() + one_frame_time);

        if (event->key() == Qt::Key_Left)
            m_player->seek(m_player->position() - one_frame_time);
    }
}

void PlayerWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //scrollWithPixels(numPixels);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        qreal volume = m_player->audio()->volume();
        volume = qMin(volume + numSteps.ry() * 5 / 100.0, 1.0);
        volume = qMax(volume, 0.0);
        m_player->audio()->setVolume(volume);
    }

    event->accept();
}

void PlayerWidget::mousePressEvent(QMouseEvent *)
{
    playPause();
}

void PlayerWidget::openMedia(const QString& filename)
{
    if (filename.isEmpty())
        return;
    m_player->play(filename);
}

void PlayerWidget::seekBySlider(int value)
{
    if (!m_player->isPlaying())
        return;
    m_player->seek(qint64(value*m_unit));
}

void PlayerWidget::seekBySlider()
{
    seekBySlider(m_slider->value());
}

void PlayerWidget::playPause()
{
    if (!m_player->isPlaying()) {
        m_player->play();
        return;
    }
    m_player->pause(!m_player->isPaused());
}

void PlayerWidget::updateSlider(qint64 value)
{
    m_slider->setRange(0, int(m_player->duration()/m_unit));
    m_slider->setValue(int(value/m_unit));
}

void PlayerWidget::updateSlider()
{
    updateSlider(m_player->position());
}

void PlayerWidget::updateSliderUnit()
{
    m_unit = m_player->notifyInterval();
    updateSlider();
}

void PlayerWidget::onVolumeChanged()
{
    m_volume_label->show();
    m_volume_label->setText("Volume : " + QString::number(m_player->audio()->volume()*100));
    m_hide_volume_label_timer.start(2000);
}

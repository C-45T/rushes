#include "playerwidget.h"

#include <QVBoxLayout>
#include <QMessageBox>

using namespace QtAV;

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
    m_unit = 1000;

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
        int one_frame_time = 1000 / 25;

        if (event->key() == Qt::Key_Right)
            m_player->seek(m_player->position() + one_frame_time);

        if (event->key() == Qt::Key_Left)
            m_player->seek(m_player->position() - one_frame_time);
    }
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

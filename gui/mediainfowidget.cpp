#include "mediainfowidget.h"

#include <QFormLayout>
#include <QFileInfo>
#include <QTime>

MediaInfoWidget::MediaInfoWidget(QWidget *parent) : QWidget(parent)
{
    QFormLayout *layout = new QFormLayout(this);

    m_name_lbl = new QLabel(this);
    m_length_lbl = new QLabel(this);
    m_resolution_lbl = new QLabel(this);
    m_vcodec_lbl = new QLabel(this);
    m_acodec_lbl = new QLabel(this);
    m_date_lbl = new QLabel(this);

    layout->addRow("file", m_name_lbl);
    layout->addRow("length", m_length_lbl);
    layout->addRow("resolution", m_resolution_lbl);
    layout->addRow("video codec", m_vcodec_lbl);
    layout->addRow("audio codec", m_acodec_lbl);
    layout->addRow("date", m_date_lbl);

}

void MediaInfoWidget::setMediaInfo(const MediaInfo &info)
{
    info.debug();

    QFileInfo file_info = QFileInfo(info.filename);
    QTime length(0, 0, 0);
    length = length.addSecs(info.length);

    QDateTime creation_date = QDateTime::fromSecsSinceEpoch(info.utc_creation_time);

    m_name_lbl->setText(file_info.baseName());
    m_length_lbl->setText(length.toString("hh:mm:ss"));
    m_resolution_lbl->setText(QString::number(info.width) + "x" + QString::number(info.height));
    m_vcodec_lbl->setText(info.video_codec + "\n" + QString::number(info.bitrate / 1000.0) + "Mb/s");
    m_acodec_lbl->setText(info.audio_codec + "\n" + QString::number(info.audio_bitrate) + "Kb/s");
    m_date_lbl->setText(creation_date.toString("dd/MM/yyyy - hh:mm:ss"));
}

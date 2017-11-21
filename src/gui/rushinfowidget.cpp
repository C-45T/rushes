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

#include "rushinfowidget.h"

#include <QFormLayout>
#include <QFileInfo>
#include <QTime>

RushInfoWidget::RushInfoWidget(QWidget *parent) : QWidget(parent)
{
    QFormLayout *layout = new QFormLayout(this);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    m_name_lbl = new QLabel(this);
    m_path_lbl = new QLabel(this);
    m_length_lbl = new QLabel(this);
    m_resolution_lbl = new QLabel(this);
    m_vcodec_lbl = new QLabel(this);
    m_acodec_lbl = new QLabel(this);
    m_date_lbl = new QLabel(this);

    m_name_lbl->setStyleSheet("font-weight: bold");

    layout->addRow(tr("file :"), m_name_lbl);
    layout->addRow(tr("path :"), m_path_lbl);
    layout->addRow(tr("length :"), m_length_lbl);
    layout->addRow(tr("resolution :"), m_resolution_lbl);
    layout->addRow(tr("video codec :"), m_vcodec_lbl);
    layout->addRow(tr("audio codec :"), m_acodec_lbl);
    layout->addRow(tr("creation date :"), m_date_lbl);

}

void RushInfoWidget::setRush(const Rush &rush)
{
    //info.debug();
    if (rush.file_name.isEmpty())
        return;

    QFileInfo file_info = QFileInfo(rush.file_name);
    QTime length(0, 0, 0);
    length = length.addSecs(rush.length);

    QDateTime creation_date = QDateTime::fromSecsSinceEpoch(rush.utc_creation_time);

    m_name_lbl->setText(file_info.fileName());
    m_path_lbl->setText(file_info.absolutePath());
    m_length_lbl->setText(length.toString("hh:mm:ss"));
    m_resolution_lbl->setText(QString("%1x%2 @ %3fps").arg(QString::number(rush.width), QString::number(rush.height), QString::number(rush.fps)));
    m_vcodec_lbl->setText(rush.video_codec + " ( " + QString::number(rush.bitrate / 1000.0) + " Mb/s )");
    m_acodec_lbl->setText(rush.audio_codec + " ( " + QString::number(rush.audio_bitrate) + " Kb/s )");
    m_date_lbl->setText(creation_date.toString("dd/MM/yyyy - hh:mm:ss"));
}

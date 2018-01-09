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
#include "autoeditdialog.h"

#include <QVBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFileDialog>

AutoEditDialog::AutoEditDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QFormLayout *form = new QFormLayout();
    layout->addLayout(form);

    m_export_prores = new QCheckBox();
    m_export_file_name = new QLineEdit("rushes_auto_edit.fcpxml");
    m_min_duration_time = new QLineEdit("500");
    m_export_folder_btn = new QPushButton("...");
    m_music_file_btn = new QPushButton("...");
    m_match_music_length = new QCheckBox();


    form->addRow("Export files to prores", m_export_prores);
    form->addRow("Music file", m_music_file_btn);
    form->addRow("Export folder", m_export_folder_btn);
    form->addRow("Export file name", m_export_file_name);
    form->addRow("Clips minimum duration (ms)", m_min_duration_time);
    form->addRow("Match music length", m_match_music_length);

    m_min_duration_time->setEnabled(false);
    m_match_music_length->setEnabled(false);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    layout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_export_folder_btn, SIGNAL(clicked(bool)), this, SLOT(chooseExportFolder()));
    connect(m_music_file_btn, SIGNAL(clicked(bool)), this, SLOT(chooseMusicFile()));
}

bool AutoEditDialog::exportProres() const
{
    return m_export_prores->isChecked();
}

QString AutoEditDialog::exportFolder() const
{
    return m_export_folder_btn->text();
}

QString AutoEditDialog::exportFileName() const
{
    return QFileInfo(exportFolder(), m_export_file_name->text()).absoluteFilePath();
}

QString AutoEditDialog::musicFileName() const
{
    return m_music_file_btn->text();
}

void AutoEditDialog::chooseExportFolder()
{
    QString output_folder = QFileDialog::getExistingDirectory();
    if (!output_folder.isEmpty())
    {
        m_export_folder_btn->setText(output_folder);

    }
}

void AutoEditDialog::chooseMusicFile()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Pick a song");

    if (!file_name.isEmpty())
         m_music_file_btn->setText(file_name);
}

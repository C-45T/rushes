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
#ifndef AUTOEDITDIALOG_H
#define AUTOEDITDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>

class AutoEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AutoEditDialog(QWidget *parent = nullptr);

    bool exportProres() const;
    QString exportFolder() const;
    QString exportFileName() const;
    QString musicFileName() const;

signals:

protected slots:
    void chooseExportFolder();
    void chooseMusicFile();

private:
    QPushButton *m_export_folder_btn;
    QPushButton *m_music_file_btn;
    QLineEdit *m_export_file_name;
    QLineEdit *m_min_duration_time;
    QCheckBox *m_match_music_length;
    QCheckBox *m_export_prores;
};

#endif // AUTOEDITDIALOG_H

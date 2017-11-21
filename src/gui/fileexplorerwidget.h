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

#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>

#include "thumbnailview.h"
#include "data/database.h"

class FileExplorerWidget : public QSplitter
{
    Q_OBJECT
public:
    FileExplorerWidget(Database &db, QWidget *parent = 0);
    ~FileExplorerWidget();

    QString currentDirectory() const;

    ThumbnailView *view() const;

protected:
    void writeSettings();
    void readSettings();

protected slots:
    void onDirectoryChanged(const QModelIndex &index);

private:
    QTreeView *m_tree_view;
    ThumbnailView *m_thumbnail_view;
    QFileSystemModel *m_model;
    Database &m_db;
    QDir m_current_dir;
};

#endif // FILEEXPLORERWIDGET_H

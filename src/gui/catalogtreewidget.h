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

#ifndef CATALOGLISTWIDGET_H
#define CATALOGLISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "data/database.h"

class CatalogTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogTreeWidget(Database *db, QWidget *parent = nullptr);

    void selectBin(const QString& bin_name);

signals:
    void binSelected(const QString& catalog_name);

public slots:
    void addBin();
    void deleteBin();
    void itemClicked(QTreeWidgetItem*item, int);
    void updateTree();

protected:
    void addNodes(QTreeWidgetItem *parent = NULL, const QString& parent_name = QString());

private:
    QTreeWidget *m_tree;
    Database *m_db;
    QString m_last_selected_item;
};

#endif // CATALOGLISTWIDGET_H

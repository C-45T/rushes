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

#include "catalogtreewidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

CatalogTreeWidget::CatalogTreeWidget(Database *db, QWidget *parent)
    : QWidget(parent),
      m_db(db)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_tree = new QTreeWidget(this);
    QPushButton *add_btn = new QPushButton(tr("Add Catalog"));
    QPushButton *delete_btn = new QPushButton(tr("Delete Catalog"));

    layout->addWidget(m_tree);
    layout->addWidget(add_btn);
    layout->addWidget(delete_btn);

    m_tree->setColumnCount(1);
    m_tree->setHeaderLabel(tr("Catalogs"));
    m_last_selected_item = "All"; // TODO : hardcoded value
    updateTree();

    connect(add_btn, SIGNAL(clicked(bool)), this, SLOT(addBin()));
    connect(delete_btn, SIGNAL(clicked(bool)), this, SLOT(deleteBin()));
    connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));
}

void CatalogTreeWidget::selectBin(const QString &bin_name)
{
    m_last_selected_item = bin_name;
    emit binSelected(m_last_selected_item);
    updateTree();
}

void CatalogTreeWidget::addBin()
{
    QString catalog_name = QInputDialog::getText(this, "Enter Catalog name", "Name");
    if (!catalog_name.isEmpty())
    {
        QString selected_item_name = QString();
        if (m_tree->selectedItems().size() > 0)
                selected_item_name = m_tree->selectedItems().first()->text(0);
        m_db->addBin(catalog_name, selected_item_name);
        m_last_selected_item = selected_item_name;
        updateTree();
    }
}

void CatalogTreeWidget::deleteBin()
{
    QString selected_item_name = QString();
    // check if an item is selected and it's not root item
    if (m_tree->selectedItems().size() > 0 && m_tree->selectedItems().first()->parent())
    {
            selected_item_name = m_tree->selectedItems().first()->text(0);
            m_last_selected_item = m_tree->selectedItems().first()->parent()->text(0);
    }
    else
    {
        return;
    }

    if (QMessageBox::question(this,
                              tr("Delete catalog"),
                              QString(tr("Are you sure you want to delete catalog %1 and all its children ?")).arg(selected_item_name))
            == QMessageBox::Yes)
    {
        m_db->deleteBin(selected_item_name);
        updateTree();
    }
}

void CatalogTreeWidget::itemClicked(QTreeWidgetItem *item, int)
{
    m_last_selected_item = item->text(0);
    emit binSelected(m_last_selected_item);
}

void CatalogTreeWidget::updateTree()
{
    m_tree->clear();
    addNodes();
}

void CatalogTreeWidget::addNodes(QTreeWidgetItem *parent, const QString &parent_name)
{
    QStringList catalogs = m_db->bins(parent_name);
    for (int i=0; i< catalogs.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent, QStringList(catalogs[i]));
        if (parent == NULL)
            m_tree->addTopLevelItem(item);
        else
            parent->addChild(item);

        // set back item selection state
        if (!m_last_selected_item.isEmpty() && catalogs[i] == m_last_selected_item)
        {
            item->setSelected(true);
            item->setExpanded(true);
            if (parent)
                parent->setExpanded(true);
        }

        addNodes(item, catalogs[i]);

        if (item->isExpanded() && parent)
            parent->setExpanded(true);
    }
}

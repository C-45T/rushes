#ifndef CATALOGLISTWIDGET_H
#define CATALOGLISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "database.h"

class CatalogTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CatalogTreeWidget(Database *db, QWidget *parent = nullptr);

signals:
    void catalogSelected(const QString& catalog_name);

public slots:
    void addCatalog();   
    void deleteCatalog();
    void itemClicked(QTreeWidgetItem*item, int);

protected:
    void updateTree();
    void addNodes(QTreeWidgetItem *parent = NULL, const QString& parent_name = QString());

private:
    QTreeWidget *m_tree;
    Database *m_db;
    QString m_last_selected_item;
};

#endif // CATALOGLISTWIDGET_H

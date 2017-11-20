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

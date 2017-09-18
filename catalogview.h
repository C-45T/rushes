#ifndef CATALOGVIEW_H
#define CATALOGVIEW_H

#include <QTableView>

class CatalogView : public QTableView
{
public:
    CatalogView(QWidget* parent = 0);

    //void setModel(QAbstractItemModel *model);

    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

    QStringList selectedFiles() const;

};

#endif // CATALOGVIEW_H

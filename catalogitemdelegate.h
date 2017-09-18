#ifndef CATALOGITEMDELEGATE_H
#define CATALOGITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QWidget>

class CatalogItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CatalogItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:

public slots:

};

#endif // CATALOGITEMDELEGATE_H

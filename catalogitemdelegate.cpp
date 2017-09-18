#include "catalogitemdelegate.h"

#include <QPainter>
#include <QDebug>

CatalogItemDelegate::CatalogItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void CatalogItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else
        painter->fillRect(option.rect, QColor("darkGray").darker());

    if (index.data().isNull())
        return;

    QImage thumbnail = QImage(index.data(Qt::UserRole + 1).toString());
    painter->drawImage(option.rect.adjusted(2, 2, 2, -52), thumbnail );

    //qDebug() << index.row() << index.column() << index.data().toString() << index.data(Qt::UserRole + 1).toString();

    QRect rectangle = option.rect.adjusted(0, 240, -10, 0);
    painter->setFont(QFont("Arial", 72, QFont::Bold ));
    painter->setPen(QColor("gray"));

    painter->drawText(rectangle, Qt::AlignRight, index.data(Qt::UserRole + 2).toString());

    rectangle = option.rect.adjusted(0, 270, 0, 0);
    painter->setFont(QFont("Arial", 14, QFont::Bold ));
    painter->setPen(QColor("white"));
    painter->drawText(rectangle, Qt::AlignCenter, index.data().toString().split("/").last());
}

QSize CatalogItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(480, 320);
}

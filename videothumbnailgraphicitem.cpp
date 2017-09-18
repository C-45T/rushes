#include "videothumbnailgraphicitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include <QDebug>

VideoThumbnailGraphicItem::VideoThumbnailGraphicItem(const QSqlRecord& record)
    : QGraphicsItem()
{
    m_record = record;
    m_thumbnail = QImage(m_record.value(3).toString());
    m_mouse_pressed = false;

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
}

QRectF VideoThumbnailGraphicItem::boundingRect() const
{
    return QRectF(0, 0, 480, 320); // TODO : remove hardcoded values
}

void VideoThumbnailGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (option->state & QStyle::State_Selected)
        painter->fillRect(option->rect, option->palette.highlight());
    else
        painter->fillRect(option->rect, QColor("darkGray").darker());

    if (m_record.value(1).isNull())
        return;

    painter->drawImage(option->rect.adjusted(2, 2, -2, -52), m_thumbnail ); // TODO : remove hardcoded values

    //qDebug() << index.row() << index.column() << index.data().toString() << index.data(Qt::UserRole + 1).toString();

    QRect rectangle = option->rect.adjusted(0, 240, -10, 0); // TODO : remove hardcoded values
    painter->setFont(QFont("Arial", 72, QFont::Bold ));
    painter->setPen(QColor("gray"));

    painter->drawText(rectangle, Qt::AlignRight, m_record.value(4).toString());

    rectangle = option->rect.adjusted(0, 270, 0, 0); // TODO : remove hardcoded values
    painter->setFont(QFont("Arial", 14, QFont::Bold ));
    painter->setPen(QColor("white"));
    painter->drawText(rectangle, Qt::AlignCenter, m_record.value(2).toString().split("/").last());
}

int VideoThumbnailGraphicItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}

QString VideoThumbnailGraphicItem::itemData() const
{
    return m_record.value(2).toString();
}

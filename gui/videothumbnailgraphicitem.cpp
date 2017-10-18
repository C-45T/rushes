#include "videothumbnailgraphicitem.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include <QDebug>

VideoThumbnailGraphicItem::VideoThumbnailGraphicItem(const MediaInfo &info)
    : QGraphicsItem()
{
    m_media_info = info;
    m_thumbnail = QImage(m_media_info.thumbnail_filename);
    m_mouse_pressed = false;

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);

    m_rating = StarRating(m_media_info.rating, 5);
}

QRectF VideoThumbnailGraphicItem::boundingRect() const
{
    return QRectF(0, 0, 480, 320); // TODO : remove hardcoded values
}

void VideoThumbnailGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // set backgroung color
    if (option->state & QStyle::State_Selected)
        painter->fillRect(option->rect, option->palette.highlight());
    else
        painter->fillRect(option->rect, QColor(Qt::gray).darker(300));

    if (m_media_info.filename.isEmpty())
        return;

    // paint thumbnail
    painter->drawImage(option->rect.adjusted(0, 0, 0, -50), m_thumbnail ); // TODO : remove hardcoded values

    //qDebug() << index.row() << index.column() << index.data().toString() << index.data(Qt::UserRole + 1).toString();

//    // draw rating
//    QRect rectangle = option->rect.adjusted(0, 240, -10, 0); // TODO : remove hardcoded values
//    painter->setFont(QFont("Arial", 72, QFont::Bold ));
//    painter->setPen(QColor("gray"));
//    painter->drawText(rectangle, Qt::AlignRight, QString::number(m_media_info.rating));

    // draw filename
    QRect rectangle = option->rect.adjusted(0, 270, 0, 0); // TODO : remove hardcoded values
    painter->setFont(QFont("Arial", 16, QFont::Bold ));
    painter->setPen(Qt::white);
    painter->drawText(rectangle, Qt::AlignCenter, m_media_info.filename.split("/").last());

    // draw stars
    m_rating.paint(painter, option->rect.adjusted(5, 0, 0, -270), option->palette, StarRating::ReadOnly);
}

int VideoThumbnailGraphicItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}

const MediaInfo &VideoThumbnailGraphicItem::itemData() const
{
    return m_media_info;
}

#ifndef VIDEOTHUMBNAILGRAPHICITEM_H
#define VIDEOTHUMBNAILGRAPHICITEM_H

#include <QGraphicsItem>

#include "thirdParty/starrating.h"
#include "data/rush.h"

class VideoThumbnailGraphicItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    VideoThumbnailGraphicItem(const Rush& rush);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

    int type() const;


    const Rush& itemData() const;

private:
    Rush m_rush;
    QImage m_thumbnail;

    StarRating m_rating;

    bool m_mouse_pressed;
};

#endif // VIDEOTHUMBNAILGRAPHICITEM_H

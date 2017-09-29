#ifndef VIDEOTHUMBNAILGRAPHICITEM_H
#define VIDEOTHUMBNAILGRAPHICITEM_H

#include <QGraphicsItem>

#include "thirdParty/starrating.h"
#include "mediainfo.h"

class VideoThumbnailGraphicItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    VideoThumbnailGraphicItem(const MediaInfo& info);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

    int type() const;


    const MediaInfo& itemData() const;

private:
    MediaInfo m_media_info;
    QImage m_thumbnail;

    StarRating m_rating;

    bool m_mouse_pressed;
};

#endif // VIDEOTHUMBNAILGRAPHICITEM_H

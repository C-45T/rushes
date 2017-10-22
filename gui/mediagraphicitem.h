#ifndef MEDIAGRAPHICITEM_H
#define MEDIAGRAPHICITEM_H

#include <QGraphicsItem>

#include "thirdParty/starrating.h"
#include "data/rush.h"

class MediaGraphicItem : public QGraphicsItem
{
public:
    MediaGraphicItem(Rush *rush);
    ~MediaGraphicItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

    Rush *rush();

private:
    Rush *m_rush;
    StarRating m_rating;
};

#endif // MEDIAGRAPHICITEM_H

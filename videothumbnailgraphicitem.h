#ifndef VIDEOTHUMBNAILGRAPHICITEM_H
#define VIDEOTHUMBNAILGRAPHICITEM_H

#include <QGraphicsItem>
#include <QSqlRecord>

class VideoThumbnailGraphicItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    VideoThumbnailGraphicItem(const QSqlRecord& record);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int type() const;

    QString itemData() const;

private:
    QSqlRecord m_record;
    QImage m_thumbnail;

    bool m_mouse_pressed;
};

#endif // VIDEOTHUMBNAILGRAPHICITEM_H

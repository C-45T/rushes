#ifndef MEDIAINFOWIDGET_H
#define MEDIAINFOWIDGET_H

#include <QWidget>
#include <QLabel>

#include "mediainfo.h"

class MediaInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MediaInfoWidget(QWidget *parent = nullptr);

    void setMediaInfo(const MediaInfo& info);

private:
    QLabel *m_name_lbl;
    QLabel *m_length_lbl;
    QLabel *m_resolution_lbl;
    QLabel *m_vcodec_lbl;
    QLabel *m_acodec_lbl;
    QLabel *m_date_lbl;

};

#endif // MEDIAINFOWIDGET_H

#ifndef RUSHINFOWIDGET_H
#define RUSHINFOWIDGET_H

#include <QWidget>
#include <QLabel>

#include "data/rush.h"

class RushInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RushInfoWidget(QWidget *parent = nullptr);

    void setRush(const Rush& rush);

private:
    QLabel *m_name_lbl;
    QLabel *m_path_lbl;
    QLabel *m_length_lbl;
    QLabel *m_resolution_lbl;
    QLabel *m_vcodec_lbl;
    QLabel *m_acodec_lbl;
    QLabel *m_date_lbl;

};

#endif // RUSHINFOWIDGET_H

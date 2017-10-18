#ifndef CATALOGFILTERWIDGET_H
#define CATALOGFILTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>

#include "core/catalogfilter.h"

class CatalogFilterWidget : public QWidget
{
public:
    CatalogFilterWidget(QWidget *parent = 0);

    void setFilter(CatalogFilter *filter);

private:
    QSlider *m_rating_slider;
    QLineEdit *m_tags_edit;
    CatalogFilter *m_filter;
};

#endif // CATALOGFILTERWIDGET_H

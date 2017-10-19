#ifndef CATALOGFILTERWIDGET_H
#define CATALOGFILTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLineEdit>

#include "core/catalogfilter.h"
#include "thirdParty/stareditor.h"

class CatalogFilterWidget : public QWidget
{
public:
    CatalogFilterWidget(QWidget *parent = 0);

    void setFilter(CatalogFilter *filter);

private:
    StarEditor *m_min_rating_editor;
    StarEditor *m_max_rating_editor;
    QLineEdit *m_tags_edit;
    CatalogFilter *m_filter;
};

#endif // CATALOGFILTERWIDGET_H

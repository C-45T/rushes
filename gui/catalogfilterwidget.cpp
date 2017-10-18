#include "catalogfilterwidget.h"

#include <QHBoxLayout>
#include "data/database.h"

CatalogFilterWidget::CatalogFilterWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_rating_slider = new QSlider(Qt::Horizontal, this);
    m_rating_slider->setRange(0, Database::MAX_RATING);

    m_tags_edit = new QLineEdit(this);

    layout->addWidget(m_rating_slider);
    layout->addWidget(m_tags_edit);
    layout->addStretch(1);
}

void CatalogFilterWidget::setFilter(CatalogFilter *filter)
{
    m_filter = filter;

    connect(m_rating_slider, SIGNAL(valueChanged(int)), m_filter, SLOT(setMinRating(int)));
    connect(m_tags_edit, SIGNAL(textChanged(QString)), m_filter, SLOT(setTagFilter(QString)));
}

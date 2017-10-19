#include "catalogfilterwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>

#include "data/database.h"

CatalogFilterWidget::CatalogFilterWidget(QWidget *parent)
    : QWidget(parent)
{
    m_min_rating_editor = new StarEditor(this);
    m_min_rating_editor->setStarRating(0);
    m_max_rating_editor = new StarEditor(this);
    m_max_rating_editor->setStarRating(5);

    m_tags_edit = new QLineEdit(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel(tr("Min Rating")));
    layout->addWidget(m_min_rating_editor);
    layout->addWidget(new QLabel(tr("Max Rating")));
    layout->addWidget(m_max_rating_editor);
    layout->addWidget(new QLabel(tr("Tags")));
    layout->addWidget(m_tags_edit);
    //layout->addStretch(1);
}

void CatalogFilterWidget::setFilter(CatalogFilter *filter)
{
    m_filter = filter;

    connect(m_min_rating_editor, SIGNAL(ratingChanged(int)), m_filter, SLOT(setMinRating(int)));
    connect(m_max_rating_editor, SIGNAL(ratingChanged(int)), m_filter, SLOT(setMaxRating(int)));
    connect(m_tags_edit, SIGNAL(textChanged(QString)), m_filter, SLOT(setTagFilter(QString)));
}

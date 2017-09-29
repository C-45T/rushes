#include "tagswidget.h"

#include <QVBoxLayout>
#include <QDebug>

TagsWidget::TagsWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_tags_lbl = new QLabel(this);
    m_tags_lbl->setTextFormat(Qt::RichText);

    layout->addWidget(m_tags_lbl);
}

void TagsWidget::setTags(const QStringList &tags)
{
    QString label_text = "";

    foreach (QString tag, tags)
    {
        label_text .append("<span style=""font-size:14px;padding:10px;color:white;font-weight:bold;background-color:green"">");
        label_text .append(tag);
        label_text .append("</span> ");
    }

    m_tags_lbl->setText(label_text);
}

#include "tagswidget.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>

TagsWidget::TagsWidget(QWidget *parent) : QWidget(parent)
{
    m_layout = new FlowLayout(this);
}

void TagsWidget::setTags(const QStringList &tags)
{
    //QString label_text = "";
    //m_layout->clear();

    foreach (QLabel *label, m_labels)
        label->deleteLater();
    m_labels.clear();
    m_layout->clear();

    foreach (QString tag, tags)
    {
        QLabel *tag_lbl = new QLabel();
        tag_lbl->setText(tag);
        tag_lbl->setProperty("isTag", true);

        m_layout->addWidget(tag_lbl);
        m_labels.append(tag_lbl);
        //tag_lbl->setTextFormat(Qt::RichText);

        //label_text .append("<span style=""font-size:14px;padding:10px;color:white;font-weight:bold;background-color:green"">");
        //label_text .append(tag);
        //label_text .append("</span> ");
    }

    //m_tags_lbl->setText(label_text);
}

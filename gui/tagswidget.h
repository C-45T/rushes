#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QLabel>

#include "thirdParty/flowlayout.h"

class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QWidget *parent = nullptr);

    void setTags(const QStringList& tags);

signals:

public slots:

private:
    FlowLayout *m_layout;
    QList<QLabel*> m_labels;
};

#endif // TAGSWIDGET_H

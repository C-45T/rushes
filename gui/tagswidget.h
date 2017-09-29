#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QLabel>

class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QWidget *parent = nullptr);

    void setTags(const QStringList& tags);

signals:

public slots:

private:
    QLabel *m_tags_lbl;
};

#endif // TAGSWIDGET_H

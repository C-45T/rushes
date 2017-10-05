#include "basedialog.h"

#include <QVBoxLayout>

BaseDialog::BaseDialog(QWidget *display_widget, QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    display_widget->setParent(this);
    layout->addWidget(display_widget);
}

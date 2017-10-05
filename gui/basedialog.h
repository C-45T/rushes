#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>

class BaseDialog : public QDialog
{
public:
    BaseDialog(QWidget *display_widget, QWidget *parent = 0);
};

#endif // BASEDIALOG_H

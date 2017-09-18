#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "database.h"
#include "catalogmodel.h"
#include "libvlcplayerwidget.h"
#include "catalogview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addVideo();
    void playVideo(const QString& filepath);

private:
    Database m_db;
    CatalogModel *m_catalog;
    LibvlcPlayerWidget *m_player;
    CatalogView *m_catalogView;
};

#endif // MAINWINDOW_H

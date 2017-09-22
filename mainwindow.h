#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "database.h"
#include "catalogmodel.h"
#include "cataloggraphicsview.h"
#include "libvlcplayerwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addVideo();
    void playVideo(const QString& filepath);

    void exportToProres();

private:

    void createMenus();

    Database m_db;
    CatalogModel *m_catalog;
    LibvlcPlayerWidget *m_player;
    CatalogGraphicsView *m_view;
};

#endif // MAINWINDOW_H

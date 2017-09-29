#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "database.h"
#include "catalogmodel.h"
#include "catalogwidget.h"

#include "gui/mediainfowidget.h"
#include "gui/playerwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addVideo();
    void playVideo(const QString& filepath);

    void addTags();
    void exportToProres();

    void onSelectionChanged();

private:

    void createMenus();

    Database m_db;
    CatalogModel *m_catalog;
    PlayerWidget *m_player;
    CatalogWidget *m_view;
    MediaInfoWidget *m_media_info;
};

#endif // MAINWINDOW_H

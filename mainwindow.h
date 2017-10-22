#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include "data/database.h"

#include "core/catalogfilter.h"
#include "core/jobsmaster.h"
#include "core/faces.h"

#include "gui/thumbnailview.h"
#include "gui/rushinfowidget.h"
#include "gui/playerwidget.h"
#include "gui/tagswidget.h"
#include "gui/catalogtreewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

public slots:
    void addVideo();
    void playVideo(const QString& filepath);

    void addTags();
    void transcode(const QString& command_preset);
    void faceRecognition();
    void addRushToCatalog();
    void removeRushFromCatalog();
    void exportDatabase();
    void importDatabase();

    void onSelectionChanged();
    void onShowJobsProgress();
    void onFaceRecognitionTraining();

    void refreshTheme();

protected:
    void readSettings();
    void writeSettings();

    void createTranscodingActions();

private:

    void createMenus();

    QList<QAction *> m_context_actions;
    QList<QAction *> m_transcode_actions;

    Database m_db;
    Faces m_faces;
    CatalogFilter *m_rush_filter;

    PlayerWidget *m_player;
    ThumbnailView *m_view;
    ThumbnailView *m_explorer_view;
    RushInfoWidget *m_media_info;
    TagsWidget *m_tag_widget;
    CatalogTreeWidget *m_bin_tree_widget;

    QSplitter *m_main_splitter;
    QSplitter *m_top_splitter;
    QSplitter *m_bottom_splitter;

    JobsMaster m_job_master;

};

#endif // MAINWINDOW_H

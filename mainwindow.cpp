#include "mainwindow.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QInputDialog>
#include <QSettings>
#include <QSignalMapper>
#include <QStatusBar>
#include <QStringList>

#include "core/catalogfilter.h"
#include "core/ffmpegparser.h"
#include "core/exportjob.h"
#include "core/importjob.h"
#include "core/facedetectionjob.h"

#include "gui/catalogfilterwidget.h"
#include "gui/videothumbnailgraphicitem.h"
#include "gui/cataloggraphicsscene.h"
#include "gui/jobswidget.h"
#include "gui/basedialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // database
    m_db.createBinTable();
    m_db.createRushTable();
    m_db.createTagTable();
    m_db.createRushBinTable();

    // layout
    //QWidget *mainWidget = new QWidget(this);
    //QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_main_splitter = new QSplitter(this);
    m_right_splitter = new QSplitter(this);
    QWidget *layoutWidget = new QWidget(this);
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(layoutWidget);
    m_main_splitter->setOrientation(Qt::Horizontal);
    m_right_splitter->setOrientation(Qt::Vertical);

    // data
    m_catalog = new CatalogModel(m_db, this);
    CatalogFilter *catalogFilter = new CatalogFilter(this);
    m_catalog->setFilter(catalogFilter);

    // widgets
    m_player = new PlayerWidget();
    m_media_info = new RushInfoWidget(this);
    CatalogFilterWidget *catalogFilterWidget = new CatalogFilterWidget(this);
    catalogFilterWidget->setFilter(catalogFilter);
    m_tag_widget = new TagsWidget(this);
    JobsWidget *small_job_widget = new JobsWidget(m_job_master, true, this);
    m_bin_tree_widget = new CatalogTreeWidget(&m_db, this);

    // graphics
    CatalogGraphicsScene *scene = new CatalogGraphicsScene(m_catalog);
    m_view = new CatalogWidget(scene, this);

    // layouts
    leftPanelLayout->addWidget(catalogFilterWidget);
    leftPanelLayout->addWidget(m_view);

    m_main_splitter->addWidget(layoutWidget);
    m_main_splitter->addWidget(m_right_splitter);

    m_right_splitter->addWidget(m_player);
    m_right_splitter->addWidget(m_media_info);
    m_right_splitter->addWidget(m_tag_widget);
    m_right_splitter->addWidget(m_bin_tree_widget);

    this->setCentralWidget(m_main_splitter);
    this->statusBar()->addPermanentWidget(small_job_widget);

    // menu and context actions
    createTranscodingActions();
    createMenus();

    // signals
    connect(catalogFilter, SIGNAL(valueChanged()), m_catalog, SLOT(updateCatalog()));
    connect(m_catalog, SIGNAL(catalogChanged()), scene, SLOT(updateScene()));
    connect(m_catalog, SIGNAL(catalogChanged()), m_view, SLOT(updateView()));
    //connect(catalogFilter, SIGNAL(valueChanged()), scene, SLOT(updateScene()));

    connect(scene, SIGNAL(itemDoubleClicked(QString)), this, SLOT(playVideo(const QString&)));
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));

    connect(m_bin_tree_widget, SIGNAL(binSelected(const QString&)), m_catalog, SLOT(setBin(const QString&)));

    // display
    m_main_splitter->show();

    readSettings();

    QTimer::singleShot(0, m_view, SLOT(updateView()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "R2APPS", "aRticho");

    settings.beginGroup("App");
    settings.setValue("currentCatalog", m_catalog->catalog());
    settings.endGroup();

    settings.beginGroup("MainWindow");
    //settings.setValue("size", size());
    //settings.setValue("pos", pos());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("mainSplitterGeometry", m_main_splitter->saveGeometry());
    settings.setValue("mainSplitterWindowState", m_main_splitter->saveState());
    settings.setValue("rightSplitterGeometry", m_right_splitter->saveGeometry());
    settings.setValue("rightSplitterWindowState", m_right_splitter->saveState());
    settings.setValue("playerSize", m_player->size());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "R2APPS", "aRticho");

    settings.beginGroup("App");
    m_catalog->setBin(settings.value("currentCatalog", "All").toString());
    // check if thumbnail folder exists or create it at first start
    QString thumnbnail_folder = settings.value("thumbnailFolder", "").toString();
    if (thumnbnail_folder.isEmpty())
    {
        QDir path = QDir::currentPath();
        // create path if not exists
        path.mkdir("cache");
        settings.setValue("thumbnailFolder", QDir::currentPath() + "/cache/");
    }

    settings.endGroup();

    settings.beginGroup("MainWindow");
    //resize(settings.value("size", QSize(400, 400)).toSize());
    //move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("windowState").toByteArray());
    restoreGeometry(settings.value("geometry").toByteArray());
    m_main_splitter->restoreGeometry(settings.value("mainSplitterGeometry").toByteArray());
    m_main_splitter->restoreState(settings.value("mainSplitterWindowState").toByteArray());
    m_right_splitter->restoreGeometry(settings.value("rightSplitterGeometry").toByteArray());
    m_right_splitter->restoreState(settings.value("rightSplitterGeometry").toByteArray());
    m_player->resize(settings.value("playerSize", QSize(320, 240)).toSize());
    settings.endGroup();
}


void MainWindow::createTranscodingActions()
{
    QFile file("ffmpeg_presets.txt"); // TODO : hardcoded value!

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QMap<QString,QString> presets;
    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList args = line.split("::");
        if (args.size() == 2)
        {
            presets[args[0]]=args[1];
        }
    }

    QSignalMapper *signal_mapper = new QSignalMapper(this);
    for (int i=0; i<presets.size(); i++)
    {
        QString preset_key = presets.keys()[i];
        QAction *action = new QAction(preset_key, this);
        connect(action, SIGNAL(triggered()), signal_mapper, SLOT(map()));
        signal_mapper->setMapping(action, presets[preset_key]);
        m_transcode_actions.append(action);
    }

    connect(signal_mapper, SIGNAL(mapped(QString)), this, SLOT(transcode(QString)));


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}


void MainWindow::addVideo()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Add to catalog", QString(), "Videos (*.mp4 *.avi *.mov *.m4v *.mts)");

    if (filenames.isEmpty())
         return;

    ImportJob *job = new ImportJob(m_db, filenames, m_catalog->catalog());
    m_job_master.addJob(job);

    connect(job, SIGNAL(finished()), m_catalog, SLOT(updateCatalog()));

}

void MainWindow::playVideo(const QString &filepath)
{
    qDebug() << "play" << filepath;
    m_player->openMedia(filepath);
}

void MainWindow::addTags()
{
    QString tags = QInputDialog::getText(this, "Add Tags", "tags (separates with commas (,))");
    if (!tags.isEmpty() && m_catalog)
    {
        foreach (Rush rush, m_view->selectedRush())
        {
            qDebug() << tags;
            m_db.addTagToRush(rush, tags.split(","));
        }
    }
}

void MainWindow::transcode(const QString &command_preset)
{
    QString output_folder = QFileDialog::getExistingDirectory();
    if (m_view && !output_folder.isEmpty())
    {
        foreach (Rush rush, m_view->selectedRush())
        {
            ExportJob *job = new ExportJob(rush, output_folder, command_preset);
            m_job_master.addJob(job);
        }
    }
}

void MainWindow::faceRecognition()
{
    //QStringList files_to_update = selectedFiles();
    foreach (Rush rush, m_view->selectedRush())
    {
        FaceDetectionJob *job = new FaceDetectionJob(m_db, m_faces, rush);
        m_job_master.addJob(job);
    }
}

void MainWindow::addRushToCatalog()
{
    QString catalog_name = QInputDialog::getText(this, "Add Rushs to Catalog", "catalog name");
    if (m_db.getIdFromAttributeValue("Catalog", "name", catalog_name) >= 0 && m_catalog)
    {
        //QStringList files_to_update = selectedFiles();
        foreach (Rush rush, m_view->selectedRush())
        {
            m_db.addRushToBin(catalog_name, rush);
        }
    }
}

void MainWindow::removeRushFromCatalog()
{
    if (m_catalog)
    {
        //QStringList files_to_update = selectedFiles();
        foreach (Rush rush, m_view->selectedRush())
        {
            m_db.removeRushFromBin(m_catalog->catalog(), rush);
        }
    }
}

void MainWindow::exportDatabase()
{
    QString file_name = QFileDialog::getSaveFileName(this, "Export database to CSV");

    if (file_name.isEmpty())
         return;

    m_db.exportToCsv(file_name);
}

void MainWindow::importDatabase()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Import database from CSV");

    if (file_name.isEmpty())
         return;

    m_db.importFromCsv(file_name);
    m_bin_tree_widget->updateTree();
    m_catalog->setBin("All"); // TODO : hardcoded value
}

void MainWindow::onSelectionChanged()
{
    Rush infos;

    if (m_view->selectedFiles().isEmpty())
        return;

    infos = m_view->focusedItem();
    m_media_info->setRush(infos);

    m_tag_widget->setTags(m_db.getRushTags(m_db.getIdFromAttributeValue("Rush", "filename", infos.filename))); // TODO : ugly, fix this

    m_view->view()->onScrollToFocusedItem();

}

void MainWindow::onShowJobsProgress()
{
    JobsWidget *job_widget = new JobsWidget(m_job_master, false, this);
    BaseDialog *dlg = new BaseDialog(job_widget, this);
    dlg->show();
}

void MainWindow::onFaceRecognitionTraining()
{
    // identify faces
    QMap<QString, QStringList> tags = m_faces.tagUnknwonFaces();

    // add tags to videos
    QMapIterator<QString, QStringList> i(tags);
    while (i.hasNext())
    {
        i.next();
        m_db.addTagToRush(m_db.getVideo(i.key()), i.value());
    }
}

void MainWindow::createMenus()
{
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction("Add to Catalog", this, SLOT(addVideo()), QKeySequence(Qt::CTRL + Qt::Key_O));
    file_menu->addAction("Export database to CSV", this, SLOT(exportDatabase()), QKeySequence(Qt::CTRL + Qt::Key_E));
    file_menu->addAction("Import database from CSV", this, SLOT(importDatabase()), QKeySequence(Qt::CTRL + Qt::Key_I));

    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    QAction *tag_action = edit_menu->addAction("Add Tags to selection", this, SLOT(addTags()), QKeySequence(Qt::CTRL + Qt::Key_T));
    QAction *facial_recognition_action = edit_menu->addAction("Facial Recognition", this, SLOT(faceRecognition()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    QAction *add_rush_to_catalog = edit_menu->addAction("Add selected Rush(s) to Catalog", this, SLOT(addRushToCatalog()), QKeySequence(Qt::CTRL + Qt::Key_Plus));
    QAction *remove_rush_from_catalog = edit_menu->addAction("Remove selected Rush(s) from Catalog", this, SLOT(removeRushFromCatalog()), QKeySequence(Qt::CTRL + Qt::Key_Delete));

    QMenu *view_menu = menuBar()->addMenu(tr("&View"));
    view_menu->addAction("Jobs Progress Window", this, SLOT(onShowJobsProgress()), QKeySequence(Qt::Key_F8));

    QMenu *tools_menu = menuBar()->addMenu(tr("&Tools"));
    tools_menu->addAction("Train face recognition algorithm", this, SLOT(onFaceRecognitionTraining()));

    QMenu *context_menu = new QMenu(m_view->view());
    m_context_actions.append(tag_action);
    m_context_actions.append(facial_recognition_action);
    m_context_actions.append(add_rush_to_catalog);
    m_context_actions.append(remove_rush_from_catalog);
    context_menu->addActions(m_context_actions);
    QMenu *transcode_menu = context_menu->addMenu(tr("&Transcode"));
    transcode_menu->addActions(m_transcode_actions);
    m_view->view()->setContextMenu(context_menu);
}


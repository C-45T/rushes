#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSplitter>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QInputDialog>
#include <QSettings>
#include <QSignalMapper>

#include "catalogfilter.h"
#include "catalogfilterwidget.h"

#include "faces.h"
#include <QStringList>

#include <QGraphicsScene>
#include "videothumbnailgraphicitem.h"
#include "cataloggraphicsscene.h"
#include "proc/ffmpegparser.h"

#include "proc/exportjob.h"
#include "proc/importjob.h"

#include "gui/jobswidget.h"
#include "gui/basedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // layout
    //QWidget *mainWidget = new QWidget(this);
    //QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QSplitter *main_splitter = new QSplitter(this);
    QSplitter *right_splitter = new QSplitter(this);
    QWidget *layoutWidget = new QWidget(this);
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(layoutWidget);
    main_splitter->setOrientation(Qt::Horizontal);
    right_splitter->setOrientation(Qt::Vertical);

    // data
    m_catalog = new CatalogModel(m_db, this);
    CatalogFilter *catalogFilter = new CatalogFilter(this);
    m_catalog->setFilter(catalogFilter);

    // widgets
    m_player = new PlayerWidget();
    m_media_info = new MediaInfoWidget(this);
    CatalogFilterWidget *catalogFilterWidget = new CatalogFilterWidget(this);
    catalogFilterWidget->setFilter(catalogFilter);
    m_tag_widget = new TagsWidget(this);
    JobsWidget *small_job_widget = new JobsWidget(m_job_master, true, this);

    // graphics
    CatalogGraphicsScene *scene = new CatalogGraphicsScene(m_catalog);
    m_view = new CatalogWidget(scene, this);

    // layouts
    leftPanelLayout->addWidget(catalogFilterWidget);
    leftPanelLayout->addWidget(m_view);
    leftPanelLayout->addWidget(small_job_widget);

    main_splitter->addWidget(layoutWidget);
    main_splitter->addWidget(right_splitter);

    right_splitter->addWidget(m_player);
    right_splitter->addWidget(m_media_info);
    right_splitter->addWidget(m_tag_widget);

    this->setCentralWidget(main_splitter);

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

    // display
    main_splitter->show();

    // database
    m_db.createCatalogTable();
    m_db.createVideoTable();
    m_db.createTagTable();

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

    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "R2APPS", "aRticho");

    settings.beginGroup("App");
    m_catalog->setCatalog(settings.value("currentCatalog", "default").toString());
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

    ImportJob *job = new ImportJob(m_db, filenames, "default");
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
        QString tags = QInputDialog::getText(this, "Add Tags", "tags");
        if (!tags.isEmpty() && m_catalog)
        {
            //QStringList files_to_update = selectedFiles();
            foreach (QString filename, m_view->selectedFiles())
            {
                qDebug() << tags;
                m_catalog->addTags(filename, tags.split(","));
            }
        }
}

void MainWindow::transcode(const QString &command_preset)
{
    QString output_folder = QFileDialog::getExistingDirectory();
    if (m_view && !output_folder.isEmpty())
    {
        foreach (MediaInfo media, m_view->selectedMedia())
        {
            ExportJob *job = new ExportJob(media, output_folder, command_preset);
            m_job_master.addJob(job);
        }
    }
}

void MainWindow::faceRecognition()
{
    //QStringList files_to_update = selectedFiles();
    foreach (QString filename, m_view->selectedFiles())
    {
        Faces f;
        QStringList tags = f.parseVideo(filename);
        tags.append(f.tagUnknwonFaces());

        qDebug() << tags;

        m_catalog->addTags(filename, tags);
    }
}

void MainWindow::onSelectionChanged()
{
    MediaInfo infos;

    if (m_view->selectedFiles().isEmpty())
        return;

    infos = m_view->focusedItem();
    m_media_info->setMediaInfo(infos);

    m_tag_widget->setTags(m_catalog->getVideoTags(infos.filename));

    m_view->view()->onScrollToFocusedItem();

}

void MainWindow::onShowJobsProgress()
{
    JobsWidget *job_widget = new JobsWidget(m_job_master, false, this);
    BaseDialog *dlg = new BaseDialog(job_widget, this);
    dlg->show();
}

void MainWindow::createMenus()
{
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction("Add to Catalog", this, SLOT(addVideo()), QKeySequence(Qt::CTRL + Qt::Key_O));

    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    QAction *tag_action = edit_menu->addAction("Add Tags to selection", this, SLOT(addTags()), QKeySequence(Qt::CTRL + Qt::Key_T));
    QAction *facial_recognition_action = edit_menu->addAction("Facial Recognition", this, SLOT(faceRecognition()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));

    QMenu *view_menu = menuBar()->addMenu(tr("&View"));
    view_menu->addAction("Jobs Progress Window", this, SLOT(onShowJobsProgress()), QKeySequence(Qt::Key_F8));

    QMenu *context_menu = new QMenu(m_view->view());
    m_context_actions.append(tag_action);
    m_context_actions.append(facial_recognition_action);
    context_menu->addActions(m_context_actions);
    QMenu *transcode_menu = context_menu->addMenu(tr("&Transcode"));
    transcode_menu->addActions(m_transcode_actions);
    m_view->view()->setContextMenu(context_menu);
}


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

#include "catalogfilter.h"
#include "catalogfilterwidget.h"

#include "faces.h"
#include <QStringList>

#include <QGraphicsScene>
#include "videothumbnailgraphicitem.h"
#include "cataloggraphicsscene.h"
#include "proc/ffmpegparser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMenus();

    // layout
    //QWidget *mainWidget = new QWidget(this);
    //QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QSplitter *splitter = new QSplitter(this);
    QWidget *layoutWidget = new QWidget(this);
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(layoutWidget);
    splitter->setOrientation(Qt::Horizontal);

    // data
    m_catalog = new CatalogModel(m_db.sqlDatabase(), this);
    CatalogFilter *catalogFilter = new CatalogFilter(this);
    m_catalog->setFilter(catalogFilter);

    // widgets
    m_player = new LibvlcPlayerWidget();
    CatalogFilterWidget *catalogFilterWidget = new CatalogFilterWidget(this);
    catalogFilterWidget->setFilter(catalogFilter);

    // graphics
    CatalogGraphicsScene *scene = new CatalogGraphicsScene(m_catalog);
    m_view = new CatalogGraphicsView(scene, this);

    // layouts
    leftPanelLayout->addWidget(catalogFilterWidget);
    leftPanelLayout->addWidget(m_view);

    splitter->addWidget(layoutWidget);
    splitter->addWidget(m_player);

    this->setCentralWidget(splitter);

    // signals

    connect(catalogFilter, SIGNAL(valueChanged()), m_catalog, SLOT(updateCatalog()));
    connect(m_catalog, SIGNAL(catalogChanged()), scene, SLOT(updateScene()));
    //connect(catalogFilter, SIGNAL(valueChanged()), scene, SLOT(updateScene()));

    connect(scene, SIGNAL(itemDoubleClicked(QString)), this, SLOT(playVideo(const QString&)));

    // display
    splitter->show();

    // database
    m_db.createCatalogTable();
    m_db.createVideoTable();
    m_db.createTagTable();

    m_catalog->setCatalog("default");

    QTimer::singleShot(0, m_view, SLOT(updateView()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::addVideo()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Add to catalog", QString(), "Videos (*.mp4 *.avi *.mov *.m4v *.mts)");

    if (filenames.isEmpty())
         return;

    m_catalog->addToCatalog("default", filenames);
}

void MainWindow::playVideo(const QString &filepath)
{
    qDebug() << "play" << filepath;
    m_player->playFile(filepath);
}

void MainWindow::exportToProres()
{
    QString output_folder = QFileDialog::getExistingDirectory();
    if (m_view && !output_folder.isEmpty())
    {
        foreach (QString filename, m_view->selectedFiles())
        {
            FFMpegParser::exportProres(filename, output_folder);
        }
    }
}

void MainWindow::createMenus()
{
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction("Add to Catalog", this, SLOT(addVideo()), QKeySequence(Qt::CTRL + Qt::Key_O));
    file_menu->addAction("Export selection to Prores", this, SLOT(exportToProres()), QKeySequence(Qt::CTRL + Qt::Key_E));

    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction("Add Tags to selection", this, SLOT(addTags()), QKeySequence(Qt::CTRL + Qt::Key_T));
    edit_menu->addAction("Facial Recognition", this, SLOT(faceRecognition()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
}


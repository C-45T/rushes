#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QListWidget>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSplitter>

#include "catalogfilter.h"
#include "catalogfilterwidget.h"

#include "faces.h"
#include <QStringList>

#include "cataloggraphicsview.h"
#include <QGraphicsScene>
#include "videothumbnailgraphicitem.h"
#include "cataloggraphicsscene.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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
    QPushButton *openBtn = new QPushButton("Open File");
    m_catalogView = new CatalogView(this);
    m_catalogView->setModel(m_catalog);

    // graphics
    CatalogGraphicsScene *scene = new CatalogGraphicsScene(m_catalog);
    CatalogGraphicsView *view = new CatalogGraphicsView(scene, this);

    // layouts
    leftPanelLayout->addWidget(catalogFilterWidget);
    leftPanelLayout->addWidget(m_catalogView);
    leftPanelLayout->addWidget(openBtn);
    leftPanelLayout->addWidget(view);

    splitter->addWidget(layoutWidget);
    splitter->addWidget(m_player);

    this->setCentralWidget(splitter);

    // signals
    connect(openBtn, SIGNAL(clicked()), this, SLOT(addVideo()));

    //connect(m_catalog, SIGNAL(catalogChanged()), m_catalogView, SLOT(resizeRowsToContents()));

    connect(catalogFilter, SIGNAL(valueChanged()), m_catalog, SLOT(updateCatalog()));
    connect(m_catalog, SIGNAL(catalogChanged()), scene, SLOT(updateScene()));
    //connect(catalogFilter, SIGNAL(valueChanged()), scene, SLOT(updateScene()));

    connect(scene, SIGNAL(itemDoubleClicked(QString)), this, SLOT(playVideo(const QString&)));

    // display
    splitter->show();
    m_catalogView->hide();

    // database
    m_db.createCatalogTable();
    m_db.createVideoTable();
    m_db.createTagTable();

    m_catalog->setCatalog("default");
    m_catalogView->resizeRowsToContents();

    view->updateView();
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
    //m_catalogView->resizeRowsToContents();
}

void MainWindow::playVideo(const QString &filepath)
{
    qDebug() << "play" << filepath;
    m_player->playFile(filepath);
}


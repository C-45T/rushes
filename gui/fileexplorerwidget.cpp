#include "fileexplorerwidget.h"

#include <QFileSystemModel>
#include <QDebug>
#include <QSettings>

#include "core/thumbnailrenderer.h"

FileExplorerWidget::FileExplorerWidget(Database &db, QWidget *parent)
    : QSplitter(parent),
      m_db(db)
{
    m_tree_view = new QTreeView(this);
    m_thumbnail_view = new ThumbnailView(m_db, this);

    m_model = new QFileSystemModel;
    m_model->setRootPath(QDir::currentPath());
    m_model->setFilter(QDir::AllDirs | QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
    m_model->sort(0);

    m_tree_view->setModel(m_model);
    m_tree_view->hideColumn(1);
    m_tree_view->hideColumn(2);
    m_tree_view->hideColumn(3);

    addWidget(m_tree_view);
    addWidget(m_thumbnail_view);

    connect(m_tree_view, SIGNAL(clicked(QModelIndex)), this, SLOT(onDirectoryChanged(QModelIndex)));
    //setRootIndex(model->index(QDir::currentPath()));

    readSettings();
}

FileExplorerWidget::~FileExplorerWidget()
{
    qDebug() << "FileExplorerWidget::~FileExplorerWidget()" << m_current_dir.absolutePath();
    writeSettings();
}

QString FileExplorerWidget::currentDirectory() const
{
    return m_current_dir.absolutePath();
}

void FileExplorerWidget::writeSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("FileExplorerWidget");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("rootPath",m_current_dir.absolutePath());
    settings.endGroup();
}

void FileExplorerWidget::readSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("FileExplorerWidget");
    restoreState(settings.value("windowState").toByteArray());
    restoreGeometry(settings.value("geometry").toByteArray());

    //QItemSelectionModel *selection = m_tree_view->selectionModel();

    m_current_dir = settings.value("rootPath", QDir::currentPath()).toString();
    qDebug() << "FileExplorerWidget::readSettings()" << m_current_dir << m_model->index(m_current_dir.absolutePath());

    //m_tree_view->setExpanded(m_model->index(m_current_dir.absolutePath()), true);
    m_tree_view->setCurrentIndex(m_model->index(m_current_dir.absolutePath()));
    onDirectoryChanged(m_model->index(m_current_dir.absolutePath()));

    //selection->select(m_model->index(m_current_dir.absolutePath()), QItemSelectionModel::ClearAndSelect);
    settings.endGroup();
}

ThumbnailView *FileExplorerWidget::view() const
{
    return m_thumbnail_view;
}

void FileExplorerWidget::onDirectoryChanged(const QModelIndex &index)
{
    m_current_dir = QDir(m_model->filePath(index));

    ThumbnailRenderer::instance()->reset();

    //setNameFilters

    QStringList name_filters;
    name_filters << "*.mp4" << "*.mov" << "*.mts" << "*.avi" << "*.m4v";
    QStringList dir_files = m_current_dir.entryList(name_filters, QDir::Files | QDir::NoDotAndDotDot);

    qDebug() << m_model->filePath(index) << dir_files;

    m_thumbnail_view->setFiles(dir_files, m_current_dir, true);
}

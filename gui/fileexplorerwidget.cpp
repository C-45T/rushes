#include "fileexplorerwidget.h"

#include <QFileSystemModel>
#include <QDebug>

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

    m_tree_view->setModel(m_model);
    m_tree_view->hideColumn(1);
    m_tree_view->hideColumn(2);
    m_tree_view->hideColumn(3);

    addWidget(m_tree_view);
    addWidget(m_thumbnail_view);

    connect(m_tree_view, SIGNAL(clicked(QModelIndex)), this, SLOT(onDirectoryChanged(QModelIndex)));
    //setRootIndex(model->index(QDir::currentPath()));
}

ThumbnailView *FileExplorerWidget::view() const
{
    return m_thumbnail_view;
}

void FileExplorerWidget::onDirectoryChanged(const QModelIndex &index)
{
    QDir current_dir(m_model->filePath(index));

    ThumbnailRenderer::instance()->reset();

    //setNameFilters

    QStringList name_filters;
    name_filters << "*.mp4" << "*.mov" << "*.mts" << "*.avi" << "*.m4v";
    QStringList dir_files = current_dir.entryList(name_filters, QDir::Files | QDir::NoDotAndDotDot);

    qDebug() << m_model->filePath(index) << dir_files;

    m_thumbnail_view->setFiles(dir_files, current_dir, true);
}

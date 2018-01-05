/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2017 Remy Ruttner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "mainwindow.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QInputDialog>
#include <QSettings>
#include <QSignalMapper>
#include <QStatusBar>
#include <QStringList>
#include <QApplication>
#include <QMessageBox>

#include "core/ffmpegparser.h"
#include "core/exportjob.h"
#include "core/facedetectionjob.h"
#include "core/fcpxmlexporter.h"
#include "core/onsetdetector.h"

#include "gui/catalogfilterwidget.h"
#include "gui/jobswidget.h"
#include "gui/basedialog.h"
#include "gui/fileexplorerwidget.h"
#include "gui/aboutdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    FFMpegParser::init();

    // database
    m_db.createBinTable();
    m_db.createRushTable();
    m_db.createTagTable();
    m_db.createRushBinTable();
    m_db.createExtractTable();

    // layout
    //QWidget *mainWidget = new QWidget(this);
    //QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_main_splitter = new QSplitter(this);
    m_top_splitter = new QSplitter(this);
    m_bottom_splitter = new QSplitter(this);
    QWidget *leftLayoutWidget = new QWidget(this);
    QWidget *rightLayoutWidget = new QWidget(this);
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftLayoutWidget);
    QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightLayoutWidget);
    m_main_splitter->setOrientation(Qt::Vertical);
    m_top_splitter->setOrientation(Qt::Horizontal);
    m_bottom_splitter->setOrientation(Qt::Horizontal);

    // data
    m_rush_filter = new CatalogFilter(m_db, this);

    // widgets
    m_player = new PlayerWidget();
    m_media_info = new RushInfoWidget(this);
    CatalogFilterWidget *catalogFilterWidget = new CatalogFilterWidget(this);
    catalogFilterWidget->setFilter(m_rush_filter);
    m_tag_widget = new TagsWidget(this);
    JobsWidget *small_job_widget = new JobsWidget(m_job_master, true, this);
    m_bin_tree_widget = new CatalogTreeWidget(&m_db, this);
    FileExplorerWidget *explorer = new FileExplorerWidget(m_db, this);

    // graphics
    m_view = new ThumbnailView(m_db, this);
    m_explorer_view = explorer->view();

    // layouts
    leftPanelLayout->addWidget(catalogFilterWidget);
    leftPanelLayout->addWidget(m_view);

    rightPanelLayout->addWidget(m_media_info);
    rightPanelLayout->addWidget(m_tag_widget);

    m_main_splitter->addWidget(m_top_splitter);
    m_main_splitter->addWidget(m_bottom_splitter);

    m_top_splitter->addWidget(explorer);
    m_top_splitter->addWidget(m_player);

    m_bottom_splitter->addWidget(m_bin_tree_widget);
    m_bottom_splitter->addWidget(leftLayoutWidget);
    m_bottom_splitter->addWidget(rightLayoutWidget);

    this->setCentralWidget(m_main_splitter);
    this->statusBar()->addPermanentWidget(small_job_widget);

    // menu and context actions
    createTranscodingActions();
    createMenus();

    // signals
    connect(explorer->view(), SIGNAL(itemDoubleClicked(MediaGraphicItem*)), this, SLOT(playMedia(MediaGraphicItem*)));
    //connect(m_rush_filter, SIGNAL(selectionChanged(QStringList, QList<qint64>)), m_view, SLOT(setFiles(QStringList)));
    connect(m_rush_filter, SIGNAL(selectionChanged(QStringList, QList<qint64>)), m_view, SLOT(setExtracts(QStringList,QList<qint64>)));
    connect(m_view, SIGNAL(itemDoubleClicked(MediaGraphicItem*)), this, SLOT(playMedia(MediaGraphicItem*)));
    connect(m_view, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(m_player, SIGNAL(keyTimeChanged(Extract*)), this, SLOT(updateExtract(Extract*)));
    m_rush_filter->querySelection();

    connect(m_bin_tree_widget, SIGNAL(binSelected(const QString&)), m_rush_filter, SLOT(setBin(const QString&)));

    // display
    m_main_splitter->show();

    readSettings();
}

MainWindow::~MainWindow()
{

}

void MainWindow::writeSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("App");
    settings.setValue("currentCatalog", m_rush_filter->bin());
    settings.endGroup();

    settings.beginGroup("MainWindow");
    //settings.setValue("size", size());
    //settings.setValue("pos", pos());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("mainSplitterGeometry", m_main_splitter->saveGeometry());
    settings.setValue("mainSplitterWindowState", m_main_splitter->saveState());
    settings.setValue("topSplitterGeometry", m_top_splitter->saveGeometry());
    settings.setValue("topSplitterWindowState", m_top_splitter->saveState());
    settings.setValue("bottomSplitterGeometry", m_bottom_splitter->saveGeometry());
    settings.setValue("bottomSplitterWindowState", m_bottom_splitter->saveState());
    settings.setValue("playerSize", m_player->size());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("App");
    m_bin_tree_widget->selectBin(settings.value("currentCatalog", "All").toString());
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
    m_top_splitter->restoreGeometry(settings.value("topSplitterGeometry").toByteArray());
    m_top_splitter->restoreState(settings.value("topSplitterWindowState").toByteArray());
    m_bottom_splitter->restoreGeometry(settings.value("bottomSplitterGeometry").toByteArray());
    m_bottom_splitter->restoreState(settings.value("bottomSplitterWindowState").toByteArray());
    m_player->resize(settings.value("playerSize", QSize(320, 240)).toSize());
    settings.endGroup();

    // crash test
//    QVector<Rush*> v;
//    v[2]->hasMetadata();
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
    foreach (Rush* rush, m_explorer_view->selectedRush())
    {
        m_db.addRushToBin(rush, m_rush_filter->bin());
    }

    m_rush_filter->querySelection();
}

void MainWindow::playVideo(const QString &filepath)
{
    qDebug() << "MainWindow::playVideo" << filepath;
    m_player->openMedia(filepath);
}

void MainWindow::playMedia(MediaGraphicItem *item)
{
    qDebug() << "MainWindow::playMedia";
    if (item->extract() != 0)
        m_player->openMedia(item->extract());
    else if (item->rush() != 0)
        m_player->openMedia(item->rush());
}

void MainWindow::addTags()
{
    QString tags = QInputDialog::getText(this, "Add Tags", "tags (separates with commas (,))");
    if (!tags.isEmpty())
    {
        foreach (Rush *rush, m_view->selectedRush())
        {
            qDebug() << tags;
            m_db.addTagToRush(*rush, tags.split(","));
            onSelectionChanged();
        }
    }
}

void MainWindow::exportSelectionToFCPXml()
{
    FCPXmlExporter exp;
    exp.exportTo("test_fcp.xml", m_view->selectedExtract());
}

void MainWindow::transcode(const QString &command_preset)
{
    QString output_folder = QFileDialog::getExistingDirectory();
    if (m_view && !output_folder.isEmpty())
    {
        foreach (Rush *rush, m_view->selectedRush())
        {
            ExportJob *job = new ExportJob(*rush, output_folder, command_preset);
            m_job_master.addJob(job);
        }
    }
}

void MainWindow::faceRecognition()
{
    //QStringList files_to_update = selectedFiles();
    foreach (Rush *rush, m_view->selectedRush())
    {
        FaceDetectionJob *job = new FaceDetectionJob(m_db, m_faces, *rush);
        m_job_master.addJob(job);

        connect(job, SIGNAL(finished()), m_view, SLOT(update()));
    }
}

void MainWindow::relinkRushs()
{
    QString new_folder = QFileDialog::getExistingDirectory();
    if (m_view && !new_folder.isEmpty())
    {
        foreach (Rush *rush, m_view->selectedRush())
        {
            // check if rush is in database
            if (rush && rush->database_id >= 0)
            {
                // check if file exists in new folder :
                QFileInfo old_file_info(rush->file_name);
                QFileInfo new_file_info(QDir(new_folder).absoluteFilePath(old_file_info.fileName()));

                // check if metadata match
                // compare rushs length and bitrate (should we do more/less?)
                if (new_file_info.exists())
                {
                    Rush new_rush;
                    FFMpegParser::getMetaData(new_file_info.absoluteFilePath(), new_rush);

                    if (new_rush.length == rush->length && new_rush.bitrate == rush->bitrate)
                        m_db.changeSourceFileName(rush, new_file_info.absoluteFilePath());
                }
            }
        }
    }
}

void MainWindow::addRushToBin()
{
    QString bin_name = QInputDialog::getText(this, "Add Rushs to Catalog", "catalog name");
    if (m_db.getIdFromAttributeValue("Bin", "name", bin_name) >= 0)
    {
        //QStringList files_to_update = selectedFiles();
        foreach (Rush *rush, m_view->selectedRush())
        {
            m_db.addRushToBin(rush, bin_name);
        }
    }
}

void MainWindow::removeRushFromBin()
{
    if (m_rush_filter)
    {
        //QStringList files_to_update = selectedFiles();
        foreach (Rush *rush, m_view->selectedRush())
        {
            m_db.removeRushFromBin(m_rush_filter->bin(), *rush);
        }
    }
    m_rush_filter->querySelection();
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
    m_rush_filter->setBin("All"); // TODO : hardcoded value
}

void MainWindow::updateExtract(Extract *extract)
{
    m_db.updateExtract(extract);
}

void MainWindow::onSelectionChanged()
{
    Rush *rush;

    if (m_view->selectedRush().isEmpty())
        return;

    rush = m_view->focusedItem();

    if (rush == 0)
        return;

    m_media_info->setRush(*rush);

    m_tag_widget->setTags(m_db.getRushTags(rush->database_id));
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
        m_db.addTagToRush(m_db.getRush(i.key()), i.value());
    }
}

void MainWindow::refreshTheme()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Import database from CSV");

    if (file_name.isEmpty())
         return;

    QFile f(file_name);
    //QFile f("QTDark.stylesheet");
    //QFile f("RRDark.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

void MainWindow::about()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::createMenus()
{
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    QAction *import_action = file_menu->addAction("Import Video in selected bin", this, SLOT(addVideo()), QKeySequence(Qt::CTRL + Qt::Key_O));
    file_menu->addAction("Export database to CSV", this, SLOT(exportDatabase()), QKeySequence(Qt::CTRL + Qt::Key_E));
    file_menu->addAction("Import database from CSV", this, SLOT(importDatabase()), QKeySequence(Qt::CTRL + Qt::Key_I));

    QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
    QAction *tag_action = edit_menu->addAction("Add Tags to selection", this, SLOT(addTags()), QKeySequence(Qt::CTRL + Qt::Key_T));
    QAction *facial_recognition_action = edit_menu->addAction("Facial Recognition", this, SLOT(faceRecognition()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    QAction *add_rush_to_catalog = edit_menu->addAction("Add selected Rush(s) to Bin", this, SLOT(addRushToBin()), QKeySequence(Qt::CTRL + Qt::Key_Plus));
    QAction *remove_rush_from_bin = edit_menu->addAction("Remove selected Rush(s) from Bin", this, SLOT(removeRushFromBin()), QKeySequence(Qt::CTRL + Qt::Key_Delete));
    QAction *relink_rushs = edit_menu->addAction("Relink selected Rushs", this, SLOT(relinkRushs()));
    QAction *export_to_fcp_xml = edit_menu->addAction("Export to FCPXML", this, SLOT(exportSelectionToFCPXml()));

    QMenu *view_menu = menuBar()->addMenu(tr("&View"));
    view_menu->addAction("Jobs Progress Window", this, SLOT(onShowJobsProgress()), QKeySequence(Qt::Key_F8));

    QMenu *tools_menu = menuBar()->addMenu(tr("&Tools"));
    tools_menu->addAction("Train face recognition algorithm", this, SLOT(onFaceRecognitionTraining()));
    tools_menu->addAction("Change theme", this, SLOT(refreshTheme()), QKeySequence(Qt::Key_F5));

    QMenu *about_menu = menuBar()->addMenu(tr("&Help"));
    about_menu->addAction(tr("About..."), this, SLOT(about()));


    if (m_view)
    {
        QMenu *context_menu = new QMenu(m_view);
        m_context_actions.append(tag_action);
        m_context_actions.append(facial_recognition_action);
        m_context_actions.append(add_rush_to_catalog);
        m_context_actions.append(remove_rush_from_bin);
        m_context_actions.append(relink_rushs);
        m_context_actions.append(export_to_fcp_xml);
        context_menu->addActions(m_context_actions);
        QMenu *transcode_menu = context_menu->addMenu(tr("&Transcode"));
        transcode_menu->addActions(m_transcode_actions);
        m_view->setContextMenu(context_menu);
    }

    if (m_explorer_view)
    {
        QMenu *explorer_context_menu = new QMenu(m_explorer_view);
        explorer_context_menu->addAction(import_action);
        m_explorer_view->setContextMenu(explorer_context_menu);
    }
}


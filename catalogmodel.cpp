#include "catalogmodel.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>
#include <QTimer>
#include <QSqlRecord>

#include "opencv2/opencv.hpp"
#include <iostream>

QImage getThumbnailFromFile(QString filename) {

    int default_image_width = 480;
    int default_image_height = 270;

    QImage emptyImage = QImage(default_image_width, default_image_height, QImage::Format_RGB888);

    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name
    cv::VideoCapture cap(filename.toStdString());

    // Check if camera opened successfully
    if(!cap.isOpened()){
        qWarning() << "Error opening video stream or file" << filename;
        return emptyImage;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    cap.set(CV_CAP_PROP_POS_FRAMES,count/2); //Set index to middle frame

    cv::Mat frame;
    // Capture frame
    cap >> frame;

    // save image
    if (!frame.empty()) {
        QImage qImage = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
        frame.release();
        return qImage.scaledToWidth(default_image_width);
    }

    return emptyImage;

}

CatalogModel::CatalogModel(QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent),
    m_db(db)
{
    m_rows = 0;
    m_thumbnail_column_number = 4;
    m_selection_filter = NULL;
    setCatalog("default");
}


QString CatalogModel::catalog() const
{
    return m_catalog;
}

void CatalogModel::setCatalog(const QString &catalog)
{
    qDebug() << "CatalogModel::setCatalog" << catalog;

    m_rows = 0;
    m_catalog = catalog;

    updateCatalog();
}

void CatalogModel::updateCatalog()
{

    QString filterRatingStr = "";
    QString filterTagStr = "";

    if (m_selection_filter) {
        filterRatingStr = m_selection_filter->sqlRatingCondition();
        filterTagStr = m_selection_filter->sqlTagCondition();
    }

    if (!filterRatingStr.isEmpty())
        filterRatingStr.prepend(" AND ");

    if (!filterTagStr.isEmpty())
        filterTagStr.prepend(" AND t.name IN ");

    QString select_part = "SELECT DISTINCT v.id, v.catalog, v.filename, v.thumbnail, v.rating ";
    QString from_part = "FROM Videos v LEFT JOIN Tag t ON (v.id = t.video_id)";
    QString cond_part = "WHERE 1=1" + filterRatingStr + filterTagStr;

    QSqlQuery countQuery;
    countQuery.exec("SELECT count(DISTINCT v.id) " + from_part + cond_part );
    if (countQuery.lastError().isValid()) {
        qDebug() << "error on" <<countQuery.lastQuery() << countQuery.lastError().text();
        m_rows = 0;
    }
    else {
        countQuery.first();
        m_rows = countQuery.value(0).toLongLong();
    }

    QString querystr = select_part + from_part + cond_part;

    setQuery(querystr, m_db);
    if (lastError().isValid())
        qDebug() << "error on" << query().lastQuery() << lastError().text();

    qDebug() << "CatalogModel::setCatalog" << m_rows << query().lastQuery() << lastError().text();

    emit catalogChanged();
}

void CatalogModel::setFilter(CatalogFilter *filter)
{
    m_selection_filter = filter;
}

void CatalogModel::setThumbnailColumnNumber(int new_columns)
{
    if (new_columns == m_thumbnail_column_number)
        return;

    int old_columns = m_thumbnail_column_number;
    int old_rows = rowCount(QModelIndex());
    int new_rows = ((m_rows-1) / new_columns ) + 1;

    if (old_columns < new_columns)
        beginInsertColumns(QModelIndex(), 0, new_columns - old_columns - 1);
    else
        beginRemoveColumns(QModelIndex(), 0, old_columns - new_columns - 1);

    if (old_rows < new_rows)
        beginInsertRows(QModelIndex(), 0, new_rows - old_rows - 1);
    if (old_rows > new_rows)
        beginRemoveRows(QModelIndex(), 0, old_rows - new_rows - 1);

    //qDebug() << old_columns << new_columns;
    m_thumbnail_column_number = new_columns;

    if (old_columns < new_columns)
        endInsertColumns();
    else
        endRemoveColumns();

    if (old_rows < new_rows)
        endInsertRows();
    if (old_rows > new_rows)
        endRemoveRows();

    emit catalogChanged();
}

int CatalogModel::columnCount(const QModelIndex &) const
{
    if (m_rows < m_thumbnail_column_number)
        return m_rows;

    return m_thumbnail_column_number; // TODO change this
}

int CatalogModel::rowCount(const QModelIndex &) const
{
    //return QSqlTableModel::rowCount(index);

    if (m_rows < 1)
        return 0;

    return ((m_rows-1) / m_thumbnail_column_number ) + 1; // TODO remove hardcoded value
}

int CatalogModel::itemCount() const
{
    return m_rows;
}

QVariant CatalogModel::data(const QModelIndex &index, int role) const
{
    //qDebug() << "CatalogModel::data" << index << role;
    if (role == Qt::DisplayRole)
        return record(index.row() * columnCount(index) + index.column()).value(2); // TODO : hardcoded values !!! fix this

    if (role == Qt::UserRole + 1)
        return record(index.row() * columnCount(index) + index.column()).value(3); // TODO : hardcoded values !!! fix this

    if (role == Qt::UserRole + 2)
        return record(index.row() * columnCount(index) + index.column()).value(4); // TODO : hardcoded values !!! fix this

    return QSqlQueryModel::data(index, role);
}

void CatalogModel::updateRating(const QStringList &files, int rating)
{
    QString file_list = "";
    for (int i=0; i< files.size(); i++)
        file_list += QString("'%1',").arg(files[i]);
    file_list.chop(1);

    // update database
    QSqlQuery update_query;
    QString querystr = QString("UPDATE Videos SET rating=%1 WHERE filename in (%2)").arg(QString::number(rating), file_list);
    update_query.exec(querystr);

    qDebug() << "CatalogModel::updateRating" << update_query.lastQuery() << update_query.lastError().text();

    // TODO : optimize, update catalog will do a select query, overkill for a rating change... cache ratings or something
    updateCatalog();
}

void CatalogModel::addTags(const QString &filename, const QStringList &tags)
{
    // retrieve video id
    qint64 video_id = getVideoId(filename);

    // if no video id exit
    if (video_id < 0 || tags.empty())
        return;

    // create insert query
    QSqlQuery add_tag_query;
    QString querystr = QString("INSERT INTO Tag (name, video_id) VALUES");
    foreach (QString tag, tags)
    {
        querystr += QString("('%1',%2),").arg(tag, QString::number(video_id));
    }

    // remove last coma
    querystr.chop(1);

    // execute query
    add_tag_query.exec(querystr);

    qDebug() << "CatalogModel::addTag" << add_tag_query.lastQuery() << add_tag_query.lastError().text();
}

int CatalogModel::getVideoId(const QString &filename) const
{
    qint64 video_id = -1;
    QSqlQuery get_id_query;
    get_id_query.exec(QString("SELECT id FROM Videos WHERE filename='%1' LIMIT 1").arg(filename));
    if (get_id_query.lastError().isValid())
            qDebug() << "error on" <<get_id_query.lastQuery() << get_id_query.lastError().text();
    else {
        get_id_query.first();
        video_id = get_id_query.value(0).toLongLong();
    }

    return video_id;
}

QStringList CatalogModel::getVideoTags(const QString &filename) const
{
    // retrieve video id
    qint64 video_id = getVideoId(filename);

    // if no video id return empty list
    if (video_id < 0)
        return QStringList();

    QStringList tags;
    QSqlQuery get_tags_query;
    get_tags_query.exec(QString("SELECT name FROM Tag WHERE video_id=%1").arg(video_id));
    if (get_tags_query.lastError().isValid()) {
            qDebug() << "error on" <<get_tags_query.lastQuery() << get_tags_query.lastError().text();
    }
    else {
        while (get_tags_query.next()) {
            tags.append(get_tags_query.value(0).toString());
        }
    }

    return tags;
}

void CatalogModel::deleteFromCatalog(const QStringList &files)
{
    QString video_ids = "";
    for (int i=0; i< files.size(); i++)
        video_ids += QString("'%1',").arg(getVideoId(files[i]));
    video_ids.chop(1);

    // delete thumbnails
    // TODO

    // delete from database
    // TODO : handle catalog ?
    QSqlQuery delete_query;

    QString querystr = QString("DELETE FROM Tag WHERE video_id in (%1)").arg(video_ids);
    delete_query.exec(querystr);
    qDebug() << "CatalogModel::deleteFromCatalog" << delete_query.lastQuery() << delete_query.lastError().text();

    querystr = QString("DELETE FROM Videos WHERE id in (%1)").arg(video_ids);
    delete_query.exec(querystr);

    qDebug() << "CatalogModel::deleteFromCatalog" << delete_query.lastQuery() << delete_query.lastError().text();

    updateCatalog();
}

void CatalogModel::addToCatalog(const QString &catalog, const QStringList &filenames)
{
    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    m_catalog = catalog;

    foreach (QString filename, filenames)
        m_queue.push_back(filename);

    if (m_processingFile.isEmpty())
        processQueue();
}

void CatalogModel::processQueue()
{
    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    m_processingFile = m_queue.first();

    qDebug() << "extract frame from " << m_processingFile;

    QTimer::singleShot(0, this, SLOT(onVideoFrameExtracted()));
}

void CatalogModel::onVideoFrameExtracted(/*const QtAV::VideoFrame& frame*/)
{
    QString filename = m_queue.first();
    QImage thumbnail = getThumbnailFromFile(filename);
    QString thumbnail_filename = filename + "_tn.png";

    qDebug() << "save thumbnail" << thumbnail_filename;
    thumbnail.save(thumbnail_filename);

    QSqlQuery insert_query;
    QString querystr = QString("INSERT INTO Videos (catalog, filename, thumbnail) VALUES ('%1', '%2', '%3')").arg(m_catalog, filename, thumbnail_filename);
    insert_query.exec(querystr);

    qDebug() << "CatalogModel::onVideoFrameExtracted" << insert_query.lastQuery() << insert_query.lastError().text();

    m_queue.pop_front();
    m_processingFile = "";
    if (!m_queue.isEmpty())
        processQueue();
    else
        updateCatalog();
}

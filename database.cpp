#include "database.h"

#include <QStringList>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>

static const char *catalogTableName = "Catalog";
static const char *videoTableName = "Rush";

Database::Database()
{
    m_database = QSqlDatabase::database();

    if (!m_database.isValid()) {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        if (!m_database.isValid())
            qFatal("Cannot add database: %s", qPrintable(m_database.lastError().text()));
    }

    const QDir writeDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!writeDir.mkpath("."))
        qFatal("Failed to create writable directory at %s", qPrintable(writeDir.absolutePath()));

    // Ensure that we have a writable location on all devices.
    //const QString fileName = writeDir.absolutePath() + "/catalogr_db.sqlite3";
    const QString fileName = "./rr-catalog-database.sqlite3"; // TODO : hardcoded value

    // When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
    m_database.setDatabaseName(fileName);
    if (!m_database.open()) {
        qFatal("Cannot open database: %s", qPrintable(m_database.lastError().text()));
        QFile::remove(fileName);
    }

    qDebug() << "database : " << fileName;
}

Database::~Database()
{
    qDebug() << "Database::~Database()";

    m_database.commit();
    m_database.close();

    qDebug() << "Database::~Database() - end";
}

void Database::addCatalog(const QString &catalog, const QString &parent)
{
    // Get parent_id
    int parent_id = -1;
    if (!parent.isEmpty())
        parent_id = getIdFromAttributeValue("Catalog", "name", parent);

    // Insert catalog in db
    QSqlQuery query;
    QString querystr = QString("INSERT INTO Catalog (name, parent_id)"
                               " VALUES ('%1', '%2') ")
            .arg(catalog, parent_id == -1 ? "NULL" : QString::number(parent_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();
}

void Database::deleteCatalog(const QString &catalog)
{
    // Get catalog id
    int catalog_id = getIdFromAttributeValue( "Catalog", "name", catalog);

    // if catalog not found return
    if (catalog_id == -1)
        return;

    // get all catalog children
    QStringList catalogs_ids = getCatalogChildren(catalog_id);

    // delete catalog from db
    QSqlQuery query;
    QString querystr = QString("DELETE FROM Catalog WHERE id in ('%1') ")
            .arg(catalogs_ids.join("','"));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();
}

QStringList Database::getCatalogChildren(int catalog_id)
{
    QStringList res;
    res.append(QString::number(catalog_id));

    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT id FROM Catalog "
                               "WHERE parent_id = '%1' "
                               ).arg(QString::number(catalog_id));

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result recursivly
    while (query.next())
    {
        res.append(getCatalogChildren(query.value(0).toInt()));
    }

    return res;
}

void Database::catalogRush(const QString &catalog_name, const MediaInfo &media)
{
    int rush_id = getIdFromAttributeValue("Rush", "filename", media.filename);
    int catalog_id = getIdFromAttributeValue("Catalog", "name", catalog_name);

    if (rush_id < 0 || catalog_id < 0)
        return;

    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO RushCatalog (rush_id, catalog_id) "
                               " VALUES (%1, %2) ")
            .arg(QString::number(rush_id), QString::number(catalog_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

void Database::removeRushFromCatalog(const QString &catalog_name, const MediaInfo &media)
{
    int rush_id = getIdFromAttributeValue("Rush", "filename", media.filename);
    int catalog_id = getIdFromAttributeValue("Catalog", "name", catalog_name);

    if (rush_id < 0 || catalog_id < 0)
        return;

    QSqlQuery query(m_database);
    QString querystr = QString("DELETE FROM RushCatalog "
                               " WHERE rush_id = %1 AND catalog_id = %2")
            .arg(QString::number(rush_id), QString::number(catalog_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

void Database::createCatalogTable()
{
    if (QSqlDatabase::database().tables().contains("Catalog")) {
        // The table already exists; we don't need to do anything.
        return;
    }

    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Catalog' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'name' TEXT NOT NULL UNIQUE,"
                "'parent_id' INTEGER"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    addCatalog("All", "");
}

void Database::createTagTable()
{
//    if (QSqlDatabase::database().tables().contains(tagTableName)) {
//        // The table already exists; we don't need to do anything.
//        return;
//    }

    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Tag' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'name' TEXT NOT NULL ,"
                "'rush_id' INTEGER NOT NULL,"
                "FOREIGN KEY('rush_id') REFERENCES Rush ( id )"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    query.exec("INSERT INTO Catalog VALUES('default')");
}

void Database::createRushCatalogTable()
{
    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'RushCatalog' ("
                "'rush_id' INTEGER NOT NULL,"
                "'catalog_id' INTEGER NOT NULL,"
                "PRIMARY KEY(rush_id, catalog_id),"
                "FOREIGN KEY('rush_id') REFERENCES Rush ( id ),"
                "FOREIGN KEY('catalog_id') REFERENCES Catalog ( id )"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}


void Database::createRushTable()
{
    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Rush' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'filename' TEXT NOT NULL UNIQUE,"
                "'thumbnail' TEXT NOT NULL,"
                "'rating' INTEGER NOT NULL DEFAULT 0,"
                "'length' INTEGER,"
                "'width' INTEGER,"
                "'height' INTEGER,"
                "'fps' INTEGER,"
                "'bitrate' INTEGER,"
                "'video_codec' TEXT,"
                "'audio_codec' TEXT,"
                "'sample_rate' INTEGER," // Hz
                "'channel' TEXT,"
                "'audio_bitrate' INTEGER," // kb/s
                "'utc_creation_time' BIGINT"
                ")")) {
        qDebug() << query.lastQuery();
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}


void Database::addVideo(const MediaInfo &media, const QString& catalog)
{
    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO Rush (filename, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate, "
                               "utc_creation_time)"
                               " VALUES ('%1', '%2', %3, "
                               "%4, %5, %6, %7, '%8', '%9', ")
            .arg(media.filename, media.thumbnail_filename,
                 QString::number(media.length), QString::number(media.width), QString::number(media.height),
                 QString::number(media.fps), QString::number(media.bitrate),
                 media.video_codec, media.audio_codec);

    // arg() takes at most 9 parameters, consinue string building
    querystr += QString("%1, '%2', %3, %4)").arg( QString::number(media.sample_rate),
                                                    media.channel,
                                                    QString::number(media.audio_bitrate),
                                                    QString::number(media.utc_creation_time));
    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

QStringList Database::catalogs(const QString &parent_name) const
{
    QStringList res;

    // get parent id
    int parent_id = getIdFromAttributeValue("Catalog", "name", parent_name);

    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT name "
                               "FROM Catalog "
                               "WHERE parent_id = '%1' ORDER by name"
                               ).arg(parent_id == -1 ? "NULL" : QString::number(parent_id));

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result
    while (query.next())
    {
        res.append(query.value(0).toString());
    }

    return res;
}

qint64 Database::getIdFromAttributeValue(const QString &table_name, const QString &attr_name, const QString &attr_value) const
{
    qint64 obj_id = -1;
    QSqlQuery query;
    query.exec(QString("SELECT id FROM %1 WHERE %2='%3' LIMIT 1").arg(table_name, attr_name, attr_value));
    qDebug() << query.lastQuery() << query.lastError().text();
    if (query.lastError().isValid())
        qDebug() << "error on" <<query.lastQuery() << query.lastError().text();
    else {
        if (query.first())
            obj_id = query.value(0).toLongLong();
    }

    return obj_id;
}




MediaInfo Database::getMediaInfo(const QSqlRecord &record)
{
    MediaInfo info;

    info.filename = record.value("filename").toString();
    info.thumbnail_filename = record.value("thumbnail").toString();
    info.length = record.value("length").toInt();
    info.width = record.value("width").toInt();
    info.height = record.value("height").toInt();
    info.fps = record.value("fps").toInt();
    info.bitrate = record.value("bitrate").toInt();
    info.video_codec = record.value("video_codec").toString();
    info.audio_codec = record.value("audio_codec").toString();
    info.sample_rate = record.value("sample_rate").toInt();
    info.channel = record.value("channel").toString();
    info.audio_bitrate = record.value("audio_bitrate").toInt();
    info.utc_creation_time = record.value("utc_creation_time").toLongLong();
    info.rating = record.value("rating").toInt();

    return info;
}

MediaInfo Database::getVideo(const QString &filename) const
{
    MediaInfo res;
    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT filename, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate,"
                               "utc_creation_time, rating "
                               "FROM Rush "
                               "WHERE filename = '%1' "
                               "LIMIT 1").arg(filename);

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result
    if (query.first())
    {
        return getMediaInfo(query.record());
    }


    return res;
}


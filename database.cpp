#include "database.h"

#include <QStringList>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>

static const char *catalogTableName = "Catalog";
static const char *videoTableName = "Videos";

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
    const QString fileName = "./rr-catalog-database.sqlite3";

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

void Database::createCatalogTable()
{
    if (QSqlDatabase::database().tables().contains(catalogTableName)) {
        // The table already exists; we don't need to do anything.
        return;
    }

    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Catalog' ("
                "'name' TEXT NOT NULL UNIQUE"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    query.exec("INSERT INTO Catalog VALUES('default')");
}

void Database::createTagTable()
{
//    if (QSqlDatabase::database().tables().contains(tagTableName)) {
//        // The table already exists; we don't need to do anything.
//        return;
//    }

    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Tag' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'name' TEXT NOT NULL ,"
                "'video_id' INTEGER NOT NULL,"
                "FOREIGN KEY('video_id') REFERENCES Videos ( id )"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    query.exec("INSERT INTO Catalog VALUES('default')");
}


void Database::createVideoTable()
{
    if (QSqlDatabase::database().tables().contains(videoTableName)) {
        // The table already exists; we don't need to do anything.
        return;
    }

    QSqlQuery query;
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Videos' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'catalog' TEXT NOT NULL,"
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
                "'utc_creation_time' BIGINT,"
                "FOREIGN KEY('catalog') REFERENCES Catalogs ( name )"
                ")")) {
        qDebug() << query.lastQuery();
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

void Database::addVideo(const MediaInfo &media, const QString& catalog)
{
    QSqlQuery query;
    QString querystr = QString("INSERT INTO Videos (catalog, filename, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate, "
                               "utc_creation_time)"
                               " VALUES ('%1', '%2', '%3', "
                               "%4, %5, %6, %7, %8, '%9', ")
            .arg(catalog, media.filename, media.thumbnail_filename,
                 QString::number(media.length), QString::number(media.width), QString::number(media.height), QString::number(media.fps), QString::number(media.bitrate), media.video_codec);

    // arg() takes at most 9 parameters, consinue string building
    querystr += QString("'%1', %2, '%3', %4, %5)").arg(media.audio_codec,
                                                   QString::number(media.sample_rate),
                                                   media.channel,
                                                   QString::number(media.audio_bitrate),
                                                   QString::number(media.utc_creation_time));
    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

MediaInfo Database::getVideo(const QString &filename) const
{
    MediaInfo res;
    QSqlQuery query;

    // build query
    QString querystr = QString("SELECT catalog, filename, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate,"
                               "utc_creation_time "
                               "FROM Videos "
                               "WHERE filename = '%1' "
                               "LIMIT 1").arg(filename);

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result
        if (query.first())
        {
            qDebug() << query.value(0) << query.value(1) << query.value(2);

            res.filename = query.value(1).toString();
            res.thumbnail_filename = query.value(2).toString();
            res.length = query.value(3).toInt();
            res.width = query.value(4).toInt();
            res.height = query.value(5).toInt();
            res.fps = query.value(6).toInt();
            res.bitrate = query.value(7).toInt();
            res.video_codec = query.value(8).toString();
            res.audio_codec = query.value(9).toString();
            res.sample_rate = query.value(10).toInt();
            res.channel = query.value(11).toString();
            res.audio_bitrate = query.value(12).toInt();
            res.utc_creation_time = query.value(13).toLongLong();
        }


    return res;
}

QStringList Database::getVideos() const
{
    QSqlQuery query;

    QString querystr = QString("SELECT * FROM  Videos");
//    query.prepare("SELECT * FROM :table");
//    query.bindValue(":table", videoTableName);
//    query.exec();

    query.exec(querystr);

    qDebug() << "select" << query.size() << query.lastQuery() << query.lastError().text();

    QStringList results;

    while (query.next()) {
        QString cat = query.value(0).toString();
        QString path = query.value(1).toString();
        QString thumbnail = query.value(2).toString();

        qDebug() << cat << path << thumbnail;

        results.append(path);
    }

    return results;

}

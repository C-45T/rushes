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
                "FOREIGN KEY('catalog') REFERENCES Catalogs ( name )"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

void Database::addVideo(const QString& path, const QString& catalog)
{
    //QImage thumbnail = getThumbnailFromFile(path);
    QString thumbnail_filename = path + ".png";

    qDebug() << "save thumbnail" << thumbnail_filename;

    //thumbnail.save(thumbnail_filename);

    QSqlQuery query;
    QString querystr = QString("INSERT INTO Videos (catalog, filename, thumbnail) VALUES ('%1', '%2', '%3')").arg(catalog, path, thumbnail_filename);
    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

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

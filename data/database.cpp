#include "database.h"

#include <QStringList>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

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

void Database::catalogRush(const QString &catalog_name, const Rush &rush)
{
    int rush_id = getIdFromAttributeValue("Rush", "filename", rush.filename);
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

void Database::removeRushFromCatalog(const QString &catalog_name, const Rush &rush)
{
    int rush_id = getIdFromAttributeValue("Rush", "filename", rush.filename);
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


void Database::addVideo(const Rush &rush, const QString& catalog)
{
    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO Rush (filename, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate, "
                               "utc_creation_time)"
                               " VALUES ('%1', '%2', %3, "
                               "%4, %5, %6, %7, '%8', '%9', ")
            .arg(rush.filename, rush.thumbnail_filename,
                 QString::number(rush.length), QString::number(rush.width), QString::number(rush.height),
                 QString::number(rush.fps), QString::number(rush.bitrate),
                 rush.video_codec, rush.audio_codec);

    // arg() takes at most 9 parameters, consinue string building
    querystr += QString("%1, '%2', %3, %4)").arg( QString::number(rush.sample_rate),
                                                    rush.channel,
                                                    QString::number(rush.audio_bitrate),
                                                    QString::number(rush.utc_creation_time));
    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

QStringList Database::getRushTags(qint64 rush_id) const
{
    // if no video id return empty list
    if (rush_id < 0)
        return QStringList();

    QStringList tags;
    QSqlQuery get_tags_query(m_database);
    get_tags_query.exec(QString("SELECT name FROM Tag WHERE rush_id=%1").arg(rush_id));
    if (get_tags_query.lastError().isValid()) {
            qDebug() << "error on" <<get_tags_query.lastQuery() << get_tags_query.lastError().text();
    }
    else {
        while (get_tags_query.next()) {
            tags.append(get_tags_query.value(0).toString());
        }
    }

    qDebug() << "tags" << tags;

    return tags;
}

void Database::addTagToRush(const Rush &rush, QStringList tags)
{
    // retrieve video id
    qint64 rush_id = getIdFromAttributeValue("Rush", "filename", rush.filename);

    if (rush_id < 0)
        return;

    QStringList previous_tags = getRushTags(rush_id);

    QSet<QString> new_tags;
    foreach (QString tag, tags) {
        if (!tag.isEmpty() && !previous_tags.contains(tag.trimmed()))
            new_tags.insert(tag.trimmed());
    }

    // if no tag to add
    if (new_tags.empty())
        return;

    // create insert query
    QSqlQuery add_tag_query(m_database);
    QString querystr = QString("INSERT INTO Tag (name, rush_id) VALUES");
    foreach (QString tag, new_tags)
    {
        querystr += QString("('%1',%2),").arg(tag.trimmed(), QString::number(rush_id));
    }

    // remove last coma
    querystr.chop(1);

    // execute query
    add_tag_query.exec(querystr);

    qDebug() << add_tag_query.lastQuery() << add_tag_query.lastError().text();
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




Rush Database::getRush(const QSqlRecord &record)
{
    Rush rush;

    rush.filename = record.value("filename").toString();
    rush.thumbnail_filename = record.value("thumbnail").toString();
    rush.length = record.value("length").toInt();
    rush.width = record.value("width").toInt();
    rush.height = record.value("height").toInt();
    rush.fps = record.value("fps").toInt();
    rush.bitrate = record.value("bitrate").toInt();
    rush.video_codec = record.value("video_codec").toString();
    rush.audio_codec = record.value("audio_codec").toString();
    rush.sample_rate = record.value("sample_rate").toInt();
    rush.channel = record.value("channel").toString();
    rush.audio_bitrate = record.value("audio_bitrate").toInt();
    rush.utc_creation_time = record.value("utc_creation_time").toLongLong();
    rush.rating = record.value("rating").toInt();

    return rush;
}

void Database::exportToCsv(const QString &output_file_name)
{
    QFile file(output_file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
              return;

    QTextStream out(&file);
    QSqlQuery query(m_database);

    // export version
    out << "<VERSION>" << endl;
    out << QApplication::applicationVersion() << endl;

    // export catalogs
    out << "<CATALOGS>" << endl;
    if (query.exec("SELECT c.name, parent.name as parent "
                   "FROM Catalog c "
                   "LEFT JOIN Catalog parent on (c.parent_id = parent.id)"))
        exportQuery(query, out);



    // export rushs
    out << "<RUSHS>" << endl;
    if (query.exec("SELECT r.*, group_concat(t.name, ',') as tags, group_concat(c.name, ',') as catalogs "
               "FROM Rush r "
               "LEFT JOIN Tag t on (t.rush_id = r.id) "
               "LEFT JOIN RushCatalog rc on (rc.rush_id = r.id) "
               "LEFT JOIN Catalog c on (c.id = rc.catalog_id) "
               "GROUP BY r.id "))
        exportQuery(query, out);

    file.close();
}

void Database::importFromCsv(const QString &input_file_name)
{
    QFile file(input_file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
              return;

    QByteArray line;

    // get version

    // get catalogs
    if (line == "<CATALOGS>")
    {
        line = file.readLine();
        while (!file.atEnd() and !line.startsWith("<"))
        {

            line = file.readLine();
        }
    }

    // get rush
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
    }
}

void Database::exportQuery(QSqlQuery &query, QTextStream &text_stream)
{
    // get field names
    QStringList fields;
    for (int i=0; i< query.record().count(); i++)
        fields.append(query.record().fieldName(i));

    text_stream << fields.join(";") << endl;

    // get values
    while (query.next())
    {
        QStringList values;
        for (int i=0; i< query.record().count(); i++)
            values.append(query.record().value(i).toString());
        text_stream << values.join(";") << endl;
    }

}

Rush Database::getVideo(const QString &filename) const
{
    Rush res;
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
        return getRush(query.record());
    }


    return res;
}


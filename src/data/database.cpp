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

#include "database.h"

#include <QStringList>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>

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
    //const QString fileName = writeDir.absolutePath() + "/rushes_db.sqlite3";
    const QString fileName = "./rushesdb.sqlite3"; // TODO : hardcoded value

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

void Database::createBinTable()
{
    if (QSqlDatabase::database().tables().contains("Bin")) {
        // The table already exists; we don't need to do anything.
        return;
    }

    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Bin' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'name' TEXT NOT NULL UNIQUE,"
                "'parent_id' INTEGER"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    addBin("All", "");
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
                "'rush_id' INTEGER ,"
                "'extract_id' INTEGER ,"
                "FOREIGN KEY('rush_id') REFERENCES Rush ( id )"
                "FOREIGN KEY('extract_id') REFERENCES Extract ( id )"
                ")")) {
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

void Database::createExtractBinTable()
{
    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'ExtractBin' ("
                "'extract_id' INTEGER NOT NULL,"
                "'bin_id' INTEGER NOT NULL,"
                "PRIMARY KEY(extract_id, bin_id),"
                "FOREIGN KEY('extract_id') REFERENCES Extract ( id ),"
                "FOREIGN KEY('bin_id') REFERENCES Bin ( id )"
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
                "'file_name' TEXT NOT NULL UNIQUE,"
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

void Database::createExtractTable()
{
    QSqlQuery query(m_database);
    if (!query.exec(
                "CREATE TABLE IF NOT EXISTS 'Extract' ("
                "'id' INTEGER NOT NULL PRIMARY KEY,"
                "'rush_id' INTEGER NOT NULL,"
                "'thumbnail' TEXT NOT NULL,"
                "'start_time' INTEGER NOT NULL," // ms
                "'stop_time' INTEGER NOT NULL," // ms
                "'key_moment' INTEGER NOT NULL," // ms
                "'rating' INTEGER NOT NULL DEFAULT 0,"
                "'comment' TEXT,"
                "FOREIGN KEY('rush_id') REFERENCES Rush ( id )"
                ")")) {
        qDebug() << query.lastQuery();
        qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }
}

void Database::addBin(const QString &bin_name, const QString &parent_name)
{
    // Get parent_id
    int parent_id = -1;
    if (!parent_name.isEmpty())
        parent_id = getIdFromAttributeValue("Bin", "name", parent_name);

    // Insert Bin in db
    QSqlQuery query;
    QString querystr = QString("INSERT INTO Bin (name, parent_id)"
                               " VALUES ('%1', '%2') ")
            .arg(bin_name, parent_id == -1 ? "NULL" : QString::number(parent_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();
}

void Database::deleteBin(const QString &bin_name)
{
    // Get Bin id
    int bin_id = getIdFromAttributeValue( "Bin", "name", bin_name);

    // if bin not found return
    if (bin_id == -1)
        return;

    // get all bins children
    QStringList bins_ids = getBinChildren(bin_id);

    // delete bin from db
    QSqlQuery query;
    QString querystr = QString("DELETE FROM Bin WHERE id in ('%1') ")
            .arg(bins_ids.join("','"));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();
}

QStringList Database::getBinChildren(int bin_id)
{
    QStringList res;
    res.append(QString::number(bin_id));

    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT id FROM Bin "
                               "WHERE parent_id = '%1' "
                               ).arg(QString::number(bin_id));

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result recursivly
    while (query.next())
    {
        res.append(getBinChildren(query.value(0).toInt()));
    }

    return res;
}

void Database::addExtractToBin(Extract *extract, const QString &bin_name)
{
    int bin_id = getIdFromAttributeValue("Bin", "name", bin_name);

    if (bin_id < 0)
        return;

    // if rush not in db, add rush
    if (extract->rush() != 0 && extract->rush()->database_id < 0)
        addRush(extract->rush());

    // if rush in db but not extract, add extract
    if (extract->database_id() < 0)
        addExtract(extract);

    // if database_id is still not set insertion failed, leave
    if (extract->database_id() < 0)
        return;

    // don't create link with main bin named "All" // TODO : remove hardcoded value
    if (bin_name == "All")
        return;

    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO ExtractBin (extract_id, bin_id) "
                               " VALUES (%1, %2) ")
            .arg(QString::number(extract->database_id()), QString::number(bin_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

void Database::addRush(Rush *rush)
{
    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO Rush (file_name, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate, "
                               "utc_creation_time)"
                               " VALUES ('%1', '%2', %3, "
                               "%4, %5, %6, %7, '%8', '%9', ")
            .arg(rush->file_name, rush->thumbnail_file_name,
                 QString::number(rush->length), QString::number(rush->width), QString::number(rush->height),
                 QString::number(rush->fps), QString::number(rush->bitrate),
                 rush->video_codec, rush->audio_codec);

    // arg() takes at most 9 parameters, consinue string building
    querystr += QString("%1, '%2', %3, %4)").arg( QString::number(rush->sample_rate),
                                                    rush->channel,
                                                    QString::number(rush->audio_bitrate),
                                                    QString::number(rush->utc_creation_time));
    if (query.exec(querystr))
    {
        rush->database_id = getIdFromAttributeValue("Rush", "file_name", rush->file_name);
    }

    qDebug() << query.lastQuery() << query.lastError().text();

}

void Database::deleteRush(Rush *rush)
{
    // retrieve rush id
    qint64 rush_id = rush->database_id;

    if (rush_id < 0)
        return;

    QSqlQuery query(m_database);
    QString querystr;

    // delete associated tags
    querystr = QString("DELETE FROM Tag "
                       " WHERE rush_id = %1")
            .arg(QString::number(rush_id));
    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // delete associated extracts
    querystr = QString("DELETE FROM Extract "
                       " WHERE rush_id = %1")
            .arg(QString::number(rush_id));
    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // delete bin association
    querystr = QString("DELETE FROM RushBin "
                       " WHERE rush_id = %1")
            .arg(QString::number(rush_id));
    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // delete rush
    querystr = QString("DELETE FROM Rush "
                       " WHERE id = %1")
            .arg(QString::number(rush_id));
    query.exec(querystr);

    rush->database_id = -1;

    qDebug() << query.lastQuery() << query.lastError().text();
}

void Database::removeExtractFromBin(const QString& bin_name, Extract *extract)
{
    int extract_id = extract->database_id();
    int bin_id = getIdFromAttributeValue("Bin", "name", bin_name);

    if (extract_id < 0 || bin_id < 0)
        return;

    QSqlQuery query(m_database);
    QString querystr = QString("DELETE FROM ExtractBin "
                               " WHERE extract_id = %1 AND bin_id = %2")
            .arg(QString::number(extract_id), QString::number(bin_id));

    query.exec(querystr);

    qDebug() << query.lastQuery() << query.lastError().text();

}

QStringList Database::getExtractTags(qint64 extract_id) const
{
    // if no video id return empty list
    if (extract_id < 0)
        return QStringList();

    QStringList tags;
    QSqlQuery get_tags_query(m_database);
    get_tags_query.exec(QString("SELECT name FROM Tag WHERE extract_id=%1").arg(extract_id));
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

void Database::setRushRating(Rush *rush, int rating)
{
    // retrieve video id
    qint64 rush_id = rush->database_id;

    if (rush_id < 0)
        return;

    QSqlQuery rate_query(m_database);
    QString querystr = QString("UPDATE Rush SET rating=%1 WHERE id=%2").arg(QString::number(rating), QString::number(rush_id));
    if (rate_query.exec(querystr))
        rush->rating = rating;

    qDebug() << rate_query.lastQuery() << rate_query.lastError().text();
}

void Database::addTagToRush(const Rush &rush, QStringList tags)
{
    // retrieve video id
    qint64 rush_id = rush.database_id;

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

void Database::addTagToExtract(const Extract &extract, QStringList tags)
{
    // retrieve video id
    qint64 rush_id = extract.database_id();

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
    QString querystr = QString("INSERT INTO Tag (name, extract_id) VALUES");
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

QStringList Database::bins(const QString &parent_name) const
{
    QStringList res;

    // get parent id
    int parent_id = getIdFromAttributeValue("Bin", "name", parent_name);

    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT name "
                               "FROM Bin "
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

QString Database::getAttributeValueFromId(const QString &table_name, const QString &attr_name, qint64 id) const
{
    QString attr_value = "";
    QSqlQuery query;
    query.exec(QString("SELECT %1 FROM %2 WHERE id=%3 LIMIT 1").arg(attr_name, table_name, QString::number(id)));
    qDebug() << query.lastQuery() << query.lastError().text();
    if (query.lastError().isValid())
        qDebug() << "error on" <<query.lastQuery() << query.lastError().text();
    else {
        if (query.first())
            attr_value = query.value(0).toString();
    }

    return attr_value;
}

Rush Database::getRush(const QString &file_name) const
{
    Rush res;
    QSqlQuery query(m_database);

    // build query
    QString querystr = QString("SELECT id, file_name, thumbnail, "
                               "length, width, height, fps, bitrate, video_codec, "
                               "audio_codec, sample_rate, channel, audio_bitrate,"
                               "utc_creation_time, rating "
                               "FROM Rush "
                               "WHERE file_name = '%1' "
                               "LIMIT 1").arg(file_name);

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result
    if (query.first())
    {
        return getRush(query.record());
    }


    return res;
}

QList<qint64> Database::getRushExtracts(qint64 rush_id) const
{
    // if no video id return empty list
    if (rush_id < 0)
        return QList<qint64>();

    QList<qint64> extract_ids;
    QSqlQuery get_ids_query(m_database);
    get_ids_query.exec(QString("SELECT distinct id FROM Extract WHERE rush_id=%1").arg(rush_id));
    if (get_ids_query.lastError().isValid()) {
            qDebug() << "error on" <<get_ids_query.lastQuery() << get_ids_query.lastError().text();
    }
    else {
        while (get_ids_query.next()) {
            extract_ids.append(get_ids_query.value(0).toInt());
        }
    }

    return extract_ids;
}

Extract Database::getExtract(int64_t &id, Rush *rush) const
{
    Extract res;
    QSqlQuery query(m_database);

    //    "'id' INTEGER NOT NULL PRIMARY KEY,"
    //    "'rush_id' INTEGER NOT NULL,"
    //    "'thumbnail' TEXT NOT NULL,"
    //    "'start_time' INTEGER NOT NULL," // ms
    //    "'stop_time' INTEGER NOT NULL," // ms
    //    "'key_moment' INTEGER NOT NULL," // ms
    //    "'rating' INTEGER NOT NULL DEFAULT 0,"
    //    "'comment' TEXT,"

    // build query
    QString querystr = QString("SELECT id, rush_id, thumbnail, "
                               "start_time, stop_time, key_moment, rating, comment "
                               "FROM Extract "
                               "WHERE id = %1 "
                               "LIMIT 1").arg(id);

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // execute and get result
    if (query.first())
    {
        return getExtract(query.record(), rush);
    }


    return res;
}

void Database::updateExtract(Extract *extract)
{
    QSqlQuery query(m_database);

    //    "'id' INTEGER NOT NULL PRIMARY KEY,"
    //    "'rush_id' INTEGER NOT NULL,"
    //    "'thumbnail' TEXT NOT NULL,"
    //    "'start_time' INTEGER NOT NULL," // ms
    //    "'stop_time' INTEGER NOT NULL," // ms
    //    "'key_moment' INTEGER NOT NULL," // ms
    //    "'rating' INTEGER NOT NULL DEFAULT 0,"
    //    "'comment' TEXT,"

    // build query
    QString querystr = QString("UPDATE Extract "
                               "SET start_time=%1, stop_time=%2, key_moment=%3, rating=%4, comment='%5' "
                               "WHERE id = %6 "
                               "").arg(QString::number(extract->start()), QString::number(extract->stop()),
                                       QString::number(extract->key_moment()), QString::number(extract->rating()),
                                       extract->comment(), QString::number(extract->database_id()));

    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();
}

void Database::changeSourceFileName(Rush *rush, const QString &new_file_name)
{
    // if rush not in database return
    if (!rush || rush->database_id < 0)
        return;

    QSqlQuery rename_query(m_database);
    QString querystr = QString("UPDATE Rush SET file_name='%1' WHERE id=%2").arg(new_file_name, QString::number(rush->database_id));
    if (rename_query.exec(querystr))
        rush->file_name = new_file_name;

    qDebug() << rename_query.lastQuery() << rename_query.lastError().text();
}

int Database::addExtract(Extract *extract, qint64 rush_id)
{
//    "'id' INTEGER NOT NULL PRIMARY KEY,"
//    "'rush_id' INTEGER NOT NULL,"
//    "'thumbnail' TEXT NOT NULL,"
//    "'start_time' INTEGER NOT NULL," // ms
//    "'stop_time' INTEGER NOT NULL," // ms
//    "'key_moment' INTEGER NOT NULL," // ms
//    "'rating' INTEGER NOT NULL DEFAULT 0,"
//    "'comment' TEXT,"

    int rush_db_id = rush_id;
    if (extract->rush() != 0)
    {
        rush_db_id = extract->rush()->database_id;
    }

    if (rush_db_id < 0)
        return -1;

    QSqlQuery query(m_database);
    QString querystr = QString("INSERT INTO Extract (rush_id, thumbnail, "
                               "start_time, stop_time, key_moment, rating, "
                               "comment)"
                               " VALUES (%1, '%2', %3, "
                               "%4, %5, %6, '%7')")
            .arg(QString::number(rush_db_id), extract->getThumbnail(),
                 QString::number(extract->start()),
                 QString::number(extract->stop()),
                 QString::number(extract->key_moment()),
                 QString::number(extract->rating()), extract->comment());

    if (query.exec(querystr)) {
        int extract_id = query.lastInsertId().toInt();
        if (extract->database_id() < 0)
            extract->setDatabase_id(extract_id);
        qDebug() << "inserted extract" << extract_id;
        return extract_id;
        //extract->setDatabase_id(id.toInt());
    }

    qDebug() << query.lastQuery() << query.lastError().text();

    return -1;
}

void Database::deleteExtract(Extract *extract)
{
    // retrieve rush id
    qint64 extract_id = extract->database_id();

    if (extract_id < 0)
        return;

    QSqlQuery query(m_database);
    QString querystr;

    // delete associated tags
    querystr = QString("DELETE FROM Tag "
                       " WHERE extract_id = %1")
            .arg(QString::number(extract_id));
    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // delete extract
    querystr = QString("DELETE FROM Extract "
                       " WHERE id = %1")
            .arg(QString::number(extract_id));
    query.exec(querystr);
    qDebug() << query.lastQuery() << query.lastError().text();

    // TODO : clear rush if no extract left
}

Rush Database::getRush(const QSqlRecord &record) const
{
    Rush rush;

    rush.database_id = record.value("id").toLongLong();
    rush.file_name = record.value("file_name").toString();
    rush.thumbnail_file_name = record.value("thumbnail").toString();
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

Extract Database::getExtract(const QSqlRecord &record, Rush *rush) const
{
    Extract extract(rush);

    extract.setDatabase_id(record.value("id").toLongLong());
    extract.setThumbnail(record.value("thumbnail").toString());
    extract.setStart(record.value("start_time").toInt());
    extract.setStop(record.value("stop_time").toInt());
    extract.setKeyMoment(record.value("key_moment").toInt());
    extract.setRating(record.value("rating").toInt());
    extract.setComment(record.value("comment").toString());

    return extract;
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

    // export bins
    out << "<BINS>" << endl;
    if (query.exec("SELECT b.name, parent.name as parent "
                   "FROM Bin b "
                   "LEFT JOIN Bin parent on (b.parent_id = parent.id) "
                   "ORDER BY b.id"))
        exportQuery(query, out);
    else
        qDebug() << query.lastQuery() << query.lastError();



    // export rushs
    out << "<RUSHS>" << endl;
    if (query.exec("SELECT r.*, group_concat(t.name, ',') as tags, group_concat(b.name, ',') as bins "
               "FROM Rush r "
               "LEFT JOIN Tag t on (t.rush_id = r.id) "
               "LEFT JOIN RushBin rb on (rb.rush_id = r.id) "
               "LEFT JOIN Bin b on (b.id = rb.bin_id) "
               "GROUP BY r.id "))
        exportQuery(query, out);
    else
        qDebug() << query.lastQuery() << query.lastError();

    // export extracts
    out << "<EXTRACTS>" << endl;
    if (query.exec("SELECT e.*, r.file_name as rush_file_name, group_concat(t.name, ',') as tags "
               "FROM Extract e "
               "JOIN Rush r on (r.id = e.rush_id) "
               "LEFT JOIN Tag t on (t.extract_id = e.id) "
               "GROUP BY e.id "))
        exportQuery(query, out);
    else
        qDebug() << query.lastQuery() << query.lastError();

    file.close();
}

void Database::importFromCsv(const QString &input_file_name)
{
    qDebug() << "Importing file" << input_file_name;

    QFile file(input_file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "error opening file";
        return;
    }

    QString line = file.readLine().trimmed();

    // get version
    if (line == "<VERSION>")
    {
        line = file.readLine().trimmed();
        qDebug() << "Importing file version" << line;
        while (!file.atEnd() and !line.startsWith("<"))
        {
            line = file.readLine().trimmed();
            qDebug() << line;
        }
    }

    // get bins
//    QMap<QString, QStringList> bins;
    if (line == "<BINS>")
    {
        // retrieve all
        line = file.readLine().trimmed();

        while (!file.atEnd() and !line.startsWith("<"))
        {
            line = file.readLine().trimmed();
            qDebug() << line;

            if (!line.startsWith("<"))
            {
                QString name = line.split(";")[0];
                QString parent = line.split(";")[1];
    //            if (!parent.isEmpty())
    //                bins[parent].append(name);

                qDebug() << "importing bin" << name;

                // as bins are exported by order of id, parents should always be created before their children
                // and this should be sufficient (no need to make sure parent is created first)
                addBin(name, parent);
            }
        }
    }

    // get rush
    if (line == "<RUSHS>")
    {
        // retrieve all
        line = file.readLine().trimmed();
        QStringList headers = line.split(";");

        while (!file.atEnd() and !line.startsWith("<"))
        {
            line = file.readLine().trimmed();

            if (!line.startsWith("<") && line.split(";").size() == headers.size())
            {
                // get rush data
                Rush r;
                r.file_name = line.split(";")[headers.indexOf("file_name")];
                r.thumbnail_file_name = line.split(";")[headers.indexOf("thumbnail")];
                r.rating = line.split(";")[headers.indexOf("rating")].toInt();
                r.length = line.split(";")[headers.indexOf("length")].toInt();
                r.width = line.split(";")[headers.indexOf("width")].toInt();
                r.height = line.split(";")[headers.indexOf("height")].toInt();
                r.fps = line.split(";")[headers.indexOf("fps")].toInt();
                r.bitrate = line.split(";")[headers.indexOf("bitrate")].toInt();
                r.video_codec = line.split(";")[headers.indexOf("video_codec")];
                r.audio_codec = line.split(";")[headers.indexOf("audio_codec")];
                r.sample_rate = line.split(";")[headers.indexOf("sample_rate")].toInt();
                r.channel = line.split(";")[headers.indexOf("channel")];
                r.audio_bitrate = line.split(";")[headers.indexOf("audio_bitrate")].toInt();
                r.utc_creation_time = line.split(";")[headers.indexOf("utc_creation_time")].toLongLong();

                // add rush but don't create an extract, they will be created after
                addRush(&r);

                qDebug() << "importing rush" << r.file_name;

                int rush_id = r.database_id;

                if (rush_id >= 0)
                {
                    // get tags
                    QStringList tags = line.split(";")[headers.indexOf("tags")].split(",");
                    addTagToRush(r, tags);

                    // get associated bins
                    QStringList bins = line.split(";")[headers.indexOf("bins")].split(",");
                    foreach (QString bin, bins)
                        ; // TODO - replace addExtractToBin(&r, bin);
                }
            }

        }
    }

    // get extract
    if (line == "<EXTRACTS>")
    {
        // retrieve all
        line = file.readLine().trimmed();
        QStringList headers = line.split(";");

        while (!file.atEnd() and !line.startsWith("<"))
        {
            line = file.readLine().trimmed();

            if (!line.startsWith("<") && line.split(";").size() == headers.size())
            {
                // get extract data
                Extract e;
                e.setThumbnail(line.split(";")[headers.indexOf("thumbnail")]);
                e.setStop(line.split(";")[headers.indexOf("stop_time")].toInt());
                e.setStart(line.split(";")[headers.indexOf("start_time")].toInt());
                e.setKeyMoment(line.split(";")[headers.indexOf("key_moment")].toInt());
                e.setComment(line.split(";")[headers.indexOf("comment")]);
                e.setRating(line.split(";")[headers.indexOf("rating")].toInt());

                QString rush_file_name = line.split(";")[headers.indexOf("rush_file_name")];
                Rush r = getRush(rush_file_name);

                qDebug() << "importing extract" << r.file_name << e.database_id() << e.start() << e.stop() << e.key_moment() << e.comment();

                addExtract(&e, r.database_id);

                if (e.database_id() >= 0)
                {
                    // get tags
                    QStringList tags = line.split(";")[headers.indexOf("tags")].split(",");
                    addTagToExtract(e, tags);
                }
            }

        }
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


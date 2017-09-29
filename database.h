#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlRecord>

#include "mediainfo.h"

class Database
{ 
public:
    static const int MAX_RATING = 5;

    Database();
    ~Database();

    QSqlDatabase& sqlDatabase() { return m_database; }

    void addCatalog(const QString& catalog);
    void addVideo(const MediaInfo& media, const QString& catalog = "default");

    MediaInfo getVideo(const QString& filename) const;
    QStringList getVideos() const;

    void createVideoTable();
    void createCatalogTable();
    void createTagTable();

    static MediaInfo getMediaInfo(const QSqlRecord& record);

private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H

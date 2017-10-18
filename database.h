#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QPair>

#include "mediainfo.h"

class Database
{ 
public:
    static const int MAX_RATING = 5;

    Database();
    ~Database();

    QSqlDatabase& sqlDatabase() { return m_database; }

    void addCatalog(const QString& catalog, const QString& parent = "");
    void deleteCatalog(const QString& catalog);
    QStringList getCatalogChildren(int catalog_id);
    void catalogRush(const QString& catalog_name, const MediaInfo& media);
    void removeRushFromCatalog(const QString& catalog_name, const MediaInfo& media);

    void addVideo(const MediaInfo& media, const QString& catalog = "default");
    void addTagToRush(const MediaInfo& media, QStringList tags);
    QStringList getRushTags(qint64 rush_id) const;

    QStringList catalogs(const QString& parent_name) const;

    qint64 getIdFromAttributeValue(const QString& table_name, const QString& attr_name, const QString& attr_value) const;

    MediaInfo getVideo(const QString& filename) const;

    void createRushTable();
    void createCatalogTable();
    void createTagTable();
    void createRushCatalogTable();

    static MediaInfo getMediaInfo(const QSqlRecord& record);

private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H

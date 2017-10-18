#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QPair>

#include "rush.h"

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
    void catalogRush(const QString& catalog_name, const Rush& rush);
    void removeRushFromCatalog(const QString& catalog_name, const Rush& rush);

    void addVideo(const Rush& rush, const QString& catalog = "default");
    void addTagToRush(const Rush& rush, QStringList tags);
    QStringList getRushTags(qint64 rush_id) const;

    QStringList catalogs(const QString& parent_name) const;

    qint64 getIdFromAttributeValue(const QString& table_name, const QString& attr_name, const QString& attr_value) const;

    Rush getVideo(const QString& filename) const;

    void createRushTable();
    void createCatalogTable();
    void createTagTable();
    void createRushCatalogTable();

    static Rush getRush(const QSqlRecord& record);

    void exportToCsv(const QString& output_file_name);
    void importFromCsv(const QString& input_file_name);

private:
    QSqlDatabase m_database;

    void exportQuery(QSqlQuery& query, QTextStream& text_stream);
};

#endif // DATABASE_H

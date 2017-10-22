#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QPair>
#include <QTextStream>

#include "rush.h"

class Database
{ 
public:
    static const int MAX_RATING = 5;

    Database();
    ~Database();

    QSqlDatabase& sqlDatabase() { return m_database; }

    void addBin(const QString& bin_name, const QString& parent_name = "");
    void deleteBin(const QString& bin_name);
    QStringList getBinChildren(int bin_id);
    void addRushToBin(const QString& bin_name, const Rush& rush);
    void removeRushFromBin(const QString& bin_name, const Rush& rush);

    void addRushToBin(const Rush& rush, const QString& bin = "All");
    void addTagToRush(const Rush& rush, QStringList tags);
    QStringList getRushTags(qint64 rush_id) const;

    QStringList bins(const QString& parent_name) const;

    qint64 getIdFromAttributeValue(const QString& table_name, const QString& attr_name, const QString& attr_value) const;

    Rush getRush(const QString& filename) const;

    void createRushTable();
    void createBinTable();
    void createTagTable();
    void createRushBinTable();

    void exportToCsv(const QString& output_file_name);
    void importFromCsv(const QString& input_file_name);

protected:
    Rush getRush(const QSqlRecord& record) const;

private:
    QSqlDatabase m_database;

    void exportQuery(QSqlQuery& query, QTextStream& text_stream);
};

#endif // DATABASE_H

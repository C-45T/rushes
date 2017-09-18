#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

class Database
{ 
public:
    static const int MAX_RATING = 5;

    Database();
    ~Database();

    QSqlDatabase& sqlDatabase() { return m_database; }

    void addCatalog(const QString& catalog);
    void addVideo(const QString& path, const QString& catalog = "default");

    QStringList getVideos() const;

    void createVideoTable();
    void createCatalogTable();
    void createTagTable();

private:
    QSqlDatabase m_database;
};

#endif // DATABASE_H

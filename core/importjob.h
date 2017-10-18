#ifndef IMPORTJOB_H
#define IMPORTJOB_H

#include "core/jobsmaster.h"
#include "data/database.h"

class ImportJob : public Job
{
    Q_OBJECT
public:
    ImportJob(Database &db, const QStringList& filenames, const QString& to_catalog = "defulat");

    void run();

private:
    QStringList m_import_list;
    QString m_catalog;

    Database& m_db;

};

#endif // IMPORTJOB_H

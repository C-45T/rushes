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

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QPair>
#include <QTextStream>

#include "rush.h"
#include "extract.h"

class Database
{ 
public:
    static const int MAX_RATING = 5;

    Database();
    ~Database();

    QSqlDatabase& sqlDatabase() { return m_database; }

    // Tables
    void createRushTable();
    void createBinTable();
    void createTagTable();
    void createRushBinTable();
    void createExtractTable();

    // Rushs
    void addRush(Rush *rush);
    void deleteRush(Rush *rush);
    Rush getRush(const QString& filename) const;
    QList<qint64> getRushExtracts(qint64 rush_id) const;
    void changeSourceFileName(Rush *rush, const QString& new_file_name);

    // Extract
    void addExtract(Extract *extract);
    Extract getExtract(int64_t &id, Rush *rush) const;
    void updateExtract(Extract *extract);

    // Bins
    void addBin(const QString& bin_name, const QString& parent_name = "");
    void deleteBin(const QString& bin_name);
    QStringList getBinChildren(int bin_id);
    QStringList bins(const QString& parent_name) const;

    // Rush Bin link
    void removeRushFromBin(const QString& bin_name, const Rush& rush);
    void addRushToBin(Rush *rush, const QString& bin_name = "All");

    // Tags
    void addTagToRush(const Rush& rush, QStringList tags);
    QStringList getRushTags(qint64 rush_id) const;

    // Rating
    void setRushRating(Rush *rush, int rating);

    // generic : TODO : move to private
    qint64 getIdFromAttributeValue(const QString& table_name, const QString& attr_name, const QString& attr_value) const;
    QString getAttributeValueFromId(const QString& table_name, const QString& attr_name, qint64 id) const;

    // import/export
    void exportToCsv(const QString& output_file_name);
    void importFromCsv(const QString& input_file_name);

protected:
    Rush getRush(const QSqlRecord& record) const;
    Extract getExtract(const QSqlRecord& record, Rush *rush) const;

private:
    QSqlDatabase m_database;

    void exportQuery(QSqlQuery& query, QTextStream& text_stream);
};

#endif // DATABASE_H

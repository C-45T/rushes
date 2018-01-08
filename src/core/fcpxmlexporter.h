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
#ifndef FCPXMLEXPORTER_H
#define FCPXMLEXPORTER_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QList>
#include <QMap>

#include "data/extract.h"

class FCPXmlExporter : public QObject
{
    Q_OBJECT
public:
    explicit FCPXmlExporter(QObject *parent = nullptr);

    void exportTo(const QString &file_name, QList<Extract *> extracts, const QString &music_file_name);

    void setExtracts(const QList<Extract *> &extracts);
    void setOnset_times(const QList<int> &onset_times);

signals:

public slots:

private:
    void writeRessources(QXmlStreamWriter &stream, QList<Extract *> extracts, const QString& audio_file_name = "");
    void writeSequence(QXmlStreamWriter &stream, QList<Extract *> extracts);
    void writeSyncedSequence(QXmlStreamWriter &stream, QList<Extract *> extracts, const QString& audio_file_name = "");

    QMap<QString,int> m_format_indexes;
    QList<int> m_onset_times;
    QList<Extract*> m_extracts;
};

#endif // FCPXMLEXPORTER_H

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
#include "fcpxmlexporter.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "ffmpegparser.h"

#include "core/onsetdetector.h"

FCPXmlExporter::FCPXmlExporter(QObject *parent) : QObject(parent)
{
}

void FCPXmlExporter::exportTo(const QString &file_name, QList<Extract*> extracts)
{
    QFileInfo file_info(file_name);
    qDebug() << "FCPXmlExporter - export file" << file_info.absoluteFilePath();

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    // Get onset times
    OnSetDetector test("08 - When The Ship Comes In.mp3");
    m_onset_times = test.getOnsetTimes();

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("fcpxml");
    stream.writeAttribute("version", "1.5");

    stream.writeStartElement("resources");
    writeRessources(stream, extracts);
    stream.writeEndElement(); // resources

    stream.writeStartElement("library");
    stream.writeStartElement("event");
    stream.writeAttribute("name", "Timeline 1 (resolve)");
    stream.writeStartElement("project");
    stream.writeAttribute("name", "Timeline 1 (resolve)");
    //writeSequence(stream, extracts);
    writeSyncedSequence(stream, extracts);
    stream.writeEndElement(); // project
    stream.writeEndElement(); // event
    stream.writeEndElement(); // library


    stream.writeEndElement(); // fcpxml

    stream.writeEndDocument();

    file.close();
}

void FCPXmlExporter::writeRessources(QXmlStreamWriter &stream, QList<Extract*> extracts)
{
    int format_index = extracts.size();

    // set format elements
    foreach(Extract *extract, extracts)
    {
        Rush *rush = extract->rush();
        if (rush == 0)
            continue;

        QString format_key = QString("%1-%2").arg(QString::number(rush->height), QString::number(rush->fps));

        if (!m_format_indexes.contains(format_key))
        {
            qDebug() << "FCPXmlExporter::writeRessources - write format" << format_key;

            m_format_indexes[format_key] = format_index;
            stream.writeStartElement("format");
            stream.writeAttribute("width", QString("%1").arg(QString::number(rush->width)));
            stream.writeAttribute("height", QString("%1").arg(QString::number(rush->height)));
            stream.writeAttribute("frameDuration", QString("1/%1s").arg(QString::number(rush->fps)));
            stream.writeAttribute("id", QString("r%1").arg(QString::number(format_index)));

            if (rush->fps == 119 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormat1080p11988");
            else if (rush->fps == 29 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormat1080p2997");
            else
                stream.writeAttribute("name", QString("FFVideoFormat%1p%2").arg(QString::number(rush->height), QString::number(rush->fps)));

            stream.writeEndElement();
            format_index++;
        }
    }

    int index = 0;
    int onset_index = 1;

    // set asset elements
    foreach(Extract *extract, extracts)
    {
        Rush *rush = extract->rush();
        if (rush == 0)
            continue;

        qDebug() << "FCPXmlExporter::writeRessources - write asset" << rush->file_name;

        Rush dummy;
        FFMpegParser::getMetaData(rush->file_name, dummy);

        QFileInfo file_info(rush->file_name);
        QString format_key = QString("%1-%2").arg(QString::number(rush->height), QString::number(rush->fps));

        qDebug() << rush->length << rush->fps << float(rush->length * rush->fps) << float(rush->length * rush->fps) / 1000.0;
        //int total_frames = (float(rush->length * rush->fps) / 1000.0);

        int sequence_length = extract->max_length();
        if (onset_index < m_onset_times.size()) {
            sequence_length = m_onset_times[onset_index] - m_onset_times[onset_index-1];
            qDebug() << "FCPXmlExporter::writeRessources - sequence_length"
                     << sequence_length
                     << m_onset_times[onset_index-1] << m_onset_times[onset_index];
        }
        else {
            qDebug() << "FCPXmlExporter::writeRessources - sequence_length" << sequence_length;
        }

        stream.writeStartElement("asset");
        stream.writeAttribute("hasVideo", "1");
        stream.writeAttribute("hasAudio", "1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(sequence_length), QString::number(1000)));
        stream.writeAttribute("start", "0/25s");
        stream.writeAttribute("name", file_info.fileName());
        stream.writeAttribute("src", QString("file://localhost/%1").arg(file_info.absoluteFilePath()));
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("id", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement();
        index++;
        onset_index++;
    }

}

void FCPXmlExporter::writeSequence(QXmlStreamWriter &stream, QList<Extract *> extracts)
{
    int64_t sequence_total_duration = 0;

    stream.writeStartElement("sequence");

    stream.writeStartElement("spine");

    int index = 0;

    foreach(Extract *extract, extracts)
    {
        Rush *rush = extract->rush();
        if (rush == 0)
            continue;

        qDebug() << "FCPXmlExporter::writeSequence - write clip" << rush->file_name;

        QFileInfo file_info(rush->file_name);
        QString format_key = QString("%1-%2").arg(QString::number(rush->height), QString::number(rush->fps));

        //int total_frames = (float(rush->length * rush->fps) / 1000.0);

        stream.writeStartElement("clip");
        stream.writeAttribute("offset", QString("%1/%2s").arg(QString::number(sequence_total_duration), QString::number(1000)));
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(extract->max_length()), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");
        stream.writeAttribute("name", file_info.fileName());
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("toFormat", "NDF");

        stream.writeStartElement("video");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(extract->max_length()), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");

        stream.writeStartElement("audio");
        stream.writeAttribute("lane", "-1");
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(extract->max_length()), QString::number(1000)));
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement(); // audio

        stream.writeEndElement(); // video

        stream.writeEndElement(); // clip

        sequence_total_duration += extract->max_length();
        index++;
    }

     stream.writeEndElement(); // spine

     stream.writeEndElement(); // sequence
}

void FCPXmlExporter::writeSyncedSequence(QXmlStreamWriter &stream, QList<Extract *> extracts)
{
    int64_t sequence_total_duration = 0;

    stream.writeStartElement("sequence");

    stream.writeStartElement("spine");

    int index = 0;

    int onset_index = 1;

    foreach(Extract *extract, extracts)
    {
        Rush *rush = extract->rush();
        if (rush == 0)
            continue;

        qDebug() << "FCPXmlExporter::writeSequence - write clip" << rush->file_name;

        QFileInfo file_info(rush->file_name);
        QString format_key = QString("%1-%2").arg(QString::number(rush->height), QString::number(rush->fps));

        int sequence_length = extract->max_length();
        if (onset_index < m_onset_times.size())
            sequence_length = m_onset_times[onset_index] - m_onset_times[onset_index-1];

        stream.writeStartElement("clip");
        stream.writeAttribute("offset", QString("%1/%2s").arg(QString::number(sequence_total_duration), QString::number(1000)));
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(sequence_length), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");
        stream.writeAttribute("name", file_info.fileName());
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("toFormat", "NDF");

        stream.writeStartElement("video");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(sequence_length), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");

        stream.writeStartElement("audio");
        stream.writeAttribute("lane", "-1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(sequence_length), QString::number(1000)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement(); // audio

        stream.writeEndElement(); // video

        stream.writeEndElement(); // clip

        sequence_total_duration += sequence_length;
        index++;
        onset_index++;
    }

     stream.writeEndElement(); // spine

     stream.writeEndElement(); // sequence
}

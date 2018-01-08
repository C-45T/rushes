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
#include "core/smartselector.h"

FCPXmlExporter::FCPXmlExporter(QObject *parent) : QObject(parent)
{
}

void FCPXmlExporter::exportTo(const QString &file_name, QList<Extract*> extracts, const QString& music_file_name)
{
    QFileInfo file_info(file_name);
    qDebug() << "FCPXmlExporter - export file" << file_info.absoluteFilePath();

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    // Get onset times
    OnSetDetector test(music_file_name);
    m_onset_times = test.getOnsetTimes();

    // build smart editing
    SmartSelector edition(m_onset_times, extracts);
    edition.setShortestSequenceDuration(500);
    m_extracts = edition.getEditing();
    m_onset_times = edition.getOnsetTimes();

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("fcpxml");
    stream.writeAttribute("version", "1.5");

    stream.writeStartElement("resources");
    writeRessources(stream, m_extracts, music_file_name);
    //writeRessources(stream, extracts);

    stream.writeEndElement(); // resources

    stream.writeStartElement("library");
    stream.writeStartElement("event");
    stream.writeAttribute("name", "Timeline 1 (resolve)");
    stream.writeStartElement("project");
    stream.writeAttribute("name", "Timeline 1 (resolve)");
    //writeSequence(stream, extracts);
    writeSyncedSequence(stream, m_extracts, music_file_name);
    stream.writeEndElement(); // project
    stream.writeEndElement(); // event
    stream.writeEndElement(); // library


    stream.writeEndElement(); // fcpxml

    stream.writeEndDocument();

    file.close();
}

void FCPXmlExporter::writeRessources(QXmlStreamWriter &stream, QList<Extract*> extracts, const QString& audio_file_name)
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
            if (rush->fps == 29)
                stream.writeAttribute("frameDuration", QString("1001/30000s"));
            else if (rush->fps == 23)
                stream.writeAttribute("frameDuration", QString("1001/24000s"));
            else if (rush->fps == 14)
                stream.writeAttribute("frameDuration", QString("1/15"));
            else
                stream.writeAttribute("frameDuration", QString("1/%1s").arg(QString::number(rush->fps)));
            stream.writeAttribute("id", QString("r%1").arg(QString::number(format_index)));

            if (rush->fps == 119 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormat1080p11988");
            else if (rush->fps == 29 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormat1080p2997");
            else if (rush->fps == 23 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormat1080p2398");
            else if (rush->fps < 20 && rush->height == 1080)
                stream.writeAttribute("name", "FFVideoFormatRateUndefined");
            else if (rush->fps == 29 && rush->height == 2160)
                stream.writeAttribute("name", "FFVideoFormat3840x2160p2997");
            else if (rush->height == 2160)
                stream.writeAttribute("name", QString("FFVideoFormat3840x2160p%1").arg(QString::number(rush->fps)));
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

//        int rush_length = rush->length / 10;
//        rush_length = ((rush_length + 2) / 4) * 4;
//        rush_length = rush_length*10;

        stream.writeStartElement("asset");
        stream.writeAttribute("hasVideo", "1");
        stream.writeAttribute("hasAudio", "1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(rush->length), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");
        stream.writeAttribute("name", file_info.fileName());
        stream.writeAttribute("src", QString("file://localhost/%1").arg(file_info.absoluteFilePath()));
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("id", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement();
        index++;
        onset_index++;
    }

    if (!audio_file_name.isEmpty())
    {
        Rush audio_rush;
        FFMpegParser::getMetaData(audio_file_name, audio_rush);

        QFileInfo file_info(audio_file_name);

        qDebug() << "FCPXmlExporter::writeAudioRessource" << audio_rush.length << audio_rush.audio_codec;

        stream.writeStartElement("asset");
        stream.writeAttribute("hasAudio", "1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(audio_rush.length), QString::number(1000)));
        stream.writeAttribute("start", QString("0/1s"));
        stream.writeAttribute("name", file_info.fileName());
        stream.writeAttribute("src", QString("file://localhost/%1").arg(file_info.absoluteFilePath()));
        // this should never fail by construction, should we check ?
        stream.writeAttribute("id", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement();
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
        stream.writeAttribute("start", QString("%1/%2s").arg(QString::number(extract->start()), QString::number(1000)));
        stream.writeAttribute("name", file_info.fileName());
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("toFormat", "NDF");

        stream.writeStartElement("adjust-conform");
        stream.writeAttribute("type", "fit");
        stream.writeEndElement(); // "adjust-conform"

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

void FCPXmlExporter::writeSyncedSequence(QXmlStreamWriter &stream, QList<Extract *> extracts, const QString& audio_file_name)
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
        int sequence_start = 0;
        if (onset_index < m_onset_times.size())
        {
            //sequence_length = m_onset_times[onset_index] - m_onset_times[onset_index-1];
            sequence_length = m_onset_times[onset_index] - sequence_total_duration;

            //recalculated_length = sequence_length;

//            if (rush->fps != 25 && rush->fps != 50)
//            {
//                int frame_time = round(1000/rush->fps);
//                sequence_length = frame_time * round(( sequence_length + (frame_time / 2) ) / frame_time);
//            }

            sequence_start = extract->key_moment() - sequence_length / 2;
            if (sequence_start < 0)
                sequence_start = 0;
            if (sequence_start + sequence_length > extract->stop())
                sequence_start = extract->stop() - sequence_length;

            //sequence_start = round((sequence_start * rush->fps) / 1000);
        }

        stream.writeStartElement("clip");
        stream.writeAttribute("offset", QString("%1/%2s").arg(QString::number(sequence_total_duration), QString::number(1000)));
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(sequence_length), QString::number(1000)));
        stream.writeAttribute("start", QString("%1/%2s").arg(QString::number(sequence_start), QString::number(1000)));
        stream.writeAttribute("name", file_info.fileName());
        // this should never fail by construction, should we check ?
        stream.writeAttribute("format", QString("r%1").arg(QString::number(m_format_indexes[format_key])));
        stream.writeAttribute("toFormat", "NDF");

        stream.writeStartElement("adjust-conform");
        stream.writeAttribute("type", "fit");
        stream.writeEndElement(); // "adjust-conform"

        stream.writeStartElement("video");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(rush->length), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");

        stream.writeStartElement("audio");
        stream.writeAttribute("lane", "-1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(rush->length), QString::number(1000)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement(); // audio

        stream.writeEndElement(); // video

        stream.writeEndElement(); // clip

        sequence_total_duration += sequence_length;
        index++;
        onset_index++;
    }

    if (!audio_file_name.isEmpty())
    {
        Rush audio_rush;
        FFMpegParser::getMetaData(audio_file_name, audio_rush);

        QFileInfo file_info(audio_file_name);

        qDebug() << "FCPXmlExporter::writeAudioRessource" << audio_rush.length << audio_rush.audio_codec;

        stream.writeStartElement("clip");
        stream.writeAttribute("lane", "2");
        stream.writeAttribute("offset", QString("0/1s"));
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(audio_rush.length), QString::number(1000)));
        stream.writeAttribute("start", "0/1s");
        stream.writeAttribute("name", file_info.fileName());
        // this should never fail by construction, should we check ?

        stream.writeStartElement("audio");
        stream.writeAttribute("lane", "-1");
        stream.writeAttribute("duration", QString("%1/%2s").arg(QString::number(audio_rush.length), QString::number(1000)));
        stream.writeAttribute("offset", "0/25s");
        stream.writeAttribute("ref", QString("r%1").arg(QString::number(index)));
        stream.writeEndElement(); // audio

        stream.writeEndElement(); // clip
    }

     stream.writeEndElement(); // spine

     stream.writeEndElement(); // sequence
}

void FCPXmlExporter::setOnset_times(const QList<int> &onset_times)
{
    m_onset_times = onset_times;
}

void FCPXmlExporter::setExtracts(const QList<Extract *> &extracts)
{
    m_extracts = extracts;
}



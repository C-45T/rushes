/****************************************************************************
 *
 * Rushes! is a video cataloger application based on QtAv, OpenCV and FFMpeg.
 * Copyright (C) 2018 Remy Ruttner
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
#include "autoeditjob.h"

#include "core/onsetdetector.h"
#include "core/smartselector.h"
#include "core/fcpxmlexporter.h"

AutoEditJob::AutoEditJob(const QString &xml_output_file_name, QList<Extract *> extracts, const QString &music_file_name, bool prores_export)
    : m_extracts(extracts)
    , m_music_file_name(music_file_name)
    , m_xml_output_file_name(xml_output_file_name)
    , m_prores_export(prores_export)
{
    // configuration
    // time of shortest sequence
    // export to prores before editing
    // output folder
    // output file name
    // music song
    // cover all track

    setDescription(QString("Auto editing of %1 extracts using music %2").arg(QString::number(extracts.size()), m_music_file_name));
    setStatus(Job::PENDING);
}

void AutoEditJob::run()
{
    setStatus(Job::STARTED);

    // Get onset times
    setDescription(QString("Compute onset times of %1").arg(m_music_file_name));
    emit progress(1, 100);
    OnSetDetector onset_detector(m_music_file_name);
    QList<int> onset_times = onset_detector.getOnsetTimes();

    // build smart editing
    setDescription(QString("Perform auto editing based on %1 selected extracts and %2 onset times").arg(QString::number(m_extracts.size()), QString::number(onset_times.size())));
    emit progress(70, 100);
    SmartSelector edition(onset_times, m_extracts);
    edition.setShortestSequenceDuration(500);
    QList<Extract*> extracts_edition = edition.getEditing();
    onset_times = edition.getOnsetTimes();

    // export to fcp xml
    setDescription(QString("Export editing to %1").arg(m_xml_output_file_name));
    emit progress(90, 100);
    FCPXmlExporter exporter;
    exporter.useTranscodedFiles(m_prores_export);
    exporter.setExtracts(extracts_edition);
    exporter.setOnsetTimes(onset_times);

    exporter.exportTo(m_xml_output_file_name, m_music_file_name);

    emit progress(100, 100);
    setStatus(Job::FINISHED);
}

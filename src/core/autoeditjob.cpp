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

AutoEditJob::AutoEditJob(const QString &xml_output_file_name, QList<Extract *> extracts, const QString &music_file_name)
    : m_extracts(extracts)
    , m_music_file_name(music_file_name)
    , m_xml_output_file_name(xml_output_file_name)
{
//    // Get onset times
//    OnSetDetector test(music_file_name);
//    m_onset_times = test.getOnsetTimes();

//    // build smart editing
//    SmartSelector edition(m_onset_times, extracts);
//    edition.setShortestSequenceDuration(500);
//    m_extracts = edition.getEditing();
    //    m_onset_times = edition.getOnsetTimes();
}

void AutoEditJob::run()
{

}

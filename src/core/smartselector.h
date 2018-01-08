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
#ifndef SMARTSELECTOR_H
#define SMARTSELECTOR_H

#include <QMap>

#include "data/extract.h"

class SmartSelector
{
public:
    SmartSelector(QList<int> onset_times, QList<Extract*> extracts);

    QList<Extract*> getEditing();
    void setShortestSequenceDuration(int duration);

    QList<int> getOnsetTimes() const;

protected:
    QPair<int, int> getMaxUnaffectedDuration();
    QPair<int, int> getMaxLengthExtractIndexAndDuration();
    QList<int> getCandidatesIndexes(QPair<int, int> onset_time);
    int getBestCandidate(QList<int> extract_indexes);

    void removeShortestDuration();
    void computeDurations();

private:
    QList<Extract*> m_extracts;
    QList<int> m_onset_times;
    QMap<int,int> m_durations;
    QMap<int, int> m_onset_extract;
    int m_shortest_duration_index;
};

#endif // SMARTSELECTOR_H

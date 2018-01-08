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
#include "smartselector.h"

#include <QPair>
#include <QDebug>

SmartSelector::SmartSelector(QList<int> onset_times, QList<Extract*> extracts) :
    m_extracts(extracts),
    m_onset_times(onset_times)
{
    computeDurations();
}

void SmartSelector::computeDurations()
{
    if (m_onset_times.size() == 0)
        return;

    int shortest_duration = m_onset_times[1];
    m_shortest_duration_index = 0;
    m_durations.clear();

    // compute every sequence duration and store index of the shortest
    for (int i=1; i<m_onset_times.size() && i< m_extracts.size() + 1 ; i++)
    {
        int duration = m_onset_times[i] - m_onset_times[i-1];
        m_durations[i-1] = duration;
        if (duration < shortest_duration)
        {
            shortest_duration = duration;
            m_shortest_duration_index = i-1;
        }
    }

    // if shortest sequence is the last, index to remove should be the previous one
    if (m_shortest_duration_index == m_onset_times.size()-1)
        m_shortest_duration_index--;
}

QList<int> SmartSelector::getOnsetTimes() const
{
    return m_onset_times;
}


QList<Extract *> SmartSelector::getEditing()
{
    while (m_onset_extract.size() < m_extracts.size() && m_onset_extract.size() < m_durations.size())
    {
        // find longest sequence
        QPair<int, int> longest_sequence = getMaxUnaffectedDuration();

        qDebug() << "longest sequence" << longest_sequence.first << longest_sequence.second;

        // pick candidates
        QList<int> candidates = getCandidatesIndexes(longest_sequence);
        int pick = getBestCandidate(candidates);

        qDebug() << " pick candidates" << candidates << pick;

        m_onset_extract[longest_sequence.first] = pick;
    }

    QList<Extract*> editing;
    // build list of extract in selected order
    foreach (int index, m_onset_extract.keys()) {
        editing.append(m_extracts[m_onset_extract[index]]);
    }

    return editing;
}

void SmartSelector::setShortestSequenceDuration(int duration)
{
    while (m_durations[m_shortest_duration_index] < duration)
        removeShortestDuration();
}

QPair<int, int> SmartSelector::getMaxUnaffectedDuration()
{
    QPair<int, int> max_duration = QPair<int, int>(-1, 0);

    for (int i=0; i<m_durations.size(); i++)
    {
        // ignore already affected durations
        if (m_onset_extract.keys().contains(i))
            continue;

        // check if this one is longer
        if (m_durations[i] > max_duration.second)
            max_duration = QPair<int, int>(i, m_durations[i]);
    }

    return max_duration;
}

QPair<int, int> SmartSelector::getMaxLengthExtractIndexAndDuration()
{
    QPair<int, int> max_duration = QPair<int, int>(-1, 0);

    for (int i=0; i<m_extracts.size(); i++)
    {
        // ignore already affected durations
        if (m_onset_extract.values().contains(i))
            continue;

        // check if this one is longer
        if (m_extracts[i]->max_length() > max_duration.second)
            max_duration = QPair<int, int>(i, m_extracts[i]->max_length());
    }

    return max_duration;
}

QList<int> SmartSelector::getCandidatesIndexes(QPair<int, int> onset_time)
{
    QList<int> selection;

    for (int i=0; i<m_extracts.size(); i++)
    {
        // ignore already affected durations
        if (m_onset_extract.values().contains(i))
            continue;

        // check if this one is longer
        if (m_extracts[i]->max_length() > onset_time.second)
            selection.append(i);
    }

    return selection;
}

int SmartSelector::getBestCandidate(QList<int> extract_indexes)
{
    int best_candidate = extract_indexes[0];
    for (int i=1; i< extract_indexes.size(); i++)
    {
        // for now just find the extract with the best rating
        if (m_extracts[extract_indexes[i]]->rating() > m_extracts[best_candidate]->rating())
            best_candidate = extract_indexes[i];
    }

    return best_candidate;
}

void SmartSelector::removeShortestDuration()
{
    m_onset_times.removeAt(m_shortest_duration_index);
    computeDurations();
}


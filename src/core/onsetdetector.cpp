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
#include "onsetdetector.h"

#include <QProcess>
#include <QDebug>

OnSetDetector::OnSetDetector(const QString &file_name)
{
    m_file_name = file_name;
}

QList<int> OnSetDetector::getOnsetTimes()
{
    computeOnsetRateTicks();
    computeRythmTicks();

    // get intersection of rythm and onset ticks
    m_onset_ticks_set.intersect(m_rythm_ticks_set);

    // copy to a list and order it
    QList<int> ordered_ticks = m_onset_ticks_set.toList();
    ordered_ticks.prepend(0);
    std::sort(ordered_ticks.begin(), ordered_ticks.end());

    qDebug() << "inersected :" << ordered_ticks;

    return ordered_ticks;
}

QSet<int> OnSetDetector::computeRythmTicks()
{
    QProcess *m_detection_process = new QProcess();

    m_detection_process->start("streaming_rhythmextractor_multifeature.exe", QStringList() << m_file_name);
    if (!m_detection_process->waitForStarted())
    {
        qDebug() << "streaming_rhythmextractor_multifeature error starting" << m_detection_process->readAll();
        return QSet<int>();
    }

    if (!m_detection_process->waitForFinished(120000))
    {
        qDebug() << "streaming_rhythmextractor_multifeature error waiting" << m_detection_process->readAll();
        return QSet<int>();
    }

    QByteArray result = m_detection_process->readAll();

    //qDebug() << result;

    int left_bracket = 0;
    int right_bracket = 0;

    int confidence_index = 0;
    confidence_index = result.lastIndexOf("confidence");

    int bpm_index = 0;
    bpm_index = result.indexOf("bpm:");

    left_bracket = result.indexOf("[", result.indexOf("ticks"));
    right_bracket = result.indexOf("]", left_bracket);
    QByteArray ticks = result.mid(left_bracket+1, right_bracket-left_bracket-1);

    left_bracket = result.indexOf("[", right_bracket);
    right_bracket = result.indexOf("]", left_bracket);
    QByteArray estimates = result.mid(left_bracket+1, right_bracket-left_bracket-1);

    left_bracket = result.indexOf("[", right_bracket);
    right_bracket = result.indexOf("]", left_bracket);
    QByteArray bpmintervals = result.mid(left_bracket+1, right_bracket-left_bracket-1);

    QString confidence = result.mid(confidence_index, result.indexOf('\n', confidence_index) - confidence_index).trimmed();

    QString bpm = result.mid(bpm_index, result.indexOf('\n', bpm_index) - bpm_index).trimmed();

    //qDebug() << "confidence" << confidence.split(':').last().trimmed();
    qDebug() << "ticks" << ticks.split(',').size() << ticks;
    qDebug() << "bpm" << bpm;
    //qDebug() << "estimates" << estimates.split(',').size() << estimates;
    //qDebug() << "bpmintervals" << bpmintervals.split(',').size() << bpmintervals;

    m_rythm_ticks_set.clear();
    foreach (QString tick, ticks.split(','))
    {
        m_rythm_ticks_set.insert(conformed_tick(tick.trimmed().toFloat()));
    }

    m_bpm = bpm.toInt();

    return m_rythm_ticks_set;
}

QSet<int> OnSetDetector::computeOnsetRateTicks()
{
    QProcess *m_detection_process = new QProcess();

    m_detection_process->start("standard_onsetrate.exe", QStringList() << m_file_name);
    if (!m_detection_process->waitForStarted())
    {
        qDebug() << "standard_onsetrate error starting" << m_detection_process->readAll();
        return QSet<int>();
    }

    if (!m_detection_process->waitForFinished(120000))
    {
        qDebug() << "standard_onsetrate error waiting" << m_detection_process->readAll();
        return QSet<int>();
    }

    QByteArray result = m_detection_process->readAll();

    int left_bracket = 0;
    int right_bracket = 0;

    left_bracket = result.indexOf("[", result.indexOf("onsetTimes"));
    right_bracket = result.indexOf("]", left_bracket);
    QByteArray onset_times = result.mid(left_bracket+1, right_bracket-left_bracket);

    m_onset_ticks_set.clear();
    foreach (QString onset_time, onset_times.split(','))
    {
        m_onset_ticks_set.insert(conformed_tick(onset_time.trimmed().toFloat()));
    }

    return m_onset_ticks_set;
}

/*
 * Take a time like 2.6789 and return a time in ms multiple of 20
 */
int OnSetDetector::conformed_tick(float tick)
{
    int rounded = round(tick * 100.0);
//    if (rounded % 2 != 0)
//        rounded += 1;

    rounded = ((rounded + 2) / 4) * 4;
    return rounded*10;
}

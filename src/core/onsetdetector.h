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
#ifndef ONSETDETECTOR_H
#define ONSETDETECTOR_H

#include <QObject>

class OnSetDetector : public QObject
{
    Q_OBJECT
public:
    OnSetDetector(const QString& file_name);

    QList<int> getOnsetTimes();

protected:
    int conformed_tick(float tick);

private:
    QString m_file_name;
};

#endif // ONSETDETECTOR_H

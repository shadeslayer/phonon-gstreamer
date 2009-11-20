/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), Nokia Corporation (or its successors, 
    if any) and the KDE Free Qt Foundation, which shall act as a proxy 
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "backendcapabilities.h"
#include "globalconfig_p.h"

namespace Phonon
{
namespace Experimental
{

QList<VideoCaptureDevice> BackendCapabilities::availableVideoCaptureDevices()
{
    QList<VideoCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().videoCaptureDeviceListFor(Phonon::NoCategory);
    foreach (int i, deviceIndexes) {
        ret.append(VideoCaptureDevice::fromIndex(i));
    }
    return ret;
}

bool BackendCapabilities::getHideAdvancedDevices()
{
    return GlobalConfig().getHideAdvancedDevices();
}

void BackendCapabilities::hideAdvancedDevices(bool hide)
{
    GlobalConfig().hideAdvancedDevices(hide);
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevicesForCategory(Phonon::Category category)
{
    QList<AudioOutputDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioOutputDeviceListFor(category);
    foreach (int i, deviceIndexes) {
        ret.append(AudioOutputDevice::fromIndex(i));
    }
    return ret;
}

QList<AudioCaptureDevice> BackendCapabilities::availableAudioCaptureDevicesForCategory(Phonon::Category category)
{
    QList<AudioCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioCaptureDeviceListFor(category);
    foreach (int i, deviceIndexes) {
        ret.append(AudioCaptureDevice::fromIndex(i));
    }
    return ret;
}

static QList<int> reindexList(QList<int> currentList, QList<int>newOrder, bool output)
{
    /*QString sb;
    sb = QString("(Size %1)").arg(currentList.size());
    foreach (int i, currentList)
        sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex Current: %s\n", sb.toUtf8().constData());
    sb = QString("(Size %1)").arg(newOrder.size());
    foreach (int i, newOrder)
        sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex Before : %s\n", sb.toUtf8().constData());*/

    QList<int> newList;

    foreach (int i, newOrder) {
        int found = currentList.indexOf(i);
        if (found < 0) {
            // It's not in the list, so something is odd (e.g. client error). Ignore it.
            continue;
        }

        // Iterate through the list from this point onward. If there are hidden devices
        // immediately following, take them too.
        newList.append(currentList.takeAt(found));
        while (found < currentList.size()) {
            bool hidden;
            if (output)
                hidden = GlobalConfig().isHiddenAudioOutputDevice(currentList.at(found));
            else
                hidden = GlobalConfig().isHiddenAudioCaptureDevice(currentList.at(found));

            if (!hidden)
                break;
            newList.append(currentList.takeAt(found));
        }
    }

    // If there are any devices left in.. just tack them on the end.
    if (currentList.size() > 0)
        newList += currentList;

    /*sb = QString("(Size %1)").arg(newList.size());
    foreach (int i, newList)
        sb += QString("%1, ").arg(i);
    fprintf(stderr, "=== Reindex After  : %s\n", sb.toUtf8().constData());*/
    return newList;
}


void BackendCapabilities::setAudioOutputDevicePriorityListForCategory(Phonon::Category category, QList<int> devices)
{
    // We have to reindex the full list of devices in such a way that the minimal changes are made
    // but the visible outcome (taking into consideration the filtered devices) is as per dictated here.
    QList<int> currentList = GlobalConfig().audioOutputDeviceListFor(category, Phonon::GlobalConfig::ShowUnavailableDevices|Phonon::GlobalConfig::ShowAdvancedDevices);

    // newlist is now ready to be stored
    GlobalConfig().setAudioOutputDeviceListFor(category, reindexList(currentList, devices, true));
}

void BackendCapabilities::setAudioCaptureDevicePriorityListForCategory(Phonon::Category category, QList<int> devices)
{
    // We have to reindex the full list of devices in such a way that the minimal changes are made
    // but the visible outcome (taking into consideration the filtered devices) is as per dictated here.
    QList<int> currentList = GlobalConfig().audioCaptureDeviceListFor(category, Phonon::GlobalConfig::ShowUnavailableDevices|Phonon::GlobalConfig::ShowAdvancedDevices);

    // newlist is now ready to be stored
    GlobalConfig().setAudioCaptureDeviceListFor(category, reindexList(currentList, devices, false));
}

} // namespace Experimental
} // namespace Phonon

/**************************************************************************
 *
 * Copyright 2013 Canonical Ltd.
 * Copyright 2013 Carlos J Mazieri <carlos.mazieri@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File: externalfswatcher.h
 * Date: 9/14/2013
 */

#ifndef EXTERNALFSWATCHER_H
#define EXTERNALFSWATCHER_H

#include <QFileSystemWatcher>

#define DEFAULT_NOTICATION_PERIOD  500


/*!
 * \brief The ExternalFSWatcher class notifies the owner when the File System when the current path \a m_setPath has changed
 *            emitting pathModified() signal.
 *
 *  The current path \a m_setPath is set by using the slot \ref  setCurrentPath()
 *
 *  The idea of this class is to minimize notifications as the current path can change quickly.
 *  A notification will occur if it was requested for a path and this path is still the current at the moment
 *  of the notification.
 *
 *  Once it detects a change it will wait \ref getIntervalToNotifyChanges() milliseconds to notify that change.
 *  At this moment it checks if no \ref setCurrentPath() has been called during this time and then notifies that change.
 */
class ExternalFSWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit ExternalFSWatcher(QObject *parent = 0);
    int      getIntervalToNotifyChanges() const;

signals:
     void      pathModified();

     public slots:
     void      setCurrentPath(const QString& curPath);
     void      setIntervalToNotifyChanges(int ms);     

private slots:
     void      slotDirChanged(const QString&);
     void      slotFireChanges();

 private:
     QString   m_setPath;
     QString   m_changedPath;
     bool      m_waitingEmit;
     int       m_msWaitTime;
};

#endif // EXTERNALFSWATCHER_H

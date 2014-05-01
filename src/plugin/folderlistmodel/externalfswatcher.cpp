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
 * File: externalfswatcher.cpp
 * Date: 9/14/2013
 */

#include "externalfswatcher.h"

#include <QTimer>
#include <QDateTime>
#include <QDebug>

#if DEBUG_EXT_FS_WATCHER
# define DEBUG_FSWATCHER()    \
    qDebug() << "[extFsWatcher]" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") \
             << Q_FUNC_INFO << "m_setPath:" << m_setPath \
             << "m_changedPath:" << m_changedPath        \
             << "m_waitingEmit:" << m_waitingEmitCounter
#else
# define DEBUG_FSWATCHER()  /**/
#endif //


ExternalFSWatcher::ExternalFSWatcher(QObject *parent) :
    QFileSystemWatcher(parent)
  , m_waitingEmitCounter(0)
  , m_msWaitTime(DEFAULT_NOTICATION_PERIOD)
  , m_lastChangedIndex(0)
{
    connect(this,   SIGNAL(directoryChanged(QString)),
            this,   SLOT(slotDirChanged(QString)));
}


void ExternalFSWatcher::setCurrentPath(const QString &curPath)
{
    if (!curPath.isEmpty())
    {        
        if (m_setPath.count() == 1 && m_setPath.at(0) != curPath)
        {
            m_setPath.removeFirst();
        }
        if (m_setPath.count() == 0)
        {
            m_setPath.append(curPath);
            QFileSystemWatcher::addPath(curPath);
        }
    }
    DEBUG_FSWATCHER();
}


void ExternalFSWatcher::setCurrentPaths(const QStringList &paths)
{
    QStringList myPaths(paths);
    ::qSort(myPaths);
    QStringList existentPaths = QFileSystemWatcher::directories();
    if (existentPaths.count() > 0)
    {
        QFileSystemWatcher::removePaths(existentPaths);
    }
    m_setPath = myPaths;
    QFileSystemWatcher::addPaths(paths);
}


void ExternalFSWatcher::slotDirChanged(const QString &dir)
{
    DEBUG_FSWATCHER();
    if (    ((m_lastChangedIndex = m_setPath.indexOf(dir)) != -1)
         && ( m_waitingEmitCounter == 0 || dir != m_changedPath )
       )
    {
        removePath(m_setPath.at(m_lastChangedIndex));
        ++m_waitingEmitCounter;
        m_changedPath = dir;
        QTimer::singleShot(m_msWaitTime, this, SLOT(slotFireChanges()));       
    }
}


/*!
 * \brief ExternalFSWatcher::slotFireChanges() emits \ref pathModified() only when it is sure
 *  that the  current path was changed.
 *
 *  A change for the current path (the last current) MUST be notified at least once.
 */
void ExternalFSWatcher::slotFireChanges()
{
   if (   --m_waitingEmitCounter == 0
       && m_lastChangedIndex != -1
       && m_lastChangedIndex < m_setPath.count() )
   {
       addPath(m_setPath.at(m_lastChangedIndex));
       if (m_setPath.at(m_lastChangedIndex) == m_changedPath)
       {
           emit pathModified(m_changedPath);
#if DEBUG_EXT_FS_WATCHER
       DEBUG_FSWATCHER() << "emit pathModified()";
#endif
       }
   }  
}



void ExternalFSWatcher::setIntervalToNotifyChanges(int ms)
{
    m_msWaitTime = ms;
}


int ExternalFSWatcher::getIntervalToNotifyChanges() const
{
    return m_msWaitTime;
}

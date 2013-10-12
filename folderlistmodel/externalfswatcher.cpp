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
             << "m_waitingEmit:" << m_waitingEmit
#else
# define DEBUG_FSWATCHER()  /**/
#endif //


ExternalFSWatcher::ExternalFSWatcher(QObject *parent) :
    QFileSystemWatcher(parent)
  , m_waitingEmit(false)
  , m_msWaitTime(DEFAULT_NOTICATION_PERIOD)
{
    connect(this,   SIGNAL(directoryChanged(QString)),
            this,   SLOT(slotDirChanged(QString)));
}


void ExternalFSWatcher::setCurrentPath(const QString &curPath)
{
    if (!curPath.isEmpty())
    {
        if (m_setPath != curPath)
        {
            if (!m_setPath.isEmpty())
            {
                removePath(m_setPath);
            }
            m_setPath = curPath;
            addPath(m_setPath);
        }        
    }
    DEBUG_FSWATCHER();
}


void ExternalFSWatcher::slotDirChanged(const QString &dir)
{
    DEBUG_FSWATCHER();
    m_changedPath = dir;
    if (!m_waitingEmit && m_setPath == m_changedPath)
    {
        m_waitingEmit = true;
        QTimer::singleShot(m_msWaitTime, this, SLOT(slotFireChanges()));       
    }
}


/*!
 * \brief ExternalFSWatcher::slotFireChanges() emits \ref pathModified() only when is sure
 */
void ExternalFSWatcher::slotFireChanges()
{
   if (m_setPath == m_changedPath)
   {     
       emit pathModified();
#if DEBUG_EXT_FS_WATCHER
       DEBUG_FSWATCHER() << "emit pathModified()";
#endif
   }
   m_waitingEmit = false;
}



void ExternalFSWatcher::setIntervalToNotifyChanges(int ms)
{
    m_msWaitTime = ms;
}


int ExternalFSWatcher::getIntervalToNotifyChanges() const
{
    return m_msWaitTime;
}

/**************************************************************************
 *
 * Copyright 2014 Canonical Ltd.
 * Copyright 2014 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: disklocation.cpp
 * Date: 08/03/2014
 */

#include "disklocation.h"
#include "disklocationitemdiriterator.h"
#include "iorequest.h"
#include "ioworkerthread.h"
#include "externalfswatcher.h"
#include "locationurl.h"
#include "disklocationitemfile.h"
#include "disklocationitemdir.h"


#if defined(Q_OS_UNIX)
#include <sys/statvfs.h>
#endif

#include <QDebug>

#if defined(DEBUG_EXT_FS_WATCHER)
# define DEBUG_WATCHER()  qDebug() << "[extFsWatcher]" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") \
                                   << Q_FUNC_INFO << this
#else
# define DEBUG_WATCHER() /**/
#endif

DiskLocation::DiskLocation(int type, QObject *parent)
    : Location(type, parent)
    , m_extWatcher(0)
{
}

DiskLocation::~ DiskLocation()
{
    stopExternalFsWatcher();
}

/*!
 * \brief DiskLocation::stopExternalFsWatcher() stops the External File System Watcher
 */
void DiskLocation::stopExternalFsWatcher()
{
    if (m_extWatcher) {
        DEBUG_WATCHER();
        delete m_extWatcher;
        m_extWatcher = 0;
    }
}

/*!
 * \brief DiskLocation::startExternalFsWatcher() starts the External File System Watcher
 */
void DiskLocation::startExternalFsWatcher()
{
    if (m_extWatcher == 0) {
        DEBUG_WATCHER();
        m_extWatcher = new ExternalFSWatcher(this);
        m_extWatcher->setIntervalToNotifyChanges(EX_FS_WATCHER_TIMER_INTERVAL);

        connect(m_extWatcher, SIGNAL(pathModified(QString)),
                this,         SIGNAL(extWatcherPathChanged(QString)));
    }

    if (m_extWatcher && m_info) {
        //setCurrentPath() checks for empty paths
        m_extWatcher->setCurrentPath(m_info->absoluteFilePath());
    }
}

void DiskLocation::onItemsFetched()
{
    if (m_extWatcher) {
        m_extWatcher->setCurrentPath(m_info->absoluteFilePath());
    }

    emit itemsFetched();
}

void DiskLocation::startWorking()
{
    if (m_usingExternalWatcher) {
        startExternalFsWatcher();
    }
}

void DiskLocation::stopWorking()
{
    stopExternalFsWatcher();
}

void DiskLocation::fetchExternalChanges(const QString &path, const DirItemInfoList &list, QDir::Filters dirFilter)
{
    auto extFsWorker = new ExternalFileSystemChangesWorker(list, path, dirFilter, false);

    addExternalFsWorkerRequest(extFsWorker);
}

void DiskLocation::addExternalFsWorkerRequest(ExternalFileSystemChangesWorker *extFsWorker)
{
    connect(extFsWorker,    SIGNAL(added(DirItemInfo)),
            this,           SIGNAL(extWatcherItemAdded(DirItemInfo)));

    connect(extFsWorker,    SIGNAL(removed(DirItemInfo)),
            this,           SIGNAL(extWatcherItemRemoved(DirItemInfo)));

    connect(extFsWorker,    SIGNAL(changed(DirItemInfo)),
            this,           SIGNAL(extWatcherItemChanged(DirItemInfo)));

    connect(extFsWorker,    SIGNAL(finished(int)),
            this,           SIGNAL(extWatcherChangesFetched(int)));

    workerThread()->addRequest(extFsWorker);
}

ExternalFSWatcher *DiskLocation::getExternalFSWatcher() const
{
    return m_extWatcher;
}

void DiskLocation::setUsingExternalWatcher(bool use)
{
    if ((m_usingExternalWatcher = use)) {
        startExternalFsWatcher();
    } else {
        stopExternalFsWatcher();
    }
}

DirItemInfo *DiskLocation::newItemInfo(const QString &urlPath)
{
    return new DirItemInfo(urlPath);
}

DirListWorker *DiskLocation::newListWorker(const QString &urlPath, QDir::Filters filter,
                                           const bool isRecursive)
{
    return new DirListWorker(urlPath, filter, isRecursive);
}

QString DiskLocation::urlBelongsToLocation(const QString &urlPath, int indexOfColonAndSlash)
{
    QString ret;
    if (urlPath.startsWith(LocationUrl::DiskRootURL.midRef(0, 5))) {
        ret  = QDir::rootPath() + DirItemInfo::removeExtraSlashes(urlPath, indexOfColonAndSlash + 1);
    }
    return ret;
}

LocationItemDirIterator *DiskLocation::newDirIterator(const QString &path, QDir::Filters filters,
                                                      QDirIterator::IteratorFlags flags,
                                                      LocationItemDirIterator::LoadMode loadmode)
{
    Q_UNUSED(loadmode);
    return new DiskLocationItemDirIterator(path, filters, flags);
}


LocationItemFile *DiskLocation::newFile(const QString &path)
{
    return new DiskLocationItemFile(path, this);
}


LocationItemDir *DiskLocation::newDir(const QString &dir)
{
    return new DiskLocationItemDir(dir);
}

bool DiskLocation::isThereDiskSpace(const QString &pathname, qint64 requiredSize)
{
    bool ret = true;

#if defined(Q_OS_UNIX)
    QFileInfo info(pathname);
    while (!info.exists() && info.absoluteFilePath() != QDir::rootPath()) {
        info.setFile(info.absolutePath());
    }

    struct statvfs  vfs;
    if ( ::statvfs( QFile::encodeName(info.absoluteFilePath()).constData(), &vfs) == 0 ) {
        qint64 free =  vfs.f_bsize * vfs.f_bfree;
        ret = free > requiredSize;
    }
#endif

    return ret;
}

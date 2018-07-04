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
 * File: disklocation.h
 * Date: 08/03/2014
 */

#ifndef DISKLOCATION_H
#define DISKLOCATION_H

#include "location.h"
#include <QList>

/*!
 *  When the External File System Wathcer is enabled,
 *  this is the interval used to check if there has been any change in the current path
 *
 *  \sa setEnabledExternalFSWatcher()
 */
#define EX_FS_WATCHER_TIMER_INTERVAL   900


class ExternalFSWatcher;
class ExternalFileSystemChangesWorker;

/*!
 * \brief The DiskLocation class extends \ref Location for Local Disk and provides a External File System watcher
 */
class DiskLocation : public Location
{
    Q_OBJECT
public:
    explicit DiskLocation(int type, QObject *parent = 0);
    virtual ~DiskLocation();

    ExternalFSWatcher *getExternalFSWatcher() const;

    virtual void fetchExternalChanges(const QString &urlPath, const DirItemInfoList &list, QDir::Filters dirFilter) ;

    virtual void startExternalFsWatcher();
    virtual void stopExternalFsWatcher();

    virtual void startWorking();
    virtual void stopWorking();

    virtual DirItemInfo *newItemInfo(const QString &urlPath);
    virtual DirListWorker *newListWorker(const QString &urlPath, QDir::Filters filter, const bool isRecursive);

    virtual LocationItemDirIterator *newDirIterator(const QString &path, QDir::Filters filters,
                                                    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                                                    LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

    virtual LocationItemFile *newFile(const QString &path);
    virtual LocationItemDir *newDir(const QString &dir = QLatin1String(0));
    virtual bool isThereDiskSpace(const QString &pathname, qint64 requiredSize);
    virtual QString urlBelongsToLocation(const QString &urlPath, int indexOfColonAndSlash);

protected:
    void addExternalFsWorkerRequest(ExternalFileSystemChangesWorker *);

public slots:
    virtual void setUsingExternalWatcher(bool use);

protected slots:
    void onItemsFetched();

protected:
    ExternalFSWatcher *m_extWatcher ;

};

#endif // DISKLOCATION_H

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
 * File: trashlocation.h
 * Date: 08/03/2014
 */

#ifndef TRASHLOCATION_H
#define TRASHLOCATION_H

#include "disk/disklocation.h"
#include "trash/qtrashdir.h"

class TrashListWorker;

class TrashLocation : public DiskLocation, public QTrashDir
{
    Q_OBJECT
public:
    explicit TrashLocation(int type, QObject *parent=0);
    ~TrashLocation();
    virtual bool        becomeParent();
    virtual void        refreshInfo();
    virtual void        fetchItems(QDir::Filter dirFilter, bool recursive=0);
    virtual void        fetchExternalChanges(const QString& urlPath,
                                             const DirItemInfoList& list,
                                             QDir::Filter dirFilter) ;

    virtual void        startWorking();
    virtual void        startExternalFsWatcher();

    virtual DirItemInfo *validateUrlPath(const QString& urlPath);

private:
    void               addTrashFetchRequest(TrashListWorker *workerObject);

public slots:

};

#endif // TRASHLOCATION_H

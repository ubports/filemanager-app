/**************************************************************************
 *
 * Copyright 2015 Canonical Ltd.
 * Copyright 2015 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: networklocation.cpp
 * Date: 08/12/2015
 */

#include "networklocation.h"
#include "networklistworker.h"
#include "locationitemdiriterator.h"
#include "diriteminfo.h"

NetworkLocation::NetworkLocation(int type, QObject *parent): Location(type, parent)
{
}



DirListWorker *NetworkLocation::newListWorker(const QString &urlPath, QDir::Filters filter,
                                              const bool isRecursive)
{
    QDirIterator::IteratorFlags flags = isRecursive ? QDirIterator::Subdirectories :
                                        QDirIterator::NoIteratorFlags;

    LocationItemDirIterator *dirIterator  = newDirIterator(urlPath, filter, flags,
                                                           LocationItemDirIterator::LoadLater);
    DirItemInfo             *baseitemInfo = newItemInfo(QLatin1String(0));
    // the NetworkListWorker object takes ownership of baseitemInfo and also creates its own copy of m_info
    return new NetworkListWorker(dirIterator, baseitemInfo, m_info);
}

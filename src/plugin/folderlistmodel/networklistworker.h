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
 * File: networklistworker.h
 * Date: 06/12/2015
 */

#ifndef NETWORKLISTWORKER_H
#define NETWORKLISTWORKER_H

#include "iorequestworker.h"

class LocationItemDirIterator;
class DirItemInfo;

/*!
 * \brief The NetworkListWorker class intends to be a generic Network directory loader.
 *.
 *  It trusts on \ref LocationItemDirIterator and \ref DirItemInfo classes
 *
 *  The LocationItemDirIterator::load() must bring the list of items.
 */
class NetworkListWorker : public DirListWorker
{
    Q_OBJECT
public:
    NetworkListWorker(LocationItemDirIterator *dirIterator,
                      DirItemInfo              *mainItemInfo,
                      const DirItemInfo        *parentItemInfo = 0);
    ~NetworkListWorker();
protected:
    virtual DirItemInfoList getNetworkContent();
    void                    setSmbItemAttributes();
protected:
    LocationItemDirIterator      *m_dirIterator;
    DirItemInfo                  *m_mainItemInfo;   //takes ownership from mainItemInfo
    DirItemInfo
    *m_parentItemInfo; //create its own instance by doing a copy from parentItemInfo
};

#endif // NETWORKLISTWORKER_H

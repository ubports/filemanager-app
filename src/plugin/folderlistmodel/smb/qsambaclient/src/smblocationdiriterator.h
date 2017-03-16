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
 * File: smblocationdiriterator.h
 * Date: 10/01/2015
 */

#ifndef SMBLOCATIONDIRITERATOR_H
#define SMBLOCATIONDIRITERATOR_H

#include "locationitemdiriterator.h"
#include "smbobject.h"

class SmbLocationDirIterator: public LocationItemDirIterator, public SmbObject
{
public:
      ~SmbLocationDirIterator();
public:
   virtual DirItemInfo	fileInfo() const;
   virtual QString	    fileName() const;
   virtual QString          filePath() const;
   virtual bool         hasNext()  const;
   virtual QString          next()          ;
   virtual void         load();
public:
   SmbLocationDirIterator(const QString & path,
                          QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                          Const_SmbUtil_Ptr  smb  = 0,
                          LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

   SmbLocationDirIterator(const QString & path, QDir::Filters filters,
                          QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                          Const_SmbUtil_Ptr  smb  = 0,
                          LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

   SmbLocationDirIterator(const QString & path, const QStringList & nameFilters,
                          QDir::Filters filters = QDir::NoFilter,
                          QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                          Const_SmbUtil_Ptr  smb  = 0,
                          LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);
private:
   QStringList  m_urlItems;
   int          m_curItem;
};

#endif // SMBLOCATIONDIRITERATOR_H

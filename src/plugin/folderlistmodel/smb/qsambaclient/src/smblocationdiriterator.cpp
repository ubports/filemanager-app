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
 * File: smblocationdiriterator.cpp
 * Date: 10/01/2015
 */

#include "smblocationdiriterator.h"
#include "smbiteminfo.h"
#include "smbutil.h"

#define INIT_ATTR(path,smb) SmbObject(path,smb), m_curItem(-1)

#define GOOD_INDEX() (m_curItem >= 0 && m_curItem < m_urlItems.count())


SmbLocationDirIterator::SmbLocationDirIterator(const QString &path,
                                               const QStringList &nameFilters,
                                               QDir::Filters filters,
                                               QDirIterator::IteratorFlags flags,
                                               Const_SmbUtil_Ptr smb)
        : LocationItemDirIterator(path, nameFilters, filters, flags)
        , INIT_ATTR(path, smb)
{
    bool recursive = flags == QDirIterator::Subdirectories ? true : false;
    m_urlItems = smbObj()->listContent(path, recursive, filters, nameFilters);
}


SmbLocationDirIterator::SmbLocationDirIterator(const QString &path,
                                               QDir::Filters filters,
                                               QDirIterator::IteratorFlags flags,
                                               Const_SmbUtil_Ptr smb)
     : LocationItemDirIterator(path,filters,flags)
     , INIT_ATTR(path, smb)
{
    bool recursive = flags == QDirIterator::Subdirectories ? true : false;
    m_urlItems = smbObj()->listContent(path, recursive, filters);
}


SmbLocationDirIterator::SmbLocationDirIterator(const QString &path,
                                               QDirIterator::IteratorFlags flags,
                                               Const_SmbUtil_Ptr smb)
       : LocationItemDirIterator(path,flags)
       , INIT_ATTR(path, smb)
{
    bool recursive = flags == QDirIterator::Subdirectories ? true : false;
    m_urlItems = smbObj()->listContent(path, recursive);
}


SmbLocationDirIterator::~SmbLocationDirIterator()
{

}


QString SmbLocationDirIterator::next()
{
    QString ret;
    if (hasNext())
    {
        ret = m_urlItems.at(++m_curItem);
    }
    return ret;
}


bool SmbLocationDirIterator::hasNext() const
{
    return m_urlItems.count() > 0 && m_curItem < (m_urlItems.count() -1);
}


DirItemInfo SmbLocationDirIterator::fileInfo() const
{
    if (GOOD_INDEX())
    {
        SmbItemInfo  info(m_urlItems.at(m_curItem), m_smb);
        return info;
    }
    return SmbItemInfo();
}


QString SmbLocationDirIterator::fileName() const
{
    QString file;
    if (GOOD_INDEX())
    {
        QStringList paths = UrlItemInfo::separatePathFilename(m_urlItems.at(m_curItem));
        if (paths.count() == 2)
        {
            file = paths.at(1);
        }
    }
    return file;
}


QString SmbLocationDirIterator::path() const
{
    QString ret;
    QStringList separated =  UrlItemInfo::separatePathFilename(cleanUrl());
    if (separated.count() > 0)
    {
        ret = separated.at(0);
    }
    return ret;
}


QString SmbLocationDirIterator::filePath() const
{
    QString fullpathname;
    if (GOOD_INDEX())
    {
        fullpathname = m_urlItems.at(m_curItem);
    }
    return fullpathname;
}

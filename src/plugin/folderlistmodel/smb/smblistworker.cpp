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
 * File: smblistworker.cpp
 * Date: 17/01/2015
 */

#include "smblistworker.h"
#include "smblocationdiriterator.h"

SmbListWorker::SmbListWorker(const QString &pathName,
                             QDir::Filter filter,
                             const bool isRecursive,
                             bool parentIsHost,
                             Const_SmbUtil_Ptr smb)
   : DirListWorker(pathName, filter, isRecursive)  
   , SmbObject(pathName, smb)
   , m_parentIsHost(parentIsHost)
{
    mLoaderType =  NetworkLoader;
}

/*!
 * \brief SmbListWorker::getNetworkContent() it fills a \ref DirItemInfoList with samba (share/directory) content
 * \return the list of \ref SmbItemInfo items
 */
DirItemInfoList SmbListWorker::getNetworkContent()
{
    DirItemInfoList smbContent;

    QDirIterator::IteratorFlags flags = mIsRecursive ?
                                        QDirIterator::Subdirectories :
                                        QDirIterator::NoIteratorFlags;

    SmbLocationDirIterator dir(mPathName, mFilter, flags, m_smb);
    while (dir.hasNext())
    {
        QString next = dir.next();
        SmbItemInfo item (next, m_smb);
        if (m_parentIsHost)
        {
            item.setAsShare();
        }
        smbContent.append(item);
    }

    return smbContent;
}


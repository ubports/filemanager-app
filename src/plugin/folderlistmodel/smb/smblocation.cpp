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
 * File: smblocation.cpp
 * Date: 17/01/2015
 */

#include "smblocation.h"
#include "smbutil.h"
#include "smbiteminfo.h"
#include "smblocationdiriterator.h"
#include "iorequest.h"
#include "ioworkerthread.h"
#include "locationurl.h"
#include "smblocationitemfile.h"
#include "smblocationitemdir.h"
#include "netauthenticationdata.h"

#if defined(Q_OS_UNIX)
#include <sys/statvfs.h>
#endif

SmbLocation::SmbLocation(int type, QObject *parent)
    : NetworkLocation(type, parent)
    , SmbLocationAuthentication()
{
    m_smb = new SmbUtil(suitableAuthenticationFunction());
    setAuthentication(NetAuthenticationData::currentUser(), NetAuthenticationData::noPassword());
}

SmbLocation::~SmbLocation()
{

}

/*!
 * \brief SmbLocation::setAuthentication() saves user/password ot be used in current SmbLocationAuthentication function
 *
 *  These information will be used in further Samba authentication for this instance
 *
 * \param user
 * \param password
 */
void SmbLocation::setAuthentication(const QString &user, const QString &password)
{
    //setInfo is not static
    SmbLocationAuthentication::setInfo(user, password);
}

QString SmbLocation::currentAuthenticationUser()
{
    //currenAuthUser is not static
    return SmbLocationAuthentication::currentAuthUser();
}

QString SmbLocation::currentAuthenticationPassword()
{
    return SmbLocationAuthentication::currentAuthPassword();
}

DirItemInfo *SmbLocation::newItemInfo(const QString &urlPath)
{
    return new SmbItemInfo(urlPath, m_smb);
}

QString SmbLocation::urlBelongsToLocation(const QString &urlPath, int indexOfColonAndSlash)
{
    QString ret;
    if (urlPath.startsWith(LocationUrl::SmbURL.midRef(0, 4)) ||
            urlPath.startsWith(LocationUrl::CifsURL.midRef(0, 5))) {

        ret  = LocationUrl::SmbURL + DirItemInfo::removeExtraSlashes(urlPath, indexOfColonAndSlash + 1);
    }

    return ret;
}


LocationItemDirIterator *SmbLocation::newDirIterator(const QString &path, QDir::Filters filters,
                                                     QDirIterator::IteratorFlags flags, LocationItemDirIterator::LoadMode loadmode)
{
    return new SmbLocationDirIterator(path, filters, flags, m_smb, loadmode);
}


LocationItemFile *SmbLocation::newFile(const QString &path)
{
    return new SmbLocationItemFile(path, this, m_smb);
}


LocationItemDir *SmbLocation::newDir(const QString &dir)
{
    return new SmbLocationItemDir(dir, m_smb);
}

bool SmbLocation::isThereDiskSpace(const QString &pathname, qint64 requiredSize)
{
    bool ret = false;

#if defined(Q_OS_UNIX)
    struct statvfs st;

    if (m_smb->getStatvfsInfo(pathname, &st) == SmbUtil::StatDone) {
        qint64 free =  st.f_bsize * st.f_bfree;
        ret = free > requiredSize;
    }

#else
    ret =  true;
#endif

    return ret;
}

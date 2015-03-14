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
#include "smblistworker.h"
#include "iorequest.h"
#include "ioworkerthread.h"

SmbLocation::SmbLocation(int type, QObject *parent)
     : Location(type, parent)
     , SmbLocationAuthentication()
{
     m_smb = new SmbUtil(suitableAuthenticationFunction());
     setAuthentication(::qgetenv("USER"), QString());
}


SmbLocation::~SmbLocation()
{

}


//======================================================================================================
/*!
 * \brief SmbLocation::setAuthentication() saves user/password ot be used in current SmbLocationAuthentication function
 *
 *  These information will be used in further Samba authentication for this instance
 *
 * \param user
 * \param password
 */
void SmbLocation::setAuthentication(const QString &user,
                                    const QString &password)
{
    //setInfo is not static
    SmbLocationAuthentication::setInfo(user,password);
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


DirItemInfo * SmbLocation::newItemInfo(const QString &urlPath)
{
    return new SmbItemInfo(urlPath, m_smb);
}


DirListWorker * SmbLocation::newListWorker(const QString &urlPath, QDir::Filter filter, const bool isRecursive)
{
    return new SmbListWorker(urlPath,filter,isRecursive, m_info ? m_info->isHost() : false, m_smb);
}


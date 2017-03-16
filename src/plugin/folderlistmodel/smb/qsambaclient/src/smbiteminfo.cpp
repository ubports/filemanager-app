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
 * File: smbiteminfo.cpp
 * Date: 08/12/2014
 */

#include "smbiteminfo.h"
#include "locationurl.h"
#include "smbutil.h"

#include <QUrl>

SmbItemInfo::SmbItemInfo() :  UrlItemInfo(), SmbObject(QLatin1String(0))
{

}

SmbItemInfo::SmbItemInfo(const QString &urlPath, Const_SmbUtil_Ptr smb) :
   UrlItemInfo(urlPath, LocationUrl::SmbURL)
  ,SmbObject(urlPath, smb)
{  
    if (isValid() && !isRoot())
    {
        setInfo(cleanUrl());
    }
}


SmbItemInfo::~SmbItemInfo()
{

}


void SmbItemInfo::setInfo(const QString& smb_path)
{   
    SmbUtil *smb = const_cast<SmbUtil*> (m_smb);
    //getStatInfo() is supposed to clear the struct stat
    struct stat st;
    int ret  = smb->getStatInfo(smb_path, &st);
    //lets start with true
    d_ptr->_exists  = d_ptr->_isReadable = true;
    switch(ret)
    {
    case SmbUtil::StatInvalid:
    case SmbUtil::StatDoesNotExist:
         //reset _isHost because it might be set in UrlItemInfo
         d_ptr->_isHost = false;
         d_ptr->_exists  = d_ptr->_isReadable = false;
         break;
    case SmbUtil::StatDir:                     
        break;
    case SmbUtil::StatHost:
        d_ptr->_isHost = true;
        break;
    case SmbUtil::StatShare:
        d_ptr->_isNetworkShare = true;
        break;
    case SmbUtil::StatWorkgroup:
        d_ptr->_isHost = false;
        d_ptr->_isWorkGroup = true;
        break;
    case SmbUtil::StatNoAccess:
        //it is special case where the authentication might have failed
        d_ptr->_isReadable = false;
        d_ptr->_needsAuthentication = true;
        break;
    }
    //all the information should be in place now
    fillFromStatBuf(st);
    //set the field here to use on any DirItemInfo object
    d_ptr->_authenticationPath = sharePath();
}


/*!
 * \brief SmbItemInfo::sharePath() returns the share part of the item
 * \return NULL if the item is root
 */
QString SmbItemInfo::sharePath() const
{
    QString share;
    if (isWorkGroup() || isHost() || isShare())
    {
           share = absoluteFilePath();
    }
    else
    if (!isRoot())
    {
        //0 1 2 3 4 6
        //s m b : / /    smb://host/share[/dir]
        int slashIndex = 6;
        int found = 0;
        QString fullpath(absoluteFilePath());
        for (; found < 2 && slashIndex != -1; ++found)
        {
            slashIndex = fullpath.indexOf(QDir::separator(), slashIndex +1);
        }
        switch(found)
        {
          case 1: share = fullpath; break;
          case 2: share = fullpath.left(slashIndex); break;
          default: break;
        }
    }
    return share;
}


void SmbItemInfo::setFile(const QString &dir, const QString &file)
{
    QString smb_path;
    if (dir.startsWith(LocationUrl::SmbURL))
    {
        smb_path = dir;
    }
    else
    {
        QUrl url(urlPath());
        QFileInfo f(url.path() + QDir::separator() + dir);
        url.setPath(f.canonicalFilePath());
        smb_path = url.toString();
    }
    if (!file.isEmpty())
    {
        smb_path += QDir::separator() + file;
    }
    SmbItemInfo *other  = new SmbItemInfo( LocationUrl::SmbURL + DirItemInfo::removeExtraSlashes(smb_path),
                                           m_smb);
    if (other->isValid())
    {
        *this = *other;
    }

    delete other; //always delete
}

void SmbItemInfo::setFile(const QString &smb_path)
{
    return setFile(smb_path, QLatin1String(0));
}


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
 * File: smblocationitemdir.cpp
 * Date: 16/05/2015
 */

#include "smblocationitemdir.h"
#include "locationitemfile.h"
#include "smbiteminfo.h"
#include "smbutil.h"
#include "locationurl.h"

#include <QUrl>

SmbLocationItemDir::SmbLocationItemDir(const QString &dir, Const_SmbUtil_Ptr smb)
     : LocationItemDir(dir), SmbObject(dir, smb)
{

}


SmbLocationItemDir::~SmbLocationItemDir()
{

}

bool SmbLocationItemDir::exists() const
{
    bool ret = false;
    if (!cleanUrl().isEmpty())
    {
        SmbItemInfo item(cleanUrl(), m_smb);
        ret = item.exists();
    }
    return ret;
}


bool SmbLocationItemDir::mkdir(const QString& dir) const
{
    return this->mkpath(dir);
}


bool SmbLocationItemDir::mkpath(const QString& dir) const
{
#define  MKDIR_NOT_CALLED_YET   0x300  // any value greater than zero as mkdir returns values <= zero
     bool ret = false;
     QString absPath = makeAbsoluteUrl(dir);
     QUrl url(absPath);
     if (url.isValid() && absPath.startsWith(LocationUrl::SmbURL))
     {
         QLatin1Char slash('/');
         QStringList paths = url.path().split(slash, QString::SkipEmptyParts);         
         QString partPath = LocationUrl::SmbURL + url.host();
         ret  = true;
         //first mkdir call updates this  mkdir_return
         int  mkdir_return = MKDIR_NOT_CALLED_YET;
         Smb::Context context = smbObj()->createContext();
         Q_ASSERT(context);
         //loop starts with share, appends each path and if it does exist tries to create it
         //for shares (counter == 0) it must exist, so it it is not created
         for(int counter = 0; ret && counter < paths.count(); ++counter)
         {            
             partPath += slash + paths.at(counter);
             //if mkdir was called it means any path in the loop will not exist, so it is NOT necessary to call openDir()
             Smb::FileHandler fd = mkdir_return == MKDIR_NOT_CALLED_YET ?
                                   smbObj()->openDir(context,partPath) : 0;
             //OK if it already exists or if it was created
             //shares (the first path, when counter == 0) must already exist
             ret = fd != 0 || (counter > 0 &&
                               (mkdir_return = ::smbc_getFunctionMkdir(context)(context, partPath.toLocal8Bit().constData(), LocationItemFile::getUmaskDirsCreation())) == 0
                              );
             if (fd != 0)
             {
                 smbObj()->closeHandle(context,fd);
             }            
         }
         smbObj()->deleteContext(context);
     }
     return ret;
}


bool SmbLocationItemDir::rmdir(const QString& dir) const
{
    bool ret = false;
    QString fullpath = makeAbsoluteUrl(dir);
    if (fullpath.startsWith(LocationUrl::SmbURL))
    {
        Smb::Context context = smbObj()->createContext();
        Q_ASSERT(context);
        if (::smbc_getFunctionRmdir(context)(context,fullpath.toLocal8Bit().constData()) == 0)
        {
            ret = true;
        }
        smbObj()->deleteContext(context);
    }
    return ret;
}


QString SmbLocationItemDir::makeAbsoluteUrl(const QString &dir) const
{
    //dir should be a full URL like smb://host/share
    QString ret(dir);
    //verify if dir is relative
    if (!dir.startsWith(LocationUrl::SmbURL) && cleanUrl().startsWith(LocationUrl::SmbURL))
    {
        ret = cleanUrl() + QDir::separator() + dir;
    }
    return ret;
}

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
 * File: smblocationitemfile.cpp
 * Date: 20/04/2015
 */

#include "smblocationitemfile.h"
#include "smbiteminfo.h"

#include <sys/stat.h>
#include <sys/statfs.h>
#include <errno.h>

#include <QDebug>


SmbLocationItemFile::SmbLocationItemFile(QObject *parent, Const_SmbUtil_Ptr  smb)
  : LocationItemFile(parent)
  , SmbObject(QLatin1String(0), smb)
  , m_fd(0)
  , m_context(0)
  , m_curReadPosition(0)
  , m_openMode(0)
{

}

SmbLocationItemFile::SmbLocationItemFile(const QString &name, QObject *parent,  Const_SmbUtil_Ptr  smb)
  : LocationItemFile(parent)
  , SmbObject(name, smb)
  , m_fd(0)
  , m_context(0)
  , m_curReadPosition(0)
  , m_openMode(0)
{

}


SmbLocationItemFile::~SmbLocationItemFile()
{
    close();
    if (m_context)
    {
        SmbObject::smbObj()->deleteContext(m_context);
        m_context = 0;
    }
}


QString SmbLocationItemFile::fileName() const
{
    return cleanUrl();
}


bool SmbLocationItemFile::rename(const QString& newName)
{
    bool ret = false;
    if (!cleanUrl().isEmpty())
    {
        close();
        ret = rename(cleanUrl(), newName);
    }
    return ret;
}


bool SmbLocationItemFile::rename(const QString& oldname, const QString& newName)
{  
    createContext();
    Smb::Context  nContext =  SmbObject::smbObj()->createContext();
    int ret = smbc_getFunctionRename(m_context)
                                    (m_context,
                                     oldname.toLocal8Bit().constData(),
                                     nContext,
                                     newName.toLocal8Bit().constData()
                                    );
    SmbObject::smbObj()->deleteContext(nContext);
    return ret == 0;
}


bool SmbLocationItemFile::remove()
{
    return private_remove(cleanUrl());
}


bool SmbLocationItemFile::remove(const QString& name)
{
   CleanUrl otherUrl(name);
   if (otherUrl.hasAuthenticationData())
   {
       qWarning() << Q_FUNC_INFO
                  << "Authentication in the form smb://user:password@pathname is not supported" ;
   }
   return private_remove(name) ;
}


bool SmbLocationItemFile::link(const QString& linkName)
{
    Q_UNUSED(linkName);
    qWarning() << Q_FUNC_INFO << "Smbclient does not provide link() function";
    return false;
}


bool SmbLocationItemFile::open(QIODevice::OpenMode mode)
{
    bool ret = false;   
    QString smb_path = cleanUrl();    
    if (!smb_path.isEmpty() && !isOpen())
    {       
        int openFlags   = 0;
        m_openMode      = mode;
        createContext();
        if (mode & QFile::ReadOnly)
        {
            openFlags = mode & QFile::WriteOnly ? O_RDWR : O_RDONLY;
        }
        else
        {
            if (mode & QFile::WriteOnly)
            {
                openFlags = O_CREAT | O_WRONLY;
            }
            if (mode & QFile::Append)
            {
                openFlags =  O_APPEND | O_CREAT | O_WRONLY;
            }
            if ((mode & QFile::Truncate) || !(mode & QFile::Append))
            {
               openFlags |=  O_TRUNC;
            }          
        }
        int creationMode = LocationItemFile::getUmaskFilesCreation();
        /*
         *  it looks like SMB open() does set the permission properly
         *  does not matter what value "creationMode" has, libsmbclient always creates files with the following permission:
         *     -rwxr--r-- 1 nobody    nogroup  0 Mai 30 14:04 second_item.txt
         *  SMB chmod() does not work either
         *
         *  It depends on Samba configuration: force user; force group; force create mode; force directory mode
         */
        m_fd = SmbObject::smbObj()->openFile(m_context, smb_path, openFlags, creationMode);
        ret = m_fd ? true : false;
    }   
    return ret;
}


qint64 SmbLocationItemFile::read(char * buffer, qint64 bytes)
{
    qint64 ret = -1;
    if (isOpen())
    {
        size_t to_write = static_cast<size_t> (bytes);
        void *buf   = static_cast<void*> (buffer);
        ssize_t wr = smbc_getFunctionRead(m_context)(m_context, m_fd, buf, to_write);
        ret = static_cast<qint64> (wr);
    }
    if (ret > 0)
    {
        m_curReadPosition += ret;
    }
    return ret;
}


qint64 SmbLocationItemFile::write(const char * buffer, qint64 bytes)
{
    qint64 ret = -1;
    if (isOpen())
    {
        size_t to_read = static_cast<size_t> (bytes);
        const void * const_buf = static_cast<const void*> (buffer);
        void *buf   = const_cast<void*> (const_buf);
        ssize_t rd = smbc_getFunctionWrite(m_context)(m_context, m_fd, buf, to_read);
        ret = static_cast<qint64> (rd);
    }
    return ret;
}


void SmbLocationItemFile::close()
{
    if (isOpen())
    {
       SmbObject::smbObj()->closeHandle(m_context, m_fd);
       m_fd = 0;
    }
    m_curReadPosition = 0;
}


bool SmbLocationItemFile::atEnd() const
{
    bool ret = true;  //closed files are at end, aren't they?
    if (isOpen())
    {
        struct stat  st;
        if (smbObj()->getFstat(m_context,m_fd, &st) == SmbUtil::StatDone)
        {
            ret = m_curReadPosition >= st.st_size;
        }
    }
    return ret;
}


qint64 SmbLocationItemFile::size() const
{
    qint64 size = 0;
    struct stat  st;
    bool ok = isOpen() ? smbObj()->getFstat(m_context,m_fd,&st) == 0 : false;
    if (!isOpen())
    {
        ok = m_context != 0 ? smbObj()->getStat(m_context,cleanUrl(),&st) == 0 :
                              smbObj()->getStatInfo(cleanUrl(),&st) == SmbUtil::StatDone;
    }
    if(ok)
    {
        size = static_cast<qint64> (st.st_size);
    }
    return size;
}


bool SmbLocationItemFile::isOpen() const
{
    return m_fd != 0 && m_context != 0 ? true : false;
}


bool SmbLocationItemFile::setPermissions(QFileDevice::Permissions perm)
{
    return setPermissions(cleanUrl(), perm);
}


bool SmbLocationItemFile::setPermissions(const QString &filename, QFileDevice::Permissions perm)
{
    bool ret = false;
    if (!filename.isEmpty())
    {
        createContextIfNotExists();
        ret = smbObj()->changePermissions(m_context, filename, LocationItemFile::unixPermissions(perm));
        /*
         *  fake the return in case the file exists becase chmod() on libsmbclient does not work,
         *  the same comment is present in the \ref open()
         */
        if (!ret)
        {
            struct stat  st;
            ret = smbObj()->getStat(m_context,filename, &st) == SmbUtil::StatDone;
        }
    }
    return ret;
}


QFile::Permissions SmbLocationItemFile::permissions() const
{
    SmbItemInfo info(cleanUrl(), m_smb);
    return info.permissions();
}


bool SmbLocationItemFile::private_remove(const QString& smb_path)
{
    bool ret = false;
    if (!smb_path.isEmpty())
    {
         close();
         createContextIfNotExists();
         if (smbc_getFunctionUnlink(m_context)(m_context, smb_path.toLocal8Bit().constData()) == 0)
         {
             ret = true;
         }
    }
    return ret;
}


/*!
 * \brief SmbLocationItemFile::createContext() Always creates a new context, if a context already exists it is deleted.
 */
void SmbLocationItemFile::createContext()
{
    if (m_context != 0)
    {
        SmbObject::smbObj()->deleteContext(m_context);
    }
    m_context = SmbObject::smbObj()->createContext();
    Q_ASSERT(m_context);
}


/*!
 * \brief SmbLocationItemFile::createContextIfNotExists() Creates a new context when the current context is null
 *
 *  It tries to reuse an existent context
 */
void SmbLocationItemFile::createContextIfNotExists()
{
    if (m_context == 0)
    {
        m_context = SmbObject::smbObj()->createContext();
        Q_ASSERT(m_context);
    }
}



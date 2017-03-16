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
 * File: smbusershare.cpp
 * Date: 02/12/2014
 */

#include "smbusershare.h"

#include <stdlib.h>

#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>

#define VAR_USER_SHARE_DIR   QLatin1String("/var/lib/samba/usershares")

QString SmbUserShare::m_error;


bool SmbUserShare::UserShareFile::exists() const
{
    return !name.isEmpty() && QFileInfo(path).exists();
}


SmbUserShare::SmbUserShare(QObject *parent) : QObject(parent)
{
}


SmbUserShare::~SmbUserShare()
{

}


bool SmbUserShare::canCreateShares()
{
    bool ret = false;
    m_error.clear();
    QString path_var =  ::qgetenv("PATH");
    if (!path_var.isEmpty())
    {
        QStringList paths = path_var.split(QLatin1Char(':'));
        for(int counter = 0; !ret && counter < paths.count(); ++counter)
        {
            QFileInfo net(paths.at(counter));
            ret = net.exists() && net.isExecutable();
        }
    } 
    if (!ret)
    {      
        m_error = tr("net tool not found, check samba installation");
    }
    else
    {
        QFileInfo varUserShareDir(VAR_USER_SHARE_DIR);
        ret &= varUserShareDir.isDir() && varUserShareDir.isWritable();
        if (!ret)
        {         
            m_error = tr("cannot write in ") + VAR_USER_SHARE_DIR;
        }
    }
    return ret;
}


QString SmbUserShare::proposedName (const QString &fulldirpath)
{
    QFileInfo path(fulldirpath);
    return path.fileName().replace(QLatin1Char(' '), QLatin1Char('_'));
}


bool SmbUserShare::createShareForFolder(const QString &fulldirpath,
                                        Access access,
                                        bool allowGuests,
                                        const QString &name)
{
    bool ret = false;
    QFileInfo dir(fulldirpath);
    if (dir.exists() && dir.isDir() )
    {
        QString cmd("net usershare add ");
        cmd += !name.isEmpty() ? name : proposedName(fulldirpath);
        cmd += QLatin1Char(' ') + fulldirpath
                +  QString(" \"create by %1 using SmbUserShare class\" ").
                    arg(QCoreApplication::applicationName());

        if (access == ReadWrite)
        {
            //cmd += QLatin1String(" everyone:f ");
            cmd += QLatin1String(" S-1-1-0:f ");
            QFile::setPermissions( fulldirpath, QFile::permissions(fulldirpath) | QFile::ReadGroup  |
                                    QFile::ReadOther | QFile::ExeOther | QFile::ExeGroup | QFile::WriteGroup | QFile::WriteOther
                                 );
        }
        else
        {
            //cmd += QLatin1String(" everyone:r ");
            cmd += QString(" S-1-1-0:r,S-1-22-1-%1:f ").arg(::getuid());
            QFile::setPermissions( fulldirpath, QFile::permissions(fulldirpath) | QFile::ReadGroup  |
                                    QFile::ReadOther | QFile::ExeOther | QFile::ExeGroup
                                  );
        }
        cmd += QLatin1String("guest_ok=");
        cmd +=  allowGuests ? QLatin1Char('y') : QLatin1Char('n');
        int retSystem  = ::system(cmd.toLocal8Bit().constData());
        ret = retSystem == 0;
    }
    return ret;
}



bool SmbUserShare::removeShare(const QString& name_OR_fulldirpath)
{
    bool ret = false;
    UserShareFile info = search(name_OR_fulldirpath);
    if (!info.name.isEmpty())
    {
        QString cmd("net usershare delete ");
        cmd += info.name;
        ret = ::system(cmd.toLocal8Bit().constData()) == 0;
    }
    return ret;
}


SmbUserShare::Access SmbUserShare::getEveryoneAccess(const QString& name_OR_fulldirpath)
{
   UserShareFile ret = search(name_OR_fulldirpath);
   return ret.getAccess();
}


bool SmbUserShare::isGuestAllowed(const QString& name_OR_fulldirpath)
{
    UserShareFile ret = search(name_OR_fulldirpath);
    return ret.isGuestAllowed();
}


SmbUserShare::UserShareFile SmbUserShare::readConfigFile(const QString &pathname)
{
   UserShareFile ret;
   QFile shareFile(pathname);
   if (shareFile.open(QFile::ReadOnly))
   {
      QString line(shareFile.readLine().trimmed());
      while (line.length() > 0)
      {
          if (!line.startsWith(QLatin1Char('#')))
          {
              QStringList pair = line.split(QLatin1Char('='));
              if (pair.count() == 2)
              {
                  QString name   = pair.at(0).trimmed();
                  QString value  = pair.at(1).trimmed();
                  if (name == QLatin1String("path"))
                  {
                      ret.path = value;
                  }
                  else
                      if (name== QLatin1String("usershare_acl"))
                      {
                          QStringList v = value.split(QChar(':'));
                          if (v.count() > 1)
                          {
                             ret.everyoneFlag = v.at(1).trimmed().at(0).toLower();
                          }
                      }
                      else
                          if (name == QLatin1String("guest_ok"))
                          {
                              ret.guest_ok = value.at(0).toLower();
                          }
                          else
                              if (name == QLatin1String("sharename"))
                              {
                                  ret.name = value;
                              }
              }
          }
          line = shareFile.readLine().trimmed();
      }//while
   }
   return ret;
}


SmbUserShare::UserShareFile SmbUserShare::search(const QString& name_OR_fulldirpath)
{
    UserShareFile ret;
    if (!name_OR_fulldirpath.isEmpty())
    {
        bool found = false ;
        bool isPath = QFileInfo(name_OR_fulldirpath).isAbsolute();
        UserShareFile current ;
        QDir d(VAR_USER_SHARE_DIR, QString(), QDir::NoSort, QDir::Files | QDir::NoSymLinks);
        for(uint counter=0; !found && counter < d.count(); ++ counter)
        {
            current = readConfigFile(d.absoluteFilePath(d[counter]));
            found   = isPath ? (current.path == name_OR_fulldirpath)
                             : (current.name == name_OR_fulldirpath);
        }
        if (found)
        {
            ret = current;
        }
    }
    return ret;
}



/**************************************************************************
 *
 * Copyright 2013 Canonical Ltd.
 * Copyright 2013 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: tempfiles.cpp
 * Date: 4/2/2013
 */

#include "tempfiles.h"
#include <stdlib.h>


#include <QDir>
#include <QFile>
#include <QFileInfo>

#define FILES_TO_CREATE   2

TempFiles::TempFiles() : m_content(QByteArray(4*4096, 'z'))
{
    m_dir = QDir::tempPath();
}


bool TempFiles::addSubDirLevel(const QString &dir)
{
    QFileInfo d;
    QFileInfo dirInfo(dir);
    if (dirInfo.isRelative())
    {
        d.setFile(m_dir, dir); //append a single directory
    }
    else
    {
        d.setFile(dir);       //use already made temp path
    }
    if (!d.exists() && QDir().mkpath(d.absoluteFilePath()))
    {             
        QFile::setPermissions(d.absoluteFilePath(),  QFile::WriteOwner | QFile::ReadOwner | QFile::ExeOwner
                                                   | QFile::WriteGroup | QFile::ReadGroup | QFile::ExeGroup
                                                   | QFile::WriteOther | QFile::ReadOther | QFile::ExeOther);
    }
    if (d.exists())
    {
        m_dir = d.absoluteFilePath();
        return true;
    }
    return false;
}

void TempFiles::removeAll()
{
    int counter = m_filesCreated.count();
    while(counter--)
    {
        if (QFileInfo(m_filesCreated.at(counter)).exists())
        {
            QFile::remove(m_filesCreated.at(counter));
            m_filesCreated.removeAt(counter);
        }
    }
}


QStringList TempFiles::createdNames()
{
    QStringList names;
    int counter = m_filesCreated.count();
    while(counter--) {
        names.append(QFileInfo(m_filesCreated.at(counter)).fileName());
    }
    return names;
}

int TempFiles::howManyExist()
{
    int ret = 0;
    int counter = m_filesCreated.count();
    while(counter--)
    {
        if (QFileInfo(m_filesCreated.at(counter)).exists())
        {
           ret++;
        }
    }
    return ret;
}

bool TempFiles::create(int counter)
{
   return  create(QLatin1String("tempfile"), counter);
}

bool TempFiles::create(const QString& name, int counter )
{
    return createPrivate(name, counter, true);
}

bool TempFiles::touch(int counter)
{
    return  touch(QLatin1String("emptyfile"), counter);
}

bool TempFiles::touch(const QString& name, int counter )
{
    return createPrivate(name, counter, false);
}

bool TempFiles::createPrivate(const QString& name, int counter, bool putContent)
{
    QString myName;
    while(counter--)
    {
        myName.sprintf("%s%c%s_%02d", m_dir.toLocal8Bit().constData(),
                       QDir::separator().toLatin1(),
                       name.toLocal8Bit().constData(),
                       counter);
        QFile file(myName);
        if (file.open(QFile::WriteOnly))
        {
            m_filesCreated.append(myName);
            if(putContent)
            {
                if (file.write(m_content) == (qint64)m_content.size())
                {
                    m_content += QByteArray(1024, 'z');
                }
                else {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }

    }
    return true;
}






QString TempFiles::lastFileCreated()
{
    QString ret;
    if (m_filesCreated.count() > 0)
    {
        ret = m_filesCreated.at(m_filesCreated.count() -1);
    }
    return ret;
}

QString TempFiles::lastNameCreated()
{
    QFileInfo f(lastFileCreated());
    return f.fileName();
}

DeepDir::DeepDir(const QString &rootDir, int level) :
    root(QDir::tempPath() + QDir::separator() + rootDir),
    totalFiles(0),
    totalItems(0)
{
    QFileInfo rootDirInfo(rootDir);
    if (rootDirInfo.isAbsolute())
    {
       root = rootDir;
    }
    if (!rootDir.isEmpty())
    {
        remove(); // clear
        QString levelStr;
        TempFiles temp;
        if (temp.addSubDirLevel(rootDir))
        {
            for(int counter=1 ; counter <= level; counter++)
            {
                levelStr.sprintf("level_%02d", counter);
                if ( !temp.addSubDirLevel(levelStr) || !temp.create(FILES_TO_CREATE) )
                {
                    break;
                }
                totalFiles += FILES_TO_CREATE;
                totalItems += FILES_TO_CREATE + 1;
                if (counter == 1)
                {
                   firstDirLevel =  temp.lastPath();
                }
            }
            lastDirLevel = temp.lastPath();
        }
    }
    else
    {
        root.clear();
    }
}

bool DeepDir::remove()
{
    bool ret = false;
    if (!root.isEmpty() && QFileInfo(root).exists())
    {
        QString cmd("/bin/rm -rf " + root);
        ret = ::system(cmd.toLocal8Bit().constData()) == 0 ;
        if (!ret)
        {
            qWarning("*** Could not remove %s, if it refers to Samba try to configure Samba using: 'force user' or 'create mask' plus 'directory mask'",
                     qPrintable(root));
        }
    }
    return ret;
}



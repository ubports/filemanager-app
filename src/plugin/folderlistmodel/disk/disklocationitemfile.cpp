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
 * File: disklocationitemfile.cpp
 * Date: 20/04/2015
 */

#include "disklocationitemfile.h"

DiskLocationItemFile::DiskLocationItemFile(QObject *parent)
    : LocationItemFile(parent)
    , m_qtFile( new QFile() )
{

}

DiskLocationItemFile::DiskLocationItemFile(const QString &name, QObject *parent)
    : LocationItemFile(parent)
    , m_qtFile( new QFile(name) )
{

}

DiskLocationItemFile::~DiskLocationItemFile()
{
    delete m_qtFile;
}

QString DiskLocationItemFile::fileName() const
{
    return m_qtFile->fileName();
}

bool DiskLocationItemFile::rename(const QString &newName)
{
    return m_qtFile->rename(newName);
}

bool DiskLocationItemFile::rename(const QString &oldname, const QString &newName)
{
    return QFile::rename(oldname, newName);
}

bool DiskLocationItemFile::remove()
{
    return m_qtFile->remove();
}

bool DiskLocationItemFile::remove(const QString &name)
{
    return QFile::remove(name);
}

bool DiskLocationItemFile::link(const QString &linkName)
{
    return m_qtFile->link(linkName);
}

bool DiskLocationItemFile::open(QIODevice::OpenMode mode)
{
    return m_qtFile->open(mode);
}

qint64 DiskLocationItemFile::read(char *buffer, qint64 bytes)
{
    return m_qtFile->read(buffer, bytes);
}

qint64 DiskLocationItemFile::write(const char *buffer, qint64 bytes)
{
    return m_qtFile->write(buffer, bytes);
}

void DiskLocationItemFile::close()
{
    m_qtFile->close();
}

bool DiskLocationItemFile::atEnd() const
{
    return m_qtFile->atEnd();
}

qint64 DiskLocationItemFile::size() const
{
    return m_qtFile->size();
}

bool DiskLocationItemFile::isOpen() const
{
    return m_qtFile->isOpen();
}

bool DiskLocationItemFile::setPermissions(QFileDevice::Permissions perm)
{
    return m_qtFile->setPermissions(perm);
}

bool DiskLocationItemFile::setPermissions(const QString &filename, QFileDevice::Permissions perm)
{
    return QFile::setPermissions(filename, perm);
}

QFile::Permissions DiskLocationItemFile::permissions() const
{
    return m_qtFile->permissions();
}

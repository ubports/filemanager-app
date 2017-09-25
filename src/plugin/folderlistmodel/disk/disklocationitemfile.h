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
 * File: disklocationitemfile.h
 * Date: 20/04/2015
 */

#ifndef DISKLOCATIONITEMFILE_H
#define DISKLOCATIONITEMFILE_H

#include "locationitemfile.h"

class DiskLocationItemFile : public LocationItemFile
{
    Q_OBJECT
public:
    explicit DiskLocationItemFile(QObject *parent);
    explicit DiskLocationItemFile(const QString &name, QObject *parent);
    ~DiskLocationItemFile();

public:
    virtual QString fileName() const;
    virtual bool rename(const QString &newName);
    virtual bool rename(const QString &oldname, const QString &newName);
    virtual bool remove();
    virtual bool remove(const QString &name);
    virtual bool link(const QString &linkName);
    virtual bool open(QFile::OpenMode mode) ;
    virtual qint64 read(char *, qint64);
    virtual qint64 write(const char *, qint64);
    virtual void close();
    virtual bool atEnd() const;
    virtual qint64 size() const;
    virtual bool isOpen() const;
    virtual bool setPermissions(const QString &filename, QFile::Permissions perm);
    virtual bool setPermissions(QFile::Permissions perm);
    virtual QFile::Permissions permissions() const;

private:
    QFile *m_qtFile;
};

#endif // DISKLOCATIONITEMFILE_H

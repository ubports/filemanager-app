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
 * File: locationitemfile.h
 * Date: 20/04/2015
 */

#ifndef LOCATIONITEMFILE_H
#define LOCATIONITEMFILE_H

#include <QFile>
#include <sys/types.h>



/*!
 * \brief The LocationItemFile class is an abstract class similar to Qt QFile
 *
 *  It will be used in Actions like copy/paste
 */
class LocationItemFile : public QObject
{
    Q_OBJECT
protected:
    explicit LocationItemFile(QObject *parent = 0);
    explicit LocationItemFile(const QString &name, QObject *parent = 0);
public:
    virtual  ~LocationItemFile();
public:
    virtual QString fileName() const = 0;
    virtual bool   rename(const QString &newName) = 0;
    virtual bool   rename(const QString &oldname, const QString &newName) = 0;
    virtual bool   remove() = 0;
    virtual bool   remove(const QString &name) = 0;
    virtual bool   link(const QString &linkName) = 0;
    virtual bool   open(QFile::OpenMode mode) = 0 ;
    virtual qint64 read(char *, qint64) = 0;
    virtual qint64 write(const char *, qint64) = 0;
    virtual void   close() = 0;
    virtual bool   atEnd() const = 0;
    virtual qint64 size() const = 0;
    virtual bool   isOpen() const = 0;
    virtual bool   setPermissions(const QString &filename, QFile::Permissions perm) = 0;
    virtual bool   setPermissions(QFile::Permissions perm) = 0;
    virtual QFile::Permissions permissions() const = 0;
public: //static functions
    static  mode_t  getUmask();                        //return the current umask
    static  mode_t  getUmaskCreationMode(mode_t mode); // mode & ~umask;
    static  mode_t  getUmaskFilesCreation();           // 0666 & ~umask;
    static  mode_t  getUmaskDirsCreation();            // 0777 & ~umask;
    static  mode_t  unixPermissions(QFile::Permissions);
signals:

public slots:

};

#endif // LOCATIONITEMFILE_H

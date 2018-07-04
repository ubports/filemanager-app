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
 * File: locationitemfile.cpp
 * Date: 20/04/2015
 */

#include "locationitemfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

LocationItemFile::LocationItemFile(QObject *parent) :
    QObject(parent)
{
}

LocationItemFile::LocationItemFile(const QString &, QObject *parent) :
    QObject(parent)
{
}

LocationItemFile::~LocationItemFile()
{

}


mode_t LocationItemFile::getUmask()
{
    mode_t mask  = ::umask(0);  //first gets the current umask and sets to 0
    ::umask(mask);              //second restores the current umask
    return mask;
}

/*!
 * \brief LocationItemFile::getUmaskCreationMode() Returns a suitable open creation mode for system calls like open() and mkdir() respecting umask()
 * \param mode
 * \return the open mode in the form: mode & ~umask
 */
mode_t LocationItemFile::getUmaskCreationMode(mode_t mode)
{
    mode_t mask = getUmask();
    mode_t umode =  mode & ~mask;
    return umode;
}

/*!
 * \brief LocationItemFile::getUmaskFilesCreation() Returns the default open mode for files
 * \return
 */
mode_t LocationItemFile::getUmaskFilesCreation()
{
    return LocationItemFile::getUmaskCreationMode(0666);
}


/*!
 * \brief LocationItemFile::getUmaskFilesCreation() Returns the default open mode for directories
 * \return
 */
mode_t LocationItemFile::getUmaskDirsCreation()
{
    return LocationItemFile::getUmaskCreationMode(0777);
}


mode_t LocationItemFile::unixPermissions(QFileDevice::Permissions perm)
{
#define SETMODE(qtPerm, Uperm)  if (perm & qtPerm) { mode |= Uperm; }
    mode_t mode = 0;
    SETMODE((QFile::ReadOwner  | QFile::ReadUser),  S_IRUSR);
    SETMODE((QFile::WriteOwner | QFile::WriteUser), S_IWUSR);
    SETMODE((QFile::ExeOwner   | QFile::ExeUser),   S_IXUSR);
    SETMODE(QFile::ReadGroup,  S_IRGRP);
    SETMODE(QFile::WriteGroup, S_IWGRP);
    SETMODE(QFile::ExeGroup,   S_IXGRP);
    SETMODE(QFile::ReadOther,  S_IROTH);
    SETMODE(QFile::WriteOther, S_IWOTH);
    SETMODE(QFile::ExeOther,   S_IXOTH);
    return mode;
}

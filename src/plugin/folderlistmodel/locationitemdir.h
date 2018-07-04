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
 * File: locationitemdir.h
 * Date: 16/05/2015
 */

#ifndef LOCATIONITEMDIR_H
#define LOCATIONITEMDIR_H

#include <QString>

class LocationItemDir
{
public:
    virtual  ~LocationItemDir();
public:
    virtual bool    exists()                   const = 0;
    virtual bool    mkdir(const QString &dir)  const = 0;
    virtual bool    mkpath(const QString &dir) const = 0;
    virtual bool    rmdir(const QString &dir)  const = 0;
protected:
    LocationItemDir ();
    LocationItemDir (const QString &dir =  QLatin1String(0));
};

#endif // LOCATIONITEMDIR_H

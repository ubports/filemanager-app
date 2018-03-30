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
 * File: disklocationitemdir.cpp
 * Date: 16/05/2015
 */

#include "disklocationitemdir.h"
#include <QDir>

DiskLocationItemDir::DiskLocationItemDir(const QString &dir) :
    LocationItemDir(dir)
    , m_qtQDir(new QDir())
{
    if (!dir.isNull() && !dir.isEmpty()) {
        m_qtQDir->setPath(dir);
    }
}

DiskLocationItemDir::~DiskLocationItemDir()
{
    delete m_qtQDir;
}

bool DiskLocationItemDir::exists() const
{
    return m_qtQDir->exists();
}

bool DiskLocationItemDir::mkdir(const QString &dir) const
{
    return m_qtQDir->mkdir(dir);
}

bool DiskLocationItemDir::mkpath(const QString &dir) const
{
    return m_qtQDir->mkpath(dir);
}

bool DiskLocationItemDir::rmdir(const QString &dir) const
{
    return m_qtQDir->rmdir(dir);
}


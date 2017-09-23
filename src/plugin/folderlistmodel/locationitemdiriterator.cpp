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
 * File: locationitemdiriterator.cpp
 * Date: 10/01/2015
 */

#include "locationitemdiriterator.h"

LocationItemDirIterator::LocationItemDirIterator(const QString &path,
                                                 const QStringList &nameFilters,
                                                 QDir::Filters filters,
                                                 QDirIterator::IteratorFlags flags,
                                                 LocationItemDirIterator::LoadMode loadmode)
    : m_path(path)
    , m_nameFilters(nameFilters)
    , m_filters(filters)
    , m_flags(flags)

{
    Q_UNUSED(loadmode);
}


LocationItemDirIterator::LocationItemDirIterator(const QString &path,
                                                 QDir::Filters filters,
                                                 QDirIterator::IteratorFlags flags,
                                                 LocationItemDirIterator::LoadMode loadmode)
    : m_path(path)
    , m_filters(filters)
    , m_flags(flags)
{
    Q_UNUSED(loadmode);
}

LocationItemDirIterator::LocationItemDirIterator(const QString &path,
                                                 QDirIterator::IteratorFlags flags,
                                                 LocationItemDirIterator::LoadMode loadmode)
    : m_path(path)
    , m_filters(QDir::NoFilter)
    , m_flags(flags)
{
    Q_UNUSED(loadmode);
}

LocationItemDirIterator::~LocationItemDirIterator()
{

}


QDirIterator::IteratorFlags LocationItemDirIterator::flags() const
{
    return m_flags;
}

QDir::Filters LocationItemDirIterator::filters() const
{
    return m_filters;
}

QString LocationItemDirIterator::path() const
{
    return m_path;
}

void LocationItemDirIterator::load()
{
    //default implementation does nothing
}

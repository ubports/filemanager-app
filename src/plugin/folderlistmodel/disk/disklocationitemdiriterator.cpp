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
 * File: disklocationitemdiriterator.cpp
 * Date: 29/03/2015
 */

#include "disklocationitemdiriterator.h"

#include <QDirIterator>

DiskLocationItemDirIterator::DiskLocationItemDirIterator(const QString &path, const QStringList &nameFilters,
                                                         QDir::Filters filters, QDirIterator::IteratorFlags flags)
    : LocationItemDirIterator(path, nameFilters, filters, flags)
    , m_qtDirIterator(new QDirIterator(path, nameFilters, filters, flags))
{

}

DiskLocationItemDirIterator::DiskLocationItemDirIterator(const QString &path, QDir::Filters filters, QDirIterator::IteratorFlags flags)
    : LocationItemDirIterator(path, filters, flags)
    , m_qtDirIterator(new QDirIterator(path, filters, flags))
{

}

DiskLocationItemDirIterator::DiskLocationItemDirIterator(const QString &path, QDirIterator::IteratorFlags flags)
    : LocationItemDirIterator(path, flags)
    , m_qtDirIterator(new QDirIterator(path, flags))
{

}

DiskLocationItemDirIterator::~DiskLocationItemDirIterator()
{
    delete m_qtDirIterator;
}

DirItemInfo DiskLocationItemDirIterator::fileInfo() const
{
    DirItemInfo itemInfo (m_qtDirIterator->fileInfo());
    return itemInfo;
}

QString DiskLocationItemDirIterator::fileName() const
{
    return m_qtDirIterator->fileName();
}

QString DiskLocationItemDirIterator::filePath() const
{
    return m_qtDirIterator->filePath();
}

bool DiskLocationItemDirIterator::hasNext() const
{
    return m_qtDirIterator->hasNext();
}

QString DiskLocationItemDirIterator::next()
{
    return m_qtDirIterator->next();
}

QString DiskLocationItemDirIterator::path() const
{
    return m_qtDirIterator->path();
}

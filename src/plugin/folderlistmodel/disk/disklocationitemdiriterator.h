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
 * File: disklocationitemdiriterator.h
 * Date: 29/03/2015
 */

#ifndef DISKLOCATIONITEMDIRITERATOR_H
#define DISKLOCATIONITEMDIRITERATOR_H

#include "locationitemdiriterator.h"

class QDirIterator;

class DiskLocationItemDirIterator : public LocationItemDirIterator
{
public:
    explicit DiskLocationItemDirIterator(QObject *parent = 0);
    ~DiskLocationItemDirIterator();

public:
    virtual DirItemInfo fileInfo() const;
    virtual QString fileName() const;
    virtual QString filePath() const;
    virtual bool hasNext() const;
    virtual QString next();
    virtual QString path() const;

public:
    DiskLocationItemDirIterator(const QString &path, QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags);
    DiskLocationItemDirIterator(const QString &path, QDir::Filters filters, QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags);
    DiskLocationItemDirIterator(const QString &path, const QStringList &nameFilters, QDir::Filters filters = QDir::NoFilter, QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags);

private:
    QDirIterator *m_qtDirIterator;

};

#endif // DISKLOCATIONITEMDIRITERATOR_H

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
 * File: locationitemdiriterator.h
 * Date: 10/01/2015
 */

#ifndef LOCATIONITEMDIRITERATOR_H
#define LOCATIONITEMDIRITERATOR_H

#include <QDirIterator>
#include "diriteminfo.h"

/*!
 * \brief The LocationItemDirIterator class is an abstract similar to Qt QDirIterator
 *
 *   Different protocols supported by filemanager (different Locations) must provide a class like that.
 *
 *   The \ref LoadLater can used in the constructor to indicate to the constructor to NOT load the path/url, instead \ref load() can called later to do that.
 */

class LocationItemDirIterator
{
public:
    enum LoadMode {
        LoadOnConstructor,  //!< loads the entire directory or url in the constructor as Qt QDirIterator does
        LoadLater           //!< do NOT load the entire directory or url, \ref load() method should be responsible to do that.
    };

    virtual ~LocationItemDirIterator();
public:
    virtual bool         hasNext()  const = 0;
    virtual QString      next()           = 0;

    virtual DirItemInfo  fileInfo() const = 0;
    /*!
    * \brief fileName()
    * \return the file name for the current directory entry, without the path prepended.
    */
    virtual QString      fileName() const = 0;

    /*!
    * \brief filePath()
    * \return the full pathname of the current item
    */
    virtual QString          filePath() const = 0;

    /*!
    * \brief path()
    * \return  the base directory of the iterator path (not the current item)
    */
    virtual QString          path()     const;

    /*!
     * \brief load()  responsible to load the entire directory or url when \ref LoadLater is passed to the constructor
     */
    virtual void         load() ;

    QDir::Filters        filters()  const;
    QDirIterator::IteratorFlags flags() const;

protected:
    LocationItemDirIterator(const QString &path,
                            QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                            LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

    LocationItemDirIterator(const QString &path,
                            QDir::Filters filters,
                            QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                            LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

    LocationItemDirIterator(const QString &path,
                            const QStringList &nameFilters,
                            QDir::Filters filters = QDir::NoFilter,
                            QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                            LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor);

protected:
    QString                     m_path;
    QStringList                 m_nameFilters;
    QDir::Filters               m_filters;
    QDirIterator::IteratorFlags m_flags;
};


#endif // LOCATIONITEMDIRITERATOR_H

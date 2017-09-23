/**************************************************************************
 *
 * Copyright 2014 Canonical Ltd.
 * Copyright 2014 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: urliteminfo.h
 * Date: 08/12/2014
 */

#ifndef URLITEMINFO_H
#define URLITEMINFO_H

#include "diriteminfo.h"


/*!
 * \brief The UrlItemInfo is an abstract class that provides URL root
 *
 *  Basically it differs from DirItemInfo in the field \a d_ptr->_normalizedPath, it must store the
 *  url like trash:///Item, while the field d_ptr->_path stores the current path in the file system as usual.
 *
 */

class UrlItemInfo : public DirItemInfo
{
public:
    static QStringList separatePathFilename(const QString &urlPath);
protected:
    UrlItemInfo(const QString &urlPath, const QString &urlRoot);
    UrlItemInfo();

protected:
    void               setRoot(const QString &urlRoot);


private:
    void    init(const QString &urlPath);
    void    verifyHost(const QString urlPath);
};

#endif // URLITEMINFO_H

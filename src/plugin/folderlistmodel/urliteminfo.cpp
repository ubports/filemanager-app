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
 * File: urliteminfo.cpp
 * Date: 08/12/2014
 */

#include "urliteminfo.h"

#include <QUrl>

UrlItemInfo::UrlItemInfo():    DirItemInfo()
{

}

UrlItemInfo::UrlItemInfo(const QString &urlPath, const QString &urlRoot):
    DirItemInfo()
{
    if (!urlPath.isEmpty()) {
        if (urlPath == urlRoot) {
            setRoot(urlPath);
        } else {
            if (!urlPath.startsWith(urlRoot)) {
                d_ptr->_isValid    = false;
                d_ptr->_isAbsolute = false;
            } else {
                init(urlPath);
            }
        }
    }
}


void UrlItemInfo::setRoot(const QString &urlPath)
{
    d_ptr->_isValid      = true;
    d_ptr->_isRoot       = true;
    d_ptr->_isDir        = true;
    d_ptr->_isReadable   = true;
    d_ptr->_isExecutable = true;
    d_ptr->_exists       = true;
    d_ptr->_isAbsolute   = true;
    d_ptr->_isRemote     = true;
    d_ptr->_fileName.clear();
    d_ptr->_path         = urlPath;
    d_ptr->_normalizedPath = d_ptr->_path;
}


/*!
 * \brief UrlItemInfo::init() fill basic item information
 *
 * It is supposed the URL does contain duplicates slashes
 *
 * \param urlPath
 */
void UrlItemInfo::init(const QString &urlPath)
{
    d_ptr->_isValid      = true;
    d_ptr->_isAbsolute   = true;
    d_ptr->_isRemote     = true;

    //veryfy if the item is a host
    verifyHost(urlPath);

    QStringList pathAndFile = separatePathFilename(urlPath);
    if (pathAndFile.count() == 2) {
        d_ptr->_path     = pathAndFile.at(0);
        d_ptr->_fileName = pathAndFile.at(1);
    } else {
        d_ptr->_path           = urlPath;
    }
    d_ptr->_normalizedPath = d_ptr->_path;
}


void UrlItemInfo::verifyHost(const QString urlPath)
{
    QUrl url(urlPath);
    if (url.isValid() && !url.isLocalFile()) {
        if (url.path().isEmpty()) {
            //!< initial set is "host", Samba shares also have Workspace which will be handled in \ref SmbItemInfo
            setAsHost();
        }
    }
}


/*!
 * \brief UrlItemInfo::separatePathFilename()
 * \param urlPath
 * \return [0]=path [1]=filename when it exists
 */
QStringList UrlItemInfo::separatePathFilename(const QString &urlPath)
{
    QStringList separated;
    int lastDir = urlPath.lastIndexOf(QDir::separator());
    //path and filename must fill the url,
    // smb://localost      path=smb://          filename=localhost
    // smb://localhost/dir path=smb://localhost filename=dir
    if (lastDir != -1) {
        QString path = urlPath.at(lastDir - 1)  == QDir::separator() ?
                       urlPath.left(lastDir + 1) : urlPath.left(lastDir);

        separated.append(path);
        separated.append(urlPath.mid(lastDir + 1));
    }
    return separated;
}

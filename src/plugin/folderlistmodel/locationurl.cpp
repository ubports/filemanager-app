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
 * File: locationurl.cpp
 * Date: 11/03/2014
 */

#include "locationurl.h"
#include <QUrl>

const QString LocationUrl::UrlIndicator("://");

const QString LocationUrl::TrashRootURL("trash:///");
const QString LocationUrl::DiskRootURL("file:///");
const QString LocationUrl::SmbURL("smb://");
const QString LocationUrl::CifsURL("cifs://");
#if 0
QString LocationURL::FishURL("fish:///");
#endif
// keep this list ordered
const QStringList LocationUrl::m_supportedURLs = QStringList() << LocationUrl::CifsURL
                                                 << LocationUrl::DiskRootURL
                                                 << LocationUrl::SmbURL
                                                 << LocationUrl::TrashRootURL
                                                 ;

LocationUrl::LocationUrl()
{

}


const QStringList &LocationUrl::supportedURLs()
{
    return m_supportedURLs;
}


bool LocationUrl::isSupportedUrl(const QUrl &url)
{
    bool ret = url.isValid() && url.isLocalFile(); // local files does not need to check
    if (!ret && !url.scheme().isEmpty()) {
        int counter = m_supportedURLs.count();
        while (!ret && counter--) {
            ret = m_supportedURLs.at(counter).startsWith(url.scheme(), Qt::CaseSensitive);
        }
    }
    return ret;
}

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
 * File: locationurl.h
 * Date: 11/03/2014
 */

#ifndef LOCATIONURL_H
#define LOCATIONURL_H

#include <QStringList>
class QUrl;

class LocationUrl
{
public:
    static const   QString UrlIndicator;
    static const   QString DiskRootURL;
    static const   QString TrashRootURL;
    static const   QString SmbURL;
    static const   QString CifsURL;
#if 0
    static const   QString FishURL;
#endif
    /*!
     * \brief supportedURLs() Tells which URLs are supported by file manager
     *
     *   It may be useful for Clipboard handling
     *
     * \return URLs list
     */
    static const QStringList  &supportedURLs();
    static bool                isSupportedUrl(const QUrl &url);
private:
    LocationUrl();
    static   const QStringList   m_supportedURLs;
};

#endif // LOCATIONURL_H

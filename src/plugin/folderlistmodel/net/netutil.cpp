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
 * File: netutil.cpp
 * Date: 29/11/2014
 */

#include "netutil.h"
#include <QHostAddress>
#include <QHostInfo>

#include <QDebug>

NetUtil::NetUtil()
{
}


QString NetUtil::normalizeHostName(const QString& name)
{
    QString host(name.toLower());
    bool isLoopBack = false;
    QHostInfo info = QHostInfo::fromName(host);
    // take advantage of network with Bonjour/Avahi
    // as winbind looks like harder to configure or does not work
    if (info.error() == QHostInfo::HostNotFound)
    {
        host += QLatin1String(".local");
        info = QHostInfo::fromName(host);
    }
    if (info.error() == QHostInfo::NoError)
    {
        host = info.hostName();
        QList<QHostAddress> addrs  = info.addresses();
        int counter = addrs.count();
        while (!isLoopBack && counter--)
        {
            isLoopBack = addrs.at(counter).isLoopback();
        }
    }
    if (isLoopBack)
    {
         host = QLatin1String("localhost");
    }
    return host;
}

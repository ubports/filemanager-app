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
#include <QUrl>

#include <QDebug>

NetUtil::NetUtil()
{ }

QString NetUtil::normalizeHostName(const QString &name)
{
    QString host(name.toLower());
    bool isLoopBack = false;
    QHostInfo info = QHostInfo::fromName(host);

    // take advantage of network with Bonjour/Avahi
    // as winbind looks like harder to configure or does not work
    if (info.error() == QHostInfo::HostNotFound) {
        host += QLatin1String(".local");
        info = QHostInfo::fromName(host);
    }

    if (info.error() == QHostInfo::NoError) {
        host = info.hostName();
        QList<QHostAddress> addrs  = info.addresses();
        int counter = addrs.count();

        while (!isLoopBack && counter--) {
            isLoopBack = addrs.at(counter).isLoopback();
        }
    }

    if (isLoopBack) {
        host = QLatin1String("localhost");
    }

    return host;
}

/*!
 * \brief NetUtil::urlConvertHostnameToIP()  Tries to convert an url like protocol://hostname/blavbla to protocol://ip-address/blavbla
 * \param url
 * \return the url using IP numbers or an empty string saying that was not possible to get its IP number
 */
QString NetUtil::urlConvertHostnameToIP(const QString &url)
{
    QString ret;
    QUrl tmpUrl(url);

    if (tmpUrl.isValid() && !tmpUrl.host().isEmpty() && tmpUrl.host() != QLatin1String("localhost")) {
        QString host = tmpUrl.host();
        QHostInfo info = QHostInfo::fromName(host);

        if (info.error() == QHostInfo::HostNotFound) {
            // take advantage of network with Bonjour/Avahi
            // as winbind looks like harder to configure or does not work
            info = QHostInfo::fromName(host + QLatin1String(".local"));
        }

        if (info.error() == QHostInfo::NoError) {
            tmpUrl.setHost(info.addresses().at(0).toString());
            ret = tmpUrl.toString();
        }
    }

    return ret;
}

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
 * File: cleanurl.cpp
 * Date: 04/02/2015
 */

#include "cleanurl.h"

#include <QUrl>

CleanUrl::CleanUrl(const QString &urlPath) : m_user(0), m_password(0)
{
    QUrl  url(urlPath);

    if (url.isValid()) {
        QString user = url.userName();

        if (!user.isEmpty()) {
            m_user = new QString(user);
            m_password = new QString(url.password());

            url.setPassword(QLatin1String(0));
            url.setUserName(QLatin1String(0));
        }

        m_url = url.toString();

    } else {
        m_url = urlPath;
    }
}


CleanUrl::~CleanUrl()
{
    if (m_user) {
        delete m_user;
    }

    if (m_password) {
        delete m_password;
    }
}


bool CleanUrl::hasAuthenticationData() const
{
    return m_user ? true : false;
}

QString CleanUrl::user() const
{
    return m_user ? *m_user : QString();
}

QString CleanUrl::password() const
{
    return m_password ? *m_password : QString();
}

QString CleanUrl::cleanUrl() const
{
    return m_url;
}

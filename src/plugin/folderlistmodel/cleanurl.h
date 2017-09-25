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
 * File: cleanurl.h
 * Date: 04/02/2015
 */

#ifndef CLEANURL_H
#define CLEANURL_H

#include <QString>

/*!
 * \brief The CleanUrl class
 *
 *  Just returns a URL without user/password if exists
 */
class CleanUrl
{
public:
    CleanUrl(const QString &urlPath);
    ~CleanUrl();

    bool hasAuthenticationData() const;
    QString cleanUrl() const;
    QString user() const;
    QString password() const;

private:
    QString      m_url;   //!<  keeps the url without user/password
    QString     *m_user;
    QString     *m_password;
};

#endif // CLEANURL_H

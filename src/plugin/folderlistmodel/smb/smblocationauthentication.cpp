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
 * File: smblocationauthentication.cpp
 * Date: 17/01/2015
 */

#include "smblocationauthentication.h"
#include <QDebug>

#define  GOOD_INDEX()  (m_infoIndex >= 0 && m_infoIndex < MAX_AUTH_INSTANCES)

#if defined(REGRESSION_TEST_FOLDERLISTMODEL) && defined(SIMPLE_UI)
# define DEBUG_AUTHENTICATION()  qDebug() << Q_FUNC_INFO << "user:" << user << "passwd:" << passwd << "server:" << server << "share:" << share
#else
# define DEBUG_AUTHENTICATION() /**/
#endif

namespace {
QByteArray  m_AuthUser[MAX_AUTH_INSTANCES];
QByteArray  m_AuthPass[MAX_AUTH_INSTANCES];
void       *m_instances[MAX_AUTH_INSTANCES];
}

SmbLocationAuthentication::SmbLocationAuthentication() : m_infoIndex(-1)
{
    for (int counter = 0; counter < MAX_AUTH_INSTANCES; ++counter) {
        if (m_instances[counter] == 0) {
            m_infoIndex = counter;
            m_instances[m_infoIndex] = this;
            break;
        }
    }
}

/*!
 * \brief SmbLocationAuthentication::~SmbLocationAuthentication
 */
SmbLocationAuthentication::~SmbLocationAuthentication()
{
    if (GOOD_INDEX()) {
        m_instances[m_infoIndex] = 0;
    } else {
        qDebug() << Q_FUNC_INFO << "ERROR no m_instances[] index";
    }
}

/*!
 * \brief SmbLocationAuthentication::setInfo
 * \param user
 * \param password
 */
void SmbLocationAuthentication::setInfo(const QString &user, const QString &password)
{
    if (GOOD_INDEX()) {
        m_AuthUser[m_infoIndex] = user.toLocal8Bit();
        m_AuthPass[m_infoIndex] = password.toLocal8Bit();

    } else {
        qDebug() << Q_FUNC_INFO << "ERROR no m_instances[] index";
    }
}

/*!
 * \brief SmbLocationAuthentication::suitableAuthenticationFunction
 * \return
 */
Smb::AuthenticationFunction SmbLocationAuthentication::suitableAuthenticationFunction() const
{
    switch (m_infoIndex) {
    case 0:
        return &SmbLocationAuthentication::authenticateCallBack0;
    case 1:
        return &SmbLocationAuthentication::authenticateCallBack1;
    case 2:
        return &SmbLocationAuthentication::authenticateCallBack2;
    case 3:
        return &SmbLocationAuthentication::authenticateCallBack3;
    default:
        return 0;
    }
}

/*!
 * \brief SmbLocationAuthentication::authenticateCallBack0
 * \param server
 * \param share
 * \param wrkgrp
 * \param wrkgrplen
 * \param user
 * \param userlen
 * \param passwd
 * \param passwdlen
 */
void SmbLocationAuthentication::authenticateCallBack0(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen)
{
    Q_UNUSED(server);
    Q_UNUSED(share);
    Q_UNUSED(wrkgrp);
    Q_UNUSED(wrkgrplen);

    ::strncpy(user,   m_AuthUser[0].constData(),  --userlen);
    ::strncpy(passwd, m_AuthPass[0].constData(),  --passwdlen);
    DEBUG_AUTHENTICATION();
}

/*!
 * \brief SmbLocationAuthentication::authenticateCallBack1
 * \param server
 * \param share
 * \param wrkgrp
 * \param wrkgrplen
 * \param user
 * \param userlen
 * \param passwd
 * \param passwdlen
 */
void SmbLocationAuthentication::authenticateCallBack1(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen)
{
    Q_UNUSED(server);
    Q_UNUSED(share);
    Q_UNUSED(wrkgrp);
    Q_UNUSED(wrkgrplen);

    ::strncpy(user,   m_AuthUser[1].constData(),  --userlen);
    ::strncpy(passwd, m_AuthPass[1].constData(),  --passwdlen);
    DEBUG_AUTHENTICATION();
}

/*!
 * \brief SmbLocationAuthentication::authenticateCallBack2
 * \param server
 * \param share
 * \param wrkgrp
 * \param wrkgrplen
 * \param user
 * \param userlen
 * \param passwd
 * \param passwdlen
 */
void SmbLocationAuthentication::authenticateCallBack2(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen)
{
    Q_UNUSED(server);
    Q_UNUSED(share);
    Q_UNUSED(wrkgrp);
    Q_UNUSED(wrkgrplen);

    ::strncpy(user,   m_AuthUser[2].constData(),  --userlen);
    ::strncpy(passwd, m_AuthPass[2].constData(),  --passwdlen);
    DEBUG_AUTHENTICATION();
}

/*!
 * \brief SmbLocationAuthentication::authenticateCallBack3
 * \param server
 * \param share
 * \param wrkgrp
 * \param wrkgrplen
 * \param user
 * \param userlen
 * \param passwd
 * \param passwdlen
 */
void SmbLocationAuthentication::authenticateCallBack3(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen)
{
    Q_UNUSED(server);
    Q_UNUSED(share);
    Q_UNUSED(wrkgrp);
    Q_UNUSED(wrkgrplen);

    ::strncpy(user,   m_AuthUser[3].constData(),  --userlen);
    ::strncpy(passwd, m_AuthPass[3].constData(),  --passwdlen);
    DEBUG_AUTHENTICATION();
}

QString SmbLocationAuthentication::currentAuthUser() const
{
    QString ret;
    if (GOOD_INDEX()) {
        ret = m_AuthUser[m_infoIndex];
    }

    return ret;
}

QString SmbLocationAuthentication::currentAuthPassword() const
{
    QString ret;
    if (GOOD_INDEX()) {
        ret = m_AuthPass[m_infoIndex];
    }

    return ret;
}

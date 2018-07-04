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
 * File: smblocationauthentication.h
 * Date: 17/01/2015
 */

#ifndef SMBLOCATIONAUTHENTICATION_H
#define SMBLOCATIONAUTHENTICATION_H

#include "smbutil.h"

/*!
    As the function to do the Samba authentication needs to be a static function
    it is necessary to have many static functions to allow many instances of SmbLocation
    (one SmbLocation instance matches a FileManager window) browsing for example the
    same samba URL using different users.
 */
#define MAX_AUTH_INSTANCES   4

/*!
 * \brief The SmbLocationAuthentication class provides authentication Samba Authentication functions
 *
 *  It intends to provide a set of functions and information (user/password) to have different instances of
 *  objects doing samba authentication at same time
 */
class SmbLocationAuthentication
{
public:
    SmbLocationAuthentication();
    ~SmbLocationAuthentication();

    Smb::AuthenticationFunction suitableAuthenticationFunction() const;

    void setInfo(const QString &user, const QString &password);
    QString currentAuthUser() const;
    QString currentAuthPassword() const;

    static void authenticateCallBack0(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen);
    static void authenticateCallBack1(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen);
    static void authenticateCallBack2(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen);
    static void authenticateCallBack3(const char *server, const char *share, char *wrkgrp, int wrkgrplen, char *user, int userlen, char *passwd, int passwdlen);

private:
    int  m_infoIndex;
};

#endif // SMBLOCATIONAUTHENTICATION_H

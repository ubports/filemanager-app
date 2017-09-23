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
 * File: netauthenticationdata.h
 * Date: 29/11/2014
 */

#ifndef NETAUTHENTICATIONDATA_H
#define NETAUTHENTICATIONDATA_H

#include <QHash>
#include <QUrl>

class QSettings;

/*!
 * \brief The NetAuthenticationData struct
 *
 *  Just keeps data for Network authentication (protocol independent)
 */

struct NetAuthenticationData {
public:
    NetAuthenticationData() {}
    NetAuthenticationData(const QString &u, const QString &p) : user(u), password(p) {}

    inline bool isEmpty() const
    {
        return user.isEmpty();
    }

    QString user;
    QString password;

    static const QString &currentUser();
    static const QString &noPassword();
};

/*!
 * \brief The NetAuthenticationDataList class
 *
 *  It keeps a list of the current URLs being used and their Authentication data
 *
 *  This information may be required for every connection (e.g. Samba shares).
 *
 *  It implements a singleton design pattern to keep a unique Authentication Data list per application
 *    \sa \ref getInstance() and \ref releaseInstance()
 *
 *  The authentication data can be saved for NOT always asking the user
 *
 *  \note It is intended to be used for any protocol.
 *        The URL being stored must be the part or prefix which requires authentication
 *        Example for Samba: if a user enters with a url like smb://localhost/any_share/folder1/folder2
 *                           only the share part requires authentication with is smb://localhost/any_share
 *                           The same authentication data can be used for any smb://localhost/any_share sub-items
 *
 */
class NetAuthenticationDataList
{
public:
    static NetAuthenticationDataList *getInstance(void *parent);
    static void releaseInstance(void *parent);
    ~NetAuthenticationDataList();

public:
    bool store(const QUrl &url, bool save = false);
    bool store(const QString &url, const QString &u, const QString &p, bool save = false);
    const NetAuthenticationData *get(const QString &) const;

private:
    NetAuthenticationDataList();

private:
    void openAuthenticationStore();
    void closeAuthenticationStore();
    QString encryptPassord(const QString &p);
    QString decryptPassword(const QString &p);
    void loadSavedAuthenticationData();
    bool saveAuthenticationData(const QString &url, const NetAuthenticationData * );

private:  //url     authentication data
    QHash<QString , NetAuthenticationData *> m_urlEntries;
    static NetAuthenticationDataList *m_instance;
    static void *m_parent;
    QSettings *m_savedAuths;
};

#endif // NETAUTHENTICATIONDATA_H

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
 * File: netauthenticationdata.cpp
 * Date: 29/11/2014
 */

#include "netauthenticationdata.h"

#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

#define CHAR_CRYPT_OFFSET  31

NetAuthenticationDataList *NetAuthenticationDataList::m_instance = 0;
void *NetAuthenticationDataList::m_parent = 0;

const QString &NetAuthenticationData::currentUser()
{
    static QString curUser(::qgetenv("USER"));
    return curUser;
}

const QString &NetAuthenticationData::noPassword()
{
    static QString emptyPassword;
    return emptyPassword;
}

NetAuthenticationDataList::NetAuthenticationDataList(): m_savedAuths(0)
{
    //settings file does not need to open  all the time
    loadSavedAuthenticationData();
}

NetAuthenticationDataList::~NetAuthenticationDataList()
{
    qDeleteAll(m_urlEntries);

    m_urlEntries.clear();
    m_parent  = 0;
    m_instance = 0;

    closeAuthenticationStore();
}

NetAuthenticationDataList *NetAuthenticationDataList::getInstance(void *parent)
{
    if (m_instance == 0) {
        m_instance = new NetAuthenticationDataList();
        m_parent  = parent;
    }

    return m_instance;
}

void NetAuthenticationDataList::releaseInstance(void *parent)
{
    if (parent == m_parent && m_instance != 0) {
        delete m_instance;
        m_instance = 0;
        m_parent = 0;
    }
}


const NetAuthenticationData *NetAuthenticationDataList::get(const QString &url) const
{
    const NetAuthenticationData *ret = 0;
    if (!url.isEmpty()) {
        ret = m_urlEntries.value(url);

        if (ret == 0) {
            //try to match cases where a more complete URL like smb://host/share/directory was entered and smb://host had been saved before
            QUrl hostUrl(url);
            hostUrl.setPath(QLatin1String(0));
            ret =  m_urlEntries.value(hostUrl.toString());
        }
    }

    return ret;
}

bool  NetAuthenticationDataList::store(const QString &url, const QString &u, const QString &p, bool save)
{
    bool ret = false;

    if (!url.isEmpty()) {
        ret = true;
        NetAuthenticationData *data = 0;

        if ( (data = const_cast<NetAuthenticationData *> (get(url))) == 0) {
            data = new NetAuthenticationData();
            m_urlEntries.insert(url, data);
        }

        data->user     = u;
        data->password = p;

        if (save ) {
            ret = saveAuthenticationData(url, data);
        }
    }

    return ret;
}

bool  NetAuthenticationDataList::store(const QUrl &url, bool save)
{
    QString user = url.userName();
    QString passwd = url.password();

    QUrl url2(url);
    url2.setUserName(QLatin1String(0));
    url2.setPassword(QLatin1String(0));

    return store(url2.toString(), user, passwd, save);
}

void NetAuthenticationDataList::loadSavedAuthenticationData()
{
    QLatin1Char slash('/');
    QLatin1String userKey("user");
    QLatin1String passKey("password");

    openAuthenticationStore();

    QStringList urls = m_savedAuths->childGroups();
    int counter = urls.count();
    while (counter--) {
        m_savedAuths->beginGroup(urls.at(counter));

        QString cleanUrl = urls.at(counter);

        cleanUrl.replace(QLatin1Char('}'), slash);

        QString user = m_savedAuths->value(userKey).toString();
        QString pass = m_savedAuths->value(passKey).toString();

        store(cleanUrl, user, decryptPassword(pass));

        m_savedAuths->endGroup();
    }

    closeAuthenticationStore();
}

bool NetAuthenticationDataList::saveAuthenticationData(const QString &url, const NetAuthenticationData *d)
{
    QLatin1Char slash('/');
    QLatin1String userKey("user");
    QLatin1String passKey("password");
    QString keyUrl(url);
    keyUrl.replace(slash, QLatin1Char('}'));

    openAuthenticationStore();
    m_savedAuths->setValue(keyUrl + slash + userKey, d->user);
    m_savedAuths->setValue(keyUrl + slash + passKey, encryptPassord(d->password));
    m_savedAuths->sync();

    bool ret = m_savedAuths->status() == QSettings::NoError;
    if (!ret) {
        qDebug() << Q_FUNC_INFO << "ERROR: could not save settings:" << m_savedAuths->fileName();
    }

    closeAuthenticationStore();

    return ret;
}

/*!
 * \brief NetAuthenticationDataList::encryptPassord() simple crypt function hide the user password
 *
 * \param p  the ascii password
 * \return
 */
QString NetAuthenticationDataList::encryptPassord(const QString &p)
{
    QString crypted;
    short unicode = 0;

    for (int counter = 0; counter < p.size(); ++counter) {
        unicode = p.at(counter).unicode() - CHAR_CRYPT_OFFSET + counter;
        crypted.append( QChar(unicode) );
    }

    return crypted.toLocal8Bit().toHex();
}


QString NetAuthenticationDataList::decryptPassword(const QString &p)
{
    QString crypted( QByteArray::fromHex(p.toLocal8Bit()) );
    QString decrypted;
    short unicode = 0;

    for (int counter = 0; counter < crypted.size(); ++counter) {
        unicode = crypted.at(counter).unicode() + CHAR_CRYPT_OFFSET - counter;
        decrypted.append( QChar(unicode) );
    }

    return decrypted;
}

void NetAuthenticationDataList::openAuthenticationStore()
{
    if (m_savedAuths == 0) {
        QString settingsLocation =
            QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()
            + QLatin1Char('/') + QCoreApplication::applicationName()
            + QLatin1Char('/') + QLatin1String("authentication.conf");
        m_savedAuths = new QSettings(settingsLocation, QSettings::IniFormat);

#if DEBUG_MESSAGES
        qDebug() << Q_FUNC_INFO << "auth file:" << m_savedAuths->fileName();
#endif
    }
}

void NetAuthenticationDataList::closeAuthenticationStore()
{
    if (m_savedAuths != 0) {
        delete m_savedAuths;
        m_savedAuths = 0;
    }
}

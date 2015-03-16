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
 * File: locationsfactory.cpp
 * Date: 05/03/2014
 */

#include "diriteminfo.h"
#include "locationsfactory.h"
#include "location.h"
#include "locationurl.h"
#include "disklocation.h"
#include "trashlocation.h"
#include "trashiteminfo.h"
#include "smblocation.h"
#include "cleanurl.h"
#include "netauthenticationdata.h"

#include <QDir>
#include <QDebug>


/*!
 * \brief LocationsFactory::LocationsFactory()
 * \param parent
 *
 * Locations emit needsAuthentication() signal, the connection
 * with LocationsFactory is Direct,  but the connection between
 * the Location and the \ref DirModel is Queued
 * \sa Location::notifyItemNeedsAuthentication()
 */
LocationsFactory::LocationsFactory(QObject *parent)
 : QObject(parent)
 , m_curLoc(0)
 , m_lastValidFileInfo(0)
 , m_authDataStore(NetAuthenticationDataList::getInstance(this))
 , m_lastUrlNeedsAuthentication(false)
{
   m_locations.append(new DiskLocation(LocalDisk));
   m_locations.append(new TrashLocation(TrashDisk));
   SmbLocation * smblocation = new SmbLocation(NetSambaShare);
   m_locations.append(smblocation);

   // Qt::DirectConnection is used here
   // it allows lastUrlNeedsAuthencation() to have the right flag
   connect(smblocation, SIGNAL(needsAuthentication(QString,QString)),
           this,        SLOT(onUrlNeedsAuthentication(QString,QString)),
                        Qt::DirectConnection);
}

LocationsFactory::~LocationsFactory()
{
    ::qDeleteAll(m_locations);
    m_locations.clear();
    if (m_lastValidFileInfo)
    {
        delete m_lastValidFileInfo;
    }
    NetAuthenticationDataList::releaseInstance(this);
}


/*!
 * \brief LocationsFactory::parse() identifies what main location that path/url refers to
 * \param path it is supposed to be always a full path like: file:///myDir /myDir trash:/// trash:///myDir
 * \return
 */

Location * LocationsFactory::parse(const QString& uPath)
{
    int index = -1;
    int type  = -1;
    Location * location = 0;   
    if ( (index = uPath.indexOf(QChar(':'))) != -1 )
    {
#if defined(Q_OS_WIN)
#else
#if defined(Q_OS_UNIX)
        if (uPath.startsWith(LocationUrl::TrashRootURL.midRef(0,6)))
        {
            type = TrashDisk;
            m_tmpPath  = LocationUrl::TrashRootURL + DirItemInfo::removeExtraSlashes(uPath, index+1);
        }
        else
#endif //Q_OS_UNIX
#endif
        if (uPath.startsWith(LocationUrl::DiskRootURL.midRef(0,5)))
        {
            type = LocalDisk;
            m_tmpPath  = QDir::rootPath() + DirItemInfo::removeExtraSlashes(uPath, index+1);
        }
        else
        if ( uPath.startsWith(LocationUrl::SmbURL.midRef(0,4)) ||
             uPath.startsWith(LocationUrl::CifsURL.midRef(0,5))
           )
        {
                type = NetSambaShare;
                m_tmpPath  = LocationUrl::SmbURL + DirItemInfo::removeExtraSlashes(uPath, index+1);
        }
    }
    else
    {
        m_tmpPath = DirItemInfo::removeExtraSlashes(uPath, -1);
        type    = LocalDisk;
        if (!m_tmpPath.startsWith(QDir::rootPath()) && m_curLoc)
        {
            //it can be any, check current location
            type = m_curLoc->type();
        }
    }
    if (!m_tmpPath.isEmpty() && type != -1)
    {
        location = m_locations.at(type);       
    }
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "input path:" << uPath  << "location result:" << location;
#endif
    return location;
}


Location * LocationsFactory::setNewPath(const QString& uPath, const QString& authUser, const QString& passwd, bool savePassword)
{
    storeValidFileInfo(0);
    CleanUrl url(uPath);
    m_lastUrlNeedsAuthentication = false;
    NetAuthenticationData authData(authUser, passwd);
    if (authData.isEmpty() && url.hasAuthenticationData())
    {
        authData.user      = url.user();
        authData.password  = url.password();
    }
    Location *location = parse(url.cleanUrl());
    if (location)
    {
        DirItemInfo *item = validateCurrentUrl(location,authData);
        if (item)
        {
            //now if there is Authentication Data
            //at this point item is ready and authentication if necessary worked
            if (item && !authData.isEmpty())
            {
                m_authDataStore->store(item->authenticationPath(),
                                       authData.user,
                                       authData.password,
                                       savePassword);
            }
            //isContentReadable() must already carry execution permission
            if (item->isValid() && item->isBrowsable() && item->isContentReadable())
            {
                location->setInfoItem(item);
                if (location != m_curLoc)
                {
                    if (m_curLoc)
                    {
                        m_curLoc->stopWorking();
                    }
                    emit locationChanged(m_curLoc, location);
                    location->startWorking();
                    m_curLoc = location;
                }
            }
            else
            {
                storeValidFileInfo(item);
                location = 0;
            }
        }
        else
        {   // not valid
            location = 0;
        }
    }
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "input path:" << uPath  << "location result:" << location;
#endif
    return location;
}


void LocationsFactory::storeValidFileInfo(DirItemInfo *item)
{
    if (m_lastValidFileInfo)
    {
        delete m_lastValidFileInfo;
    }
    m_lastValidFileInfo = item;
}


void LocationsFactory::onUrlNeedsAuthentication(QString, QString)
{
    m_lastUrlNeedsAuthentication = true;
}


bool LocationsFactory::lastUrlNeedsAuthentication() const
{
    return m_lastUrlNeedsAuthentication;
}


DirItemInfo * LocationsFactory::validateCurrentUrl(Location *location, const NetAuthenticationData &authData)
{
    //when there is authentication data, set the authentication before validating an item
    if (!authData.isEmpty())
    {
        location->setAuthentication(authData.user, authData.password);
    }

    DirItemInfo *item = location->validateUrlPath(m_tmpPath);

    //for remote loacations, authentication might have failed
    //if so try to use a stored authentication data and autenticate again
    if (   item && item->needsAuthentication()
        && location->useAuthenticationDataIfExists(*item))
    {
        delete item;
        item = location->validateUrlPath(m_tmpPath);
    }
    //if failed it is necessary to ask the user to provide user/password
    if ( item && item->needsAuthentication() )
    {
        location->notifyItemNeedsAuthentication(item);
        delete item;
        item = 0;
    }
    if (item && !item->isContentReadable())
    {
        delete item;
        item = 0;
    }
    return item;
}

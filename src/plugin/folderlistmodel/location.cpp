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
 * File: location.cpp
 * Date: 08/03/2014
 */
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
 * File: locations.cpp
 * Date: 04/03/2014
 */

#include "location.h"
#include "ioworkerthread.h"
#include "netauthenticationdata.h"

#include <QDebug>

Q_GLOBAL_STATIC(IOWorkerThread, ioWorkerThread)


Location::Location(int type, QObject *parent)
    : QObject(parent)
    , m_info(0)
    , m_type(type)
    , m_usingExternalWatcher(false)
{

}

Location::~Location()
{
    if (m_info) {
        delete m_info;
        m_info = 0;
    }
}


bool Location::isRoot() const
{
    return m_info ? m_info->isRoot() : false;
}


bool Location::isWritable() const
{
    return m_info ? m_info->isWritable() : false;
}


bool Location::isReadable() const
{
    return m_info ? m_info->isContentReadable() : false;
}

void Location::setInfoItem(const DirItemInfo &itemInfo)
{
    setInfoItem (new DirItemInfo(itemInfo));
}

void Location::setInfoItem(DirItemInfo *itemInfo)
{
    if (m_info) {
        delete m_info;
    }
    m_info = itemInfo;
}


QString Location::urlPath() const
{
    return m_info ? m_info->urlPath() : QString();
}


void Location::startWorking()
{

}

void Location::stopWorking()
{

}


IOWorkerThread *Location::workerThread() const
{
    return ioWorkerThread();
}


//providing an empty method
void Location::fetchExternalChanges(const QString &path,
                                    const DirItemInfoList &list,
                                    QDir::Filters dirFilter)
{
    Q_UNUSED(path);
    Q_UNUSED(list);
    Q_UNUSED(dirFilter);
}

//======================================================================================================
/*!
 * \brief Location::setUsingExternalWatcher() Default implementation sets nothing
 *
 *  It considers that there is no external Watcher
 * \param use
 */
void Location::setUsingExternalWatcher(bool use)
{
    Q_UNUSED(use)
    m_usingExternalWatcher = false;
}


/*!
 * \brief Location::setAuthentication()
 *
 * Default implementation does nothing as local disk does not need it
 *
 * Network Locations need to reimplement this
 *
 * \param user
 * \param password
 */
void Location::setAuthentication(const QString &user,
                                 const QString &password)

{
    Q_UNUSED(user);
    Q_UNUSED(password);
}

/*!
 * \brief Location::currentAuthenticationUser()
 *
 * Default implementation returns current user
 *
 * \return
 */
QString Location::currentAuthenticationUser()
{
    return NetAuthenticationData::currentUser();
}

/*!
 * \brief Location::currentAuthenticationPassword()
 *
 * Default implementation returns empty string
 *
 * \return
 */
QString  Location::currentAuthenticationPassword()
{
    return QString();
}

/*!
 * \brief Location::notifyItemNeedsAuthentication()
 * \param item
 *
 * \note
 *    The connection between Location objects and the \class DirModel is Qt::QueuedConnection
 *    It allows a UI to continuosly show dialogs asking the user to provide User and Password
 *     to authenticate the current URL
 */
void Location::notifyItemNeedsAuthentication(const DirItemInfo *item)
{
    if (item == 0) {
        item = m_info;
    }
    if (item != 0) {
        emit needsAuthentication(currentAuthenticationUser(), item->urlPath());
    }
}



bool Location::useAuthenticationDataIfExists(const DirItemInfo &item)
{
    NetAuthenticationDataList *authData = NetAuthenticationDataList::getInstance(this);
    const NetAuthenticationData *auth = authData->get(item.authenticationPath());
    bool ret = false;
    if (auth && !(     auth->user      == currentAuthenticationUser()
                       &&  auth->password  == currentAuthenticationPassword()
                 )
       ) {
        setAuthentication(auth->user, auth->password);
        ret =  true;
    }
    NetAuthenticationDataList::releaseInstance(this);
    return ret;
}



void Location::refreshInfo()
{
    if (m_info) {
        DirItemInfo *item = newItemInfo(m_info->absoluteFilePath());
        delete m_info;
        m_info = item;
    }
}


bool Location::becomeParent()
{
    bool ret = false;
    if (m_info && !m_info->isRoot()) {
        DirItemInfo *other = newItemInfo(m_info->absolutePath());
        if (other->isValid()) {
            delete m_info;
            m_info = other;
            ret = true;
        } else {
            delete other;
        }
    }
    return ret;
}


DirItemInfo *Location::validateUrlPath(const QString &uPath)
{
    QString myPath(uPath);
    DirItemInfo *item = newItemInfo(myPath);
    if (item->isRelative() && m_info) {
        item->setFile(m_info->urlPath(), uPath);
        myPath  =  item->urlPath();
    }

#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "path:" << myPath << "needsAuthentication:" <<
             item->needsAuthentication();
#endif

    // the isContentReadable() is not checked here
    // because it will be false when authentication is required
    if (!item->isValid() || !item->exists()) {
        delete item;
        item = 0;
    }
    return item;
}


void Location::fetchItems(QDir::Filters dirFilter, bool recursive)
{
    //it should never happen here
    if (m_info->needsAuthentication()) {
        emit needsAuthentication(currentAuthenticationUser(), m_info->absoluteFilePath());
    } else {
        DirListWorker *dlw  = newListWorker(m_info->absoluteFilePath(), dirFilter, recursive);
        connect(dlw,  SIGNAL(itemsAdded(DirItemInfoList)),
                this, SIGNAL(itemsAdded(DirItemInfoList)));
        connect(dlw,  SIGNAL(workerFinished()),
                this,     SIGNAL(itemsFetched()));
        workerThread()->addRequest(dlw);
    }
}

/*
 *   Each Location should have its implementation if it is possible
 */
bool Location::isThereDiskSpace(const QString &pathname, qint64 requiredSize)
{
    Q_UNUSED(pathname);
    Q_UNUSED(requiredSize);
    return true;
}


/*!
 * \brief Location::currentInfo()
 * \return  the updated information about the current path
 */
const DirItemInfo *Location::currentInfo()
{
    if (m_info == 0) {
        m_info = new DirItemInfo();
    }
    refreshInfo(); //update information
    return m_info;
}

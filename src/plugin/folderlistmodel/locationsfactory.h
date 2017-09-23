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
 * File: locationsfactory.h
 * Date: 05/03/2014
 */

#ifndef LOCATIONSFACTORY_H
#define LOCATIONSFACTORY_H

#include "location.h"

#include <QObject>
#include <QList>

class DirItemInfo;
class NetAuthenticationDataList;
class NetAuthenticationData;

/*!
 * \brief The LocationsFactory class represents the set of main
 *        URL locations the File Manager supports.
 *
 *  It is basically devided into main groups:
 *   \li Disk: \ref LocalDisk and \ref TrashDisk
 *   \li Net:  \ref NetSambaShare and NetFishShare
 *
 * smb://   browses workgroup
 *
 * Location parser: \ref parser()
 * \li   \\workkgroup                    becomes smb://workgroup
 * \li   \\                              becomes smb://
 * \li   trash:/  and  trash://          becomes trash:///
 * \li   fish:/   and  fish://           becomes fish:///
 * \li   file:/ , file://  and file:///  becomes /
 *
 * \note Due to current File Manager UI typing method both: "file:" and "trash:" are supported
 */
class LocationsFactory : public QObject
{
    Q_OBJECT
public:
    explicit LocationsFactory(QObject *parent = 0);
    ~LocationsFactory();

    inline  Location *getLocation(int index) const
    {
        return m_locations.at(index);
    }
    inline  Location *getDiskLocation()  const
    {
        return getLocation(Location::LocalDisk);
    }
    inline  Location *getTrashLocation() const
    {
        return getLocation(Location::TrashDisk);
    }

    /*!
     * \brief parse()  Just parses (does not set/change the current location) according to \a urlPath
     * \param urlPath  urlPath the url like: file:///Item trash:///item /item, it MUST point to a valid Directory
     * \return   The location which supports the \a  urlPath
     */
    Location *parse(const QString &urlPath);

    /*!
     * \brief setNewPath() Sets a new path, it can be in the current location or on another location
     *
     *   When the location changes, the signal \ref locationChanged() is fired.
     *
     * \param urlPath the url like: file:///Item trash:///item /item, it MUST point to a valid Directory
     * \param user     an user when the URL requires authentication [optional]
     * \param password when the URL requires authentication [optional]
     *
     * \return the location that supports the urlPath or NULL when \a urlPath is NOT a valid url or it is not a valid Directory
     *
     *  \sa \ref parse() \ref location()
     */
    Location *setNewPath(const QString &urlPath,
                         const QString &user = QString(),
                         const QString &password = QString(),
                         bool savePassword = false);

    /*!
     * \brief location()
     * \return The current location
     */
    Location *currentLocation() const
    {
        return m_curLoc;
    }

    /*!
     * \brief availableLocations()
     * \return
     */
    const QList<Location *> &
    availableLocations() const
    {
        return m_locations;
    }

    /*!
     * \brief lastValidFileInfo()
     *
     * When calling setNewPath(file_path) using a path to a File instead of a Directory
     * the setNewPath() is not able to set a new path (current location or other), however it uses
     * Location::validateUrlPath() which validates the path for files also, then this valid DirItemInfo object
     * is saved using \ref  storeValidFileInfo() for further use.
     *
     * \return The last valid DirItemInfo parsed which is not a Directory
     */
    const DirItemInfo *lastValidFileInfo() const
    {
        return m_lastValidFileInfo;
    }

    /*!
     * \brief lastUrlNeedsAuthentication()
     * \return true when last URL used in setNewPath() needs authentication
     *
     * It can be used to show a dialog to the user asking for user/password
     * instead of showing a message saying that url does not exist
     */
    bool        lastUrlNeedsAuthentication() const;

private:
    /*!
     * \brief storeValidFileInfo() saves an item created by \ref setNewPath() when
     *  the item is not Browsable.
     *
     *  It happens for example when the \a urlPath entered
     *  in \ref setNewPath() is a file (not a DIR nor other Browsable item)
     *
     * \param item
     */
    void        storeValidFileInfo(DirItemInfo *item);

    /*!
     * \brief validateCurrentUrl() it attempts to validate the current URL being parsed
     *
     * The validation includes authentication, if a Authentication Data is already avaialable
     * it is set to the location being parsed.
     * When authentication fails the signal Location::needsAuthentication() is emitted
     *
     * \param location current Location for the current urlPath entered in \ref setNewPath()
     *
     * \return new Item validated (authenticated when remote authentication is required), otherwise NULL
     */
    DirItemInfo *validateCurrentUrl(Location *location, const NetAuthenticationData &);

    /*!
     * \brief addLocation() just appends the location in the list \ref m_locations and connect signals
     * \param location
     */
    void        addLocation(Location *location);

signals:
    void        locationChanged(const Location *old, const Location *current);

private slots:
    void        onUrlNeedsAuthentication(QString, QString);

private:
    Location           *m_curLoc;
    QList<Location *>    m_locations;
    QString             m_tmpPath;
    DirItemInfo        *m_lastValidFileInfo;
    NetAuthenticationDataList *m_authDataStore;
    bool                m_lastUrlNeedsAuthentication;

#if defined(REGRESSION_TEST_FOLDERLISTMODEL)
    friend class TestDirModel;
#endif

};

#endif // LOCATIONSFACTORY_H

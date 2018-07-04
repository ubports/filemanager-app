/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author : David Planella <david.planella@ubuntu.com>
 *          Arto Jalkanen <ajalkane@gmail.com>
 *          Carlos Mazieri <carlos.mazieri@gmail.com>
 */

#include "placesmodel.h"
#include <QDebug>
#include <QDir>
#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

namespace {
const QString userSavedLocationsName("userSavedLocations");
const QString userRemovedLocationsName("userRemovedLocations");
}

PlacesModel::PlacesModel(QObject *parent) :
    QAbstractListModel(parent)
    , m_going_to_rescanMtab(false)
{
    m_userMountLocation = "/media/" + qgetenv("USER");

    // For example /run/user/1000
    m_runtimeLocations = QStandardPaths::standardLocations(QStandardPaths::RuntimeLocation);

    // Set the storage location to a path that works well
    // with app isolation
    QString settingsLocation =
        QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()
        + "/" + QCoreApplication::applicationName() + "/" + "places.conf";
    m_settings = new QSettings(settingsLocation, QSettings::IniFormat, this);

    m_userSavedLocations   = m_settings->value(userSavedLocationsName).toStringList();
    m_userRemovedLocations = m_settings->value(userRemovedLocationsName).toStringList();

    //remove old key "storedLocations" which is no longer used
    QLatin1String oldStoredLocations("storedLocations");
    if (m_settings->contains(oldStoredLocations)) {
        m_settings->remove(oldStoredLocations);
    }

    // Prepopulate the model with the user locations
    // for the first time it's used
    addDefaultLocation(locationHome());
    addDefaultLocation(locationDocuments());
    addDefaultLocation(locationDownloads());
    addDefaultLocation(locationMusic());
    addDefaultLocation(locationPictures());
    addDefaultLocation(locationVideos());

    //Network locations
    addDefaultLocation(locationSamba());

    //mounted locations
    addDefaultLocation("/");
    initNewUserMountsWatcher();
    rescanMtab();

    //other user saved locations
    foreach (const QString &userLocation, m_userSavedLocations) {
        addLocationNotRemovedWithoutStoring(userLocation);
    }

    m_settings->sync();

    foreach (const QString &location, m_locations) {
        qDebug() << "Location: " << location;
    }
}

PlacesModel::~PlacesModel()
{

}

void PlacesModel::initNewUserMountsWatcher()
{
    m_newUserMountsWatcher = new QFileSystemWatcher(this);

    connect(m_newUserMountsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(mtabChanged(QString)));
    connect(m_newUserMountsWatcher, SIGNAL(directoryChanged(QString)), this,
            SLOT(mtabChanged(QString)));

    m_newUserMountsWatcher->addPath(m_mtabParser.path());

    /*
     it looks like QFileSystemWatcher does not work for /etc/mtab sometimes, lets use /media/<user> as well
     See:
        https://forum.qt.io/topic/8566/qfilesystemwatcher-not-working-with-etc-mtab
        https://bugs.launchpad.net/ubuntu-filemanager-app/+bug/1444367
    */
    m_newUserMountsWatcher->addPath(m_userMountLocation);

    qDebug() << Q_FUNC_INFO << "Start watching mtab file for new mounts, using:"
             << m_newUserMountsWatcher->files() << "and" << m_newUserMountsWatcher->directories();
}

void PlacesModel::mtabChanged(const QString &path)
{
    qDebug() << Q_FUNC_INFO << "file changed in " << path;
    if (!m_going_to_rescanMtab) {
        m_going_to_rescanMtab = true;
        QTimer::singleShot(100, this, SLOT(rescanMtab()));
    }
    // Since old mtab file is replaced with new contents, must readd filesystem watcher
    m_newUserMountsWatcher->removePath(path);
    m_newUserMountsWatcher->addPath(path);
}

void PlacesModel::rescanMtab()
{
    m_going_to_rescanMtab = false;
    const QString &path = m_mtabParser.path();
    qDebug() << Q_FUNC_INFO << "rescanning mtab" << path;

    QList<QMtabEntry> entries = m_mtabParser.parseEntries();

    QSet<QString> userMounts;

    foreach (QMtabEntry e, entries) {
        qDebug() << Q_FUNC_INFO << "Considering" << "fsName:" <<  e.fsName << "dir:" << e.dir << "type:" <<
                 e.type;
        if (isMtabEntryUserMount(e)) {
            qDebug() << Q_FUNC_INFO << "Adding as userMount directory dir" << e.dir;
            userMounts << e.dir;
        }
    }

    QSet<QString> addedMounts = QSet<QString>(userMounts).subtract(m_userMounts);
    QSet<QString> removedMounts = QSet<QString>(m_userMounts).subtract(userMounts);

    m_userMounts = userMounts;

    foreach (QString addedMount, addedMounts) {
        qDebug() << Q_FUNC_INFO << "user mount added: " << addedMount;
        addLocationNotRemovedWithoutStoring(addedMount);
        emit userMountAdded(addedMount);
    }

    foreach (QString removedMount, removedMounts) {
        qDebug() << Q_FUNC_INFO << "user mount removed: " << removedMount;
        int index = m_locations.indexOf(removedMount);
        if (index > -1) {
            removeItemWithoutStoring(index);
        }
        emit userMountRemoved(removedMount);
    }
}

bool PlacesModel::isMtabEntryUserMount(const QMtabEntry &e) const
{
    if (e.fsName == "none") {
        qDebug() << Q_FUNC_INFO << "Ignoring mounts with filesystem name 'none'";
        return false;
    }
    if (isSubDirectory(m_userMountLocation, e.dir)) {
        qDebug() << Q_FUNC_INFO << "Is user mount location";
        return true;
    }
    foreach (const QString &runtimeLocation, m_runtimeLocations) {
        if (isSubDirectory(runtimeLocation, e.dir)) {
            qDebug() << Q_FUNC_INFO << "Is user mount location";
            return true;
        }
    }

    return false;
}

bool PlacesModel::isSubDirectory(const QString &dir, const QString &path) const
{
    QFileInfo dirFi = QFileInfo(dir);
    QFileInfo pathFi = QFileInfo(path);

    QString absDir = dirFi.absolutePath();
    QString absPath = pathFi.absolutePath();

    return absPath.startsWith(QString(absDir + "/"));
}

QString PlacesModel::standardLocation(QStandardPaths::StandardLocation location) const
{
    QStringList locations = QStandardPaths::standardLocations(location);
    QString standardLocation = "";

    foreach (const QString &location, locations) {
        // We always return the first location or an empty string
        // The frontend should check out that it exists
        if (QDir(location).exists()) {
            standardLocation = location;
            break;
        }
    }

    return standardLocation;
}

QString PlacesModel::locationHome() const
{
    return standardLocation(QStandardPaths::HomeLocation);
}

QString PlacesModel::locationDocuments() const
{
    return standardLocation(QStandardPaths::DocumentsLocation);
}

QString PlacesModel::locationDownloads() const
{
    return standardLocation(QStandardPaths::DownloadLocation);
}

QString PlacesModel::locationMusic() const
{
    return standardLocation(QStandardPaths::MusicLocation);
}

QString PlacesModel::locationPictures() const
{
    return standardLocation(QStandardPaths::PicturesLocation);
}

QString PlacesModel::locationVideos() const
{
    return standardLocation(QStandardPaths::MoviesLocation);
}

QString PlacesModel::locationSamba() const
{
    return QLatin1Literal("smb://");
}

QString PlacesModel::userMountLocation() const
{
    return m_userMountLocation;
}

int PlacesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_locations.count();
}

QVariant PlacesModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    return m_locations.at(index.row());
}

QHash<int, QByteArray> PlacesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::UserRole, "path");

    return roles;
}

void PlacesModel::removeItem(int indexToRemove)
{
    if (indexToRemove >= 0 && indexToRemove < m_locations.count()) {
        bool sync_settings = false;
        const QString &location = m_locations.at(indexToRemove);
        //check if the index belongs to a  user saved location
        int index_user_location = m_userSavedLocations.indexOf(location);

        if (index_user_location > -1) {
            // Remove the User saved location permanently
            m_userSavedLocations.removeAt(index_user_location);
            m_settings->setValue(userSavedLocationsName, m_userSavedLocations);
            sync_settings = true;
        }

        //save it as removed location, even a default location can be removed
        if (!m_userRemovedLocations.contains(location)) {
            m_userRemovedLocations.append(location);
            m_settings->setValue(userRemovedLocationsName, m_userRemovedLocations);
            sync_settings = true;
        }

        removeItemWithoutStoring(indexToRemove);

        if (sync_settings) {
            m_settings->sync();
        }
    }
}

void PlacesModel::removeItemWithoutStoring(int indexToRemove)
{

    // Tell Qt that we're going to be changing the model
    // There's no tree-parent, first new item will be at
    // indexToRemove, and the last one too
    beginRemoveRows(QModelIndex(), indexToRemove, indexToRemove);

    // Remove the actual location
    m_locations.removeAt(indexToRemove);

    // Tell Qt we're done with modifying the model so that
    // it can update the UI and everything else to reflect
    // the new state
    endRemoveRows();
}

/*!
 * \brief PlacesModel::addLocation()
 *
 * Adds the location permanently in the settings file.
 *
 * If the location has already been deleted by the user it is first removed from the removed settings \a m_userRemovedLocations.
 *
 * The location is saved in settings file in \a m_userSavedLocations
 *
 * \param location
 */
void PlacesModel::addLocation(const QString &location)
{
    bool sync_settings = false;
    //verify it the user had deleted it before and now is inserting it again
    int indexRemoved = m_userRemovedLocations.indexOf(location);
    if (indexRemoved > -1) {
        m_userRemovedLocations.removeAt(indexRemoved);
        m_settings->setValue(userRemovedLocationsName, m_userRemovedLocations);
        sync_settings = true;
    }
    if (addLocationNotRemovedWithoutStoring(location)) {
        // Store the location permanently if it is not default location
        if (!isDefaultLocation(location) && !m_userSavedLocations.contains(location)) {
            m_userSavedLocations.append(location);
            m_settings->setValue(userSavedLocationsName, m_userSavedLocations);
            sync_settings = true;
        }
    }
    if (sync_settings) {
        m_settings->sync();
    }
}

/*!
 * \brief PlacesModel::addLocationNotRemovedWithoutStoring()
 *
 *  Add that location only if it was not removed before by the user.
 *
 *  When the user removes a location from Places using \ref removeItem(int index) it is stored in settings file.
 *  The user must use \ref addLocation(const QString &location) to add back an already removed location.
 *
 * \param location
 *
 * \return true when the location was added (not existent in \a m_locations nor in \a m_userRemovedLocations),
 *         otherwise false
 */
bool PlacesModel::addLocationNotRemovedWithoutStoring(const QString &location)
{
    // Do not allow for duplicates and look for removed locations from settings
    if (!m_locations.contains(location) && !m_userRemovedLocations.contains(location)) {
        // Tell Qt that we're going to be changing the model
        // There's no tree-parent, first new item will be at
        // m_locations.count(), and the last one too
        beginInsertRows(QModelIndex(), m_locations.count(), m_locations.count());

        // Append the actual location
        m_locations.append(location);

        // Tell Qt we're done with modifying the model so that
        // it can update the UI and everything else to reflect
        // the new state
        endInsertRows();
        return true;
    }
    return false;
}

void PlacesModel::addDefaultLocation(const QString &location)
{
    // a Default location can be removed by the user
    if (addLocationNotRemovedWithoutStoring(location)) {
        m_defaultLocations.append(location);
    }
}

void PlacesModel::removeItem(const QString &location)
{
    removeItem(m_locations.indexOf(location));
}

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
 */

#include "placesmodel.h"
#include <QDebug>
#include <QDir>
#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

PlacesModel::PlacesModel(QAbstractListModel *parent) :
    QAbstractListModel(parent)
{
    QStringList defaultLocations;
    // Set the storage location to a path that works well
    // with app isolation
    QString settingsLocation =
            QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first()
            + "/" + QCoreApplication::applicationName() + "/" + "places.conf";
    m_settings = new QSettings(settingsLocation, QSettings::IniFormat, this);

    // Prepopulate the model with the user locations
    // for the first time it's used
    defaultLocations.append(locationHome());
    defaultLocations.append(locationDocuments());
    defaultLocations.append(locationDownloads());
    defaultLocations.append(locationMusic());
    defaultLocations.append(locationPictures());
    defaultLocations.append(locationVideos());

    if (!m_settings->contains("storedLocations")) {
        m_locations.append(defaultLocations);
    } else {
        m_locations = m_settings->value("storedLocations").toStringList();
    }

    foreach (const QString &location, m_locations) {
        qDebug() << "Location: " << location;
    }

    initNewUserMountsWatcher();
    rescanUserMountDirectories();
}

PlacesModel::~PlacesModel() {

}

void
PlacesModel::initNewUserMountsWatcher() {
    m_scanMountDirsTimer = new QTimer(this);
    m_scanMountDirsTimer->setSingleShot(true);
    m_scanMountDirsTimer->setInterval(500); // 0.5 secs

    m_newUserMountsWatcher = new QFileSystemWatcher(this);

    QString user = qgetenv("USER");
    QString watchPath = "/media/" + user;
    qDebug() << Q_FUNC_INFO << "Start watching path for new mounts" << watchPath;

    m_newUserMountsWatcher->addPath(watchPath);

    connect(m_newUserMountsWatcher, &QFileSystemWatcher::directoryChanged, this, &PlacesModel::userMountsChanged);
    connect(m_scanMountDirsTimer, &QTimer::timeout, this, &PlacesModel::rescanUserMountDirectories);
//    connect(m_newUserMountsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(userMountsChanged()));
//    connect(m_scanMountDirsTimer, SIGNAL(timeout()), this, SLOT(rescanUserMountDirectories()));
}

void
PlacesModel::userMountsChanged() {
    qDebug() << Q_FUNC_INFO;
    if (!m_scanMountDirsTimer->isActive()) {
        qDebug() << Q_FUNC_INFO << "starting mount dirs scanner timer";
        m_scanMountDirsTimer->start();
    } else {
        qDebug() << Q_FUNC_INFO << "mount dirs timer scanner already running, not sarting timer";
    }
}

void
PlacesModel::rescanUserMountDirectories() {
    QStringList dirs = m_newUserMountsWatcher->directories();
    foreach (QString dir, dirs) {
        rescanUserMountDirectory(dir);
    }
}

void
PlacesModel::rescanUserMountDirectory(const QString &dirStr) {
    qDebug() << Q_FUNC_INFO << "rescanning" << dirStr;

    QDir userDir(dirStr, QString(), QDir::SortFlags(QDir::Name | QDir::IgnoreCase), QDir::AllDirs | QDir::NoDotAndDotDot);

    QSet<QString> userMounts;

    foreach (QString entry, userDir.entryList()) {
        qDebug() << Q_FUNC_INFO << "entry: " << entry;
        QFileInfo fi(userDir, entry);

        qDebug() << Q_FUNC_INFO << "absoluteFilePath: " << fi.absoluteFilePath();

        userMounts << fi.absoluteFilePath();
    }


    QSet<QString> addedMounts = QSet<QString>(userMounts).subtract(m_userMounts);
    QSet<QString> removedMounts = QSet<QString>(m_userMounts).subtract(userMounts);

    foreach (QString addedMount, addedMounts) {
        qDebug() << Q_FUNC_INFO << "user mount added: " << addedMount;
        addLocationWithoutStoring(addedMount);
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

    m_userMounts = userMounts;
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
    removeItemWithoutStoring(indexToRemove);

    // Remove the location permanently
    m_settings->setValue("storedLocations", m_locations);
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

void PlacesModel::addLocation(const QString &location)
{
    if (addLocationWithoutStoring(location)) {
        // Store the location permanently
        m_settings->setValue("storedLocations", m_locations);
    }
}

bool PlacesModel::addLocationWithoutStoring(const QString &location)
{
    // Do not allow for duplicates
    if (!m_locations.contains(location)) {
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

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

#include "xdguserdir.h"
#include <QDebug>
#include <QDir>

XdgUserDir::XdgUserDir(QAbstractListModel *parent) :
    QAbstractListModel(parent)
{

}

XdgUserDir::~XdgUserDir() {

}

QString XdgUserDir::standardLocation(QStandardPaths::StandardLocation location) const
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

QString XdgUserDir::locationHome() const
{
    return standardLocation(QStandardPaths::HomeLocation);
}

QString XdgUserDir::locationDocuments() const
{
    return standardLocation(QStandardPaths::DocumentsLocation);
}

QString XdgUserDir::locationDownloads() const
{
    return standardLocation(QStandardPaths::DownloadLocation);
}

QString XdgUserDir::locationMusic() const
{
    return standardLocation(QStandardPaths::MusicLocation);
}

QString XdgUserDir::locationPictures() const
{
    return standardLocation(QStandardPaths::PicturesLocation);
}

QString XdgUserDir::locationVideos() const
{
    return standardLocation(QStandardPaths::MoviesLocation);
}

int XdgUserDir::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 6;
}

QVariant XdgUserDir::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    switch(index.row()) {
        case 0: return locationHome();
        case 1: return locationDocuments();
        case 2: return locationDownloads();
        case 3: return locationMusic();
        case 4: return locationPictures();
        case 5: return locationVideos();
    }

    return QVariant();
}

QHash<int, QByteArray> XdgUserDir::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::UserRole, "path");

    return roles;
 }

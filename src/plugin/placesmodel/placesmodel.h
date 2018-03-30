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
 *          Arto Jalkanen <arto.jalkanen@gmail.com>
 */

#ifndef PLACESMODEL_H
#define PLACESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStandardPaths>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QSet>

#include "qmtabparser.h"

// TODO: Add new roles for user mounted locations, user's bookmarks
// TODO: Add new roles for location name and icon

class PlacesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString locationHome READ locationHome CONSTANT)
    Q_PROPERTY(QString locationDocuments READ locationDocuments CONSTANT)
    Q_PROPERTY(QString locationDownloads READ locationDownloads CONSTANT)
    Q_PROPERTY(QString locationMusic READ locationMusic CONSTANT)
    Q_PROPERTY(QString locationPictures READ locationPictures CONSTANT)
    Q_PROPERTY(QString locationVideos READ locationVideos CONSTANT)
    Q_PROPERTY(QString locationSamba READ locationSamba CONSTANT)
    Q_PROPERTY(QString userMountLocation READ userMountLocation CONSTANT)

public:
    explicit PlacesModel(QObject *parent = 0);
    ~PlacesModel();

    QString locationHome() const;
    QString locationDocuments() const;
    QString locationDownloads() const;
    QString locationMusic() const;
    QString locationPictures() const;
    QString locationVideos() const;
    QString locationSamba() const;
    QString userMountLocation() const;

    int rowCount(const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void userMountAdded(const QString &path);
    void userMountRemoved(const QString &paht);

public slots:
    void addLocation(const QString &location);
    void removeItem(int indexToRemove);

    inline bool isUserMountDirectory(const QString &location)
    {
        return m_userMounts.contains(location);
    }

    bool isDefaultLocation(const QString &location) const
    {
        return m_defaultLocations.contains(location);
    }

    inline int indexOfLocation(const QString &location) const
    {
        return m_locations.indexOf(location);
    }

private slots:
    void mtabChanged(const QString &path);
    void rescanMtab();

private:
    void initNewUserMountsWatcher();
    bool addLocationNotRemovedWithoutStoring(const QString &location);
    // Returns true if location was not known before, and false if it was known
    void removeItemWithoutStoring(int itemToRemove);
    //just add into m_locations, does not emit any signal
    void addDefaultLocation(const QString &location);
    void removeItem(const QString &location);

    QMtabParser m_mtabParser;
    QStringList m_runtimeLocations;
    QString m_userMountLocation;
    bool isMtabEntryUserMount(const QMtabEntry &entry) const;
    bool isSubDirectory(const QString &dir, const QString &path) const;
    QString standardLocation(QStandardPaths::StandardLocation location) const;
    QStringList
    m_locations;  //<! m_locations = m_defaultLocations + m_userSavedLocations - m_userRemovedLocations
    QStringList    m_defaultLocations;
    QStringList    m_userSavedLocations;
    QStringList    m_userRemovedLocations;
    QSettings *m_settings;
    QFileSystemWatcher *m_newUserMountsWatcher;
    QSet<QString> m_userMounts;
    bool          m_going_to_rescanMtab;
#if defined(REGRESSION_TEST_PLACES_MODEL)
    friend class PlacesmodelTest;
#endif
};

#endif // PLACESMODEL_H




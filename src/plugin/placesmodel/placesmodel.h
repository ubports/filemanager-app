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

#ifndef PLACESMODEL_H
#define PLACESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStandardPaths>
#include <QSettings>

class PlacesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString locationHome READ locationHome CONSTANT)
    Q_PROPERTY(QString locationDocuments READ locationDocuments CONSTANT)
    Q_PROPERTY(QString locationDownloads READ locationDownloads CONSTANT)
    Q_PROPERTY(QString locationMusic READ locationMusic CONSTANT)
    Q_PROPERTY(QString locationPictures READ locationPictures CONSTANT)
    Q_PROPERTY(QString locationVideos READ locationVideos CONSTANT)

public:
    explicit PlacesModel(QAbstractListModel *parent = 0);
    ~PlacesModel();
    QString locationHome() const;
    QString locationDocuments() const;
    QString locationDownloads() const;
    QString locationMusic() const;
    QString locationPictures() const;
    QString locationVideos() const;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void addLocation(const QString &location);
    void removeItem(int indexToRemove);

private:
    QString standardLocation(QStandardPaths::StandardLocation location) const;
    QStringList m_locations;
    QSettings *m_settings;
};

#endif // PLACESMODEL_H




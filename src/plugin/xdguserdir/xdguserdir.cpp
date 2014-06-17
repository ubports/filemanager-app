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
    return XdgUserDir::standardLocation(QStandardPaths::HomeLocation);
}

QString XdgUserDir::locationDocuments() const
{
    return XdgUserDir::standardLocation(QStandardPaths::DocumentsLocation);
}

QString XdgUserDir::locationDownloads() const
{
    return XdgUserDir::standardLocation(QStandardPaths::DownloadLocation);
}

QString XdgUserDir::locationMusic() const
{
    return XdgUserDir::standardLocation(QStandardPaths::MusicLocation);
}

QString XdgUserDir::locationPictures() const
{
    return XdgUserDir::standardLocation(QStandardPaths::PicturesLocation);
}

QString XdgUserDir::locationVideos() const
{
    return XdgUserDir::standardLocation(QStandardPaths::MoviesLocation);
}

int rowCount(const QAbstractListModel &parent)
{
    return 6;
}

QVariant data(const QModelIndex &index, int role)
{
    switch(index.row()) {
        case 0: return locationHome();
        case 1: return locationDocuments();
        case 2: return locationDownloads();
        case 3: return locationMusic();
        case 4: return locationPictures();
        case 5: return locationVideos();
    }
}

QHash<int, QByteArray> roleNames() const {
    QHash<int, QByteArray> roles;
    roles.insert("path");

    return roles;
 }

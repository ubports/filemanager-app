/**************************************************************************
 *
 * Copyright 2014 Canonical Ltd()->
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
 * File: diriteminfo.cpp
 * Date: 30/01/2014
 */

#include "diriteminfo.h"
#include "locationurl.h"
#include <sys/types.h>
#include <sys/stat.h>


QMimeDatabase DirItemInfoPrivate::mimeDatabase;


DirItemInfoPrivate::DirItemInfoPrivate() :
    _isValid(false)
    , _isLocal(false)
    , _isRemote(false)
    , _isSelected(false)
    , _isAbsolute(false)
    , _exists(false)
    , _isFile(false)
    , _isDir(false)
    , _isSymLink(false)
    , _isRoot(false)
    , _isReadable(false)
    , _isWritable(false)
    , _isExecutable(false)
    , _isLocalSharedDir(false)
    , _isHost(false)
    , _isWorkGroup(false)
    , _isNetworkShare(false)
    , _needsAuthentication(false)
    , _permissions(0)
    , _size(0)
{

}


DirItemInfoPrivate::DirItemInfoPrivate(const DirItemInfoPrivate &other):
    QSharedData(other)
    , _isValid(other._isValid)
    , _isLocal(other._isLocal)
    , _isRemote(other._isRemote)
    , _isSelected(other._isSelected)
    , _isAbsolute(other._isAbsolute)
    , _exists(other._exists)
    , _isFile(other._isFile)
    , _isDir(other._isDir)
    , _isSymLink(other._isSymLink)
    , _isRoot(other._isRoot)
    , _isReadable(other._isReadable)
    , _isWritable(other._isWritable)
    , _isExecutable(other._isExecutable)
    , _isLocalSharedDir(other._isLocalSharedDir)
    , _isHost(other._isHost)
    , _isWorkGroup(other._isWorkGroup)
    , _isNetworkShare(other._isNetworkShare)
    , _needsAuthentication(other._needsAuthentication)
    , _permissions(other._permissions)
    , _size(other._size)
    , _created(other._created)
    , _lastModified(other._lastModified)
    , _lastRead(other._lastRead)
    , _path(other._path)
    , _fileName(other._fileName)
    , _normalizedPath(other._normalizedPath)
    , _authenticationPath(other._authenticationPath)
{

}


DirItemInfoPrivate::DirItemInfoPrivate(const QFileInfo &fi):
    _isValid(true)
    , _isLocal(true)
    , _isRemote(false)
    , _isSelected(false)
    , _isAbsolute(false)
    , _exists(false)
    , _isFile(false)
    , _isDir(false)
    , _isSymLink(false)
    , _isRoot(false)
    , _isReadable(false)
    , _isWritable(false)
    , _isExecutable(false)
    , _isLocalSharedDir(false)
    , _isHost(false)
    , _isWorkGroup(false)
    , _isNetworkShare(false)
    , _needsAuthentication(false)
    , _permissions(0)
    , _size(0)
{
    setFileInfo(fi);
}

void DirItemInfoPrivate::setFileInfo(const QFileInfo &fi)
{
    if (fi.exists() && fi.isRelative()) {
        QFileInfo abs(fi.absoluteFilePath());
        setFileInfo(abs);
    } else {
        _path           = fi.absolutePath();
        _normalizedPath = _path;
        _fileName       = fi.fileName();
        _isAbsolute     = fi.isAbsolute();
        _exists         = fi.exists();
        _isDir          = fi.isDir();
        _isFile         = fi.isFile();
        _isSymLink      = fi.isSymLink();
        _isRoot         = fi.isRoot();
        _isReadable     = fi.isReadable();
        _isWritable     = fi.isWritable();
        _isExecutable   = fi.isExecutable();
        _permissions    = fi.permissions();
        _size           = fi.size();
        _created        = fi.created();
        _lastRead       = fi.lastRead();
        _lastModified   = fi.lastModified();
    }
}

//================================================================

DirItemInfo::DirItemInfo(): d_ptr(new DirItemInfoPrivate())
{ }


DirItemInfo::~DirItemInfo()
{ }

DirItemInfo::DirItemInfo(const QFileInfo &fi):
    d_ptr(new DirItemInfoPrivate(fi))
{ }

DirItemInfo::DirItemInfo(const QString &filePath):
    d_ptr(  new DirItemInfoPrivate(QFileInfo(filePath)) )
{ }

DirItemInfo::DirItemInfo(const DirItemInfo &other)
{
    d_ptr = other.d_ptr;
}

bool DirItemInfo::isSelected() const
{
    return d_ptr->_isSelected;
}

/*!
 * \brief DirItemInfo::setSelection()
 * \param selected true/false new selection state
 * \return true if a new state was set, false if the selection is already equal to \a selected
 */
bool DirItemInfo::setSelection(bool selected)
{
    bool ret = selected != isSelected();
    d_ptr->_isSelected = selected;

    return ret;
}

bool DirItemInfo::isValid() const
{
    return d_ptr->_isValid;
}

bool DirItemInfo::isLocal() const
{
    return d_ptr->_isLocal;
}

bool DirItemInfo::isRemote() const
{
    return d_ptr->_isRemote;
}

bool DirItemInfo::exists() const
{
    return d_ptr->_exists;
}

QString DirItemInfo::filePath() const
{
    return filePathFrom(d_ptr->_path);
}

QString DirItemInfo::fileName() const
{
    return d_ptr->_fileName;
}

QString DirItemInfo::absoluteFilePath() const
{
    return filePath();
}

QString DirItemInfo::absolutePath() const
{
    return d_ptr->_path;
}

bool  DirItemInfo::isReadable() const
{
    return d_ptr->_isReadable;
}

/*!
 * \brief DirItemInfo::isContentReadable() Expands the isReadable() idea, when it is a folder says if it is possible to get the content
 * \return true if the file/directory can be read, otherwise false.
 */
bool  DirItemInfo::isContentReadable() const
{
    bool readable = d_ptr->_isReadable;

    if (isDir() && !isSymLink()) {
        readable &= isExecutable();
    }

    return readable;
}

bool DirItemInfo::isWritable() const
{
    return d_ptr->_isWritable;
}

bool DirItemInfo::isExecutable() const
{
    return d_ptr->_isExecutable;
}

bool DirItemInfo::isRelative() const
{
    return ! isAbsolute();
}

bool  DirItemInfo::isAbsolute() const
{
    return d_ptr->_isAbsolute;
}

bool  DirItemInfo::isFile() const
{
    return d_ptr->_isFile;
}

bool DirItemInfo::isDir() const
{
    return d_ptr->_isDir;
}

bool DirItemInfo::isSymLink() const
{
    return d_ptr->_isSymLink;
}

bool DirItemInfo::isRoot() const
{
    return d_ptr->_isRoot;
}

QFile::Permissions  DirItemInfo::permissions() const
{
    return d_ptr->_permissions;
}

qint64 DirItemInfo::size() const
{
    return d_ptr->_size;
}

QDateTime DirItemInfo::created() const
{
    return d_ptr->_created;
}

QDateTime DirItemInfo::lastModified() const
{
    return d_ptr->_lastModified;
}

QDateTime DirItemInfo::lastRead() const
{
    return d_ptr->_lastRead;
}

void DirItemInfo::setFile(const QString &dir, const QString &file)
{
    QFileInfo f;
    f.setFile(dir, file);
    d_ptr->setFileInfo(f);
}

void DirItemInfo::setFile(const QString &fullname)
{
    QFileInfo f;
    f.setFile(fullname);
    d_ptr->setFileInfo(f);
}

QFileInfo DirItemInfo::diskFileInfo() const
{
    QFileInfo fi(absoluteFilePath());
    return fi;
}

QString DirItemInfo::path() const
{
    return d_ptr->_path;
}

QMimeType DirItemInfo::mimeType() const
{
    return d_ptr->mimeDatabase.mimeTypeForFile(diskFileInfo());
}

QString DirItemInfo::urlPath() const
{
    return filePathFrom(d_ptr->_normalizedPath);
}

QString DirItemInfo::filePathFrom(const QString &p) const
{
    QString filepath;

    if (!p.isEmpty()) {
        filepath = p;

        if (!p.endsWith(QDir::separator()) && !d_ptr->_fileName.isEmpty()) {
            filepath += QDir::separator();
        }
    }

    filepath += d_ptr->_fileName;

    return filepath;
}

bool DirItemInfo::permission(QFileDevice::Permissions permissions) const
{
    return (d_ptr->_permissions & permissions) == permissions;
}

bool DirItemInfo::isSharedDir() const
{
    return d_ptr->_isLocalSharedDir;
}

bool DirItemInfo::isHost() const
{
    return d_ptr->_isHost;
}

bool DirItemInfo::isWorkGroup() const
{
    return d_ptr->_isWorkGroup;
}

bool DirItemInfo::isShare() const
{
    return d_ptr->_isNetworkShare;
}

/*!
 * \brief DirItemInfo::isBrowsable() considers browsable items that can hold a list of items
 * \return
 */
bool DirItemInfo::isBrowsable() const
{
    return isDir() || isHost() || isShare() || isWorkGroup();
}

bool DirItemInfo::needsAuthentication() const
{
    return d_ptr->_needsAuthentication;
}

QString DirItemInfo::authenticationPath() const
{
    return  d_ptr->_authenticationPath;
}

/*!
 * \brief  DirItemInfo::fillFromStatBuf() This was copied from \ref QFileSystemMetaData::fillFromStatBuf()
 * \param statBuffer
 */
void DirItemInfo::fillFromStatBuf(const struct stat &statBuffer)
{
#define LinkType       0x00010000
#define FileType       0x00020000
#define DirectoryType  0x00040000
#define SequentialType 0x00800000

    //size
    d_ptr->_size = statBuffer.st_size;

    //times
    d_ptr->_lastModified = statBuffer.st_mtime ?
                           QDateTime::fromTime_t(statBuffer.st_mtime) :
                           QDateTime(QDate(), QTime());

    d_ptr->_created = statBuffer.st_ctime ?
                      QDateTime::fromTime_t(statBuffer.st_ctime) :
                      d_ptr->_lastModified;

    d_ptr->_lastRead = statBuffer.st_atime ?
                       QDateTime::fromTime_t(statBuffer.st_atime) :
                       d_ptr->_lastModified;

    /*
     //user, group
     userId_ =  statBuffer.st_uid;
     groupId_ = statBuffer.st_gid;
    */

    /*
     *  When handling filesystems other than local (e.g. any network)
     *  Permissions  are relative to the user being used to access the resource
     *
     *  So it is necessary to qualify the user accessing the resource as
     *   owner / belongs to group or / others
     */
    // quint32 userMatches     = 0;
    QFile::Permissions readPermission  = 0;
    QFile::Permissions writePermission = 0;
    QFile::Permissions execPermission  = 0;


    //owner permissions
    if (statBuffer.st_mode & S_IRUSR) {
        readPermission  |= QFile::ReadOwner |  QFile::ReadUser;
    }

    if (statBuffer.st_mode & S_IWUSR) {
        writePermission |= QFile::WriteOwner | QFile::WriteUser;
    }

    if (statBuffer.st_mode & S_IXUSR) {
        execPermission  |= QFile::ExeOwner |  QFile::ExeUser;
    }

    //group permissions
    if (statBuffer.st_mode & S_IRGRP) {
        readPermission |= QFile::ReadGroup;
    }

    if (statBuffer.st_mode & S_IWGRP) {
        writePermission |= QFile::WriteGroup;
    }

    if (statBuffer.st_mode & S_IXGRP) {
        execPermission |= QFile::ExeGroup;
    }

    //other permissions
    if (statBuffer.st_mode & S_IROTH) {
        readPermission |= QFile::ReadOther;
    }

    if (statBuffer.st_mode & S_IWOTH) {
        writePermission |= QFile::WriteOther;
    }

    if (statBuffer.st_mode & S_IXOTH) {
        execPermission |= QFile::ExeOther;
    }

    /*
     * Permissions are relative to a remote user
     * it was necessary to be the user being accessing the file
     */
    if (readPermission) {
        d_ptr->_isReadable = true;
    }

    if (writePermission) {
        d_ptr->_isWritable = true;
    }

    if (execPermission) {
        d_ptr->_isExecutable = true;
    }

    //set full permissions flag
    d_ptr->_permissions = readPermission | writePermission | execPermission;

    // Type
    if ((statBuffer.st_mode & S_IFMT) == S_IFREG) {
        //  d_ptr->_permissions   |= FileType;
        d_ptr->_isFile = true;

    } else if ((statBuffer.st_mode & S_IFMT) == S_IFDIR) {
        // d_ptr->_permissions |= DirectoryType;
        d_ptr->_isDir = true;

    } else {
        // d_ptr->_permissions |= SequentialType;
    }
}

QString DirItemInfo::removeExtraSlashes(const QString &url, int firstSlashIndex)
{
    QString ret;

    if (firstSlashIndex == -1) {
        firstSlashIndex = url.indexOf(LocationUrl::UrlIndicator);

        if (firstSlashIndex != -1) {
            ++firstSlashIndex;

        } else {
            firstSlashIndex = -1;
        }
    }

    if (firstSlashIndex >= 0) {
        while ( firstSlashIndex < url.length() && url.at(firstSlashIndex) == QDir::separator()) {
            ++firstSlashIndex;
        }

        if (firstSlashIndex < url.length()) {
            ret = url.mid(firstSlashIndex);
        }

    } else {
        ret = url;
        firstSlashIndex = -1;
    }

    if (firstSlashIndex >= 0 && ret.endsWith(QDir::separator())) {
        ret.chop(1);
    }

    //replace any double slashes by just one
    for (int charCounter = ret.size() - 1; charCounter > 0; --charCounter) {
        if (ret.at(charCounter) == QDir::separator() &&
                ret.at(charCounter - 1) == QDir::separator()) {
            ret.remove(charCounter, 1);
        }
    }

    return ret;
}

void DirItemInfo::setAsHost()
{
    d_ptr->_isHost = true;
    d_ptr->_exists = true;
    d_ptr->_isReadable   = true;
    d_ptr->_isExecutable = true;
}

void DirItemInfo::setAsShare()
{
    d_ptr->_isNetworkShare = true;
}

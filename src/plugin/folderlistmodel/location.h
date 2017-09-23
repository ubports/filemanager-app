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
 * File: location.h
 * Date: 08/03/2014
 */

#ifndef LOCATION_H
#define LOCATION_H

#include "diriteminfo.h"
#include "locationitemdiriterator.h"

#include <QObject>
#include <QDirIterator>

class IOWorkerThread;
class DirListWorker;
class LocationItemDirIterator;
class LocationItemFile;
class LocationItemDir;

/*!
 * \brief The Location class represents any location (full path) where there are items to browse: directories, shares, from Disk and from Network.
 *
 *  It is an abstract class the must be inherited for specific Location handling as example: \ref DiskLocation and \ref TrashLocation
 *
 *  The location must be able to:
 *     \li  provide the browsing for the location in \ref fetchItems()
 *     \li  become itself its parent in \ref becomeParent() it will allow easy \ref DirModel::cdUp()
 *     \li  refresh its information in  \ref refreshInfo()
 *     \li  validate its location (creates a valid DirItemInfo object or any descendent) from a url string
 *
 *  The \ref startWorking() is called by \ref LocationsFactory just before this location becomes the current in the File Manager
 *  In the same way the \ref stopWorking() is called by \ref LocationsFactory just before this location no longer be the current in the File Manager
 */
class Location : public QObject
{
    Q_OBJECT
public:

    Q_ENUMS(Locations)
    /*!
     * \brief The Locations enum defines which Locations are supported
     *
     * \note Items also work as indexes for \a m_locations, they must be 0..(n-1)
     */
    enum Locations {
        LocalDisk = 0, //<! any mounted file system
        TrashDisk,     //<! special trash location in the disk
        NetSambaShare  //<! SAMBA or CIFS shares
#if 0
        NetFishShare   //<! FISH protocol over ssh that provides file sharing
#endif
    };

public:
    virtual ~Location();
protected:
    explicit Location( int type, QObject *parent = 0);

    IOWorkerThread *workerThread() const;

signals:
    void     itemsAdded(const DirItemInfoList &files);
    void     itemsFetched();
    void     extWatcherPathChanged(const QString &);
    void     extWatcherItemRemoved(const DirItemInfo &);
    void     extWatcherItemChanged(const DirItemInfo &);
    void     extWatcherItemAdded(const   DirItemInfo &);
    void     extWatcherChangesFetched(int);
    void     needsAuthentication(const QString &user, const QString &urlPath);

public slots:
    virtual void setUsingExternalWatcher(bool use);
    virtual void setAuthentication(const QString &user,
                                   const QString &password);


public: //pure functions
    /*!
     * \brief newItemInfo()  returns a Location suitable DirItemInfo object
     *
     * Every Locations must create its own DirItemInfo object with all the information set
     * \param urlPath  it can also contain User and Password when in the form of an URL
     * \return the object created
     */
    virtual DirItemInfo     *newItemInfo(const QString &urlPath) = 0;

    /*!
     * \brief newListWorker() creates a Location suitable DirListWorker object which will create a new \ref DirItemInfoList for browsing items
     *
     *  The DirListWorker object will be used in \ref fetchItems()
     *
     * \param urlPath  urlPath  it can also contain User and Password when in the form of an URL
     * \param filter
     * \param isRecursive
     * \return the object which will fill a new \ref DirItemInfoList for browsing items
     */
    virtual DirListWorker   *newListWorker(const QString &urlPath, QDir::Filters filter,
                                           const bool isRecursive) = 0;

    /*!
     * \brief newDirIterator() creates a LocationItemDirIterator object which is similar to Qt QDirIterator object
     *
     *         It will be used to create a recursive list of items in copy/cut/paste/remove Actions
     *         It can used in DirListWorker as well
     * \param path
     * \param flags
     * \return
     */
    virtual LocationItemDirIterator *newDirIterator(const QString &path,
                                                    QDir::Filters filters,
                                                    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
                                                    LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor)  = 0;
    /*!
      * \brief newFile() creates a LocationItemFile object which is similar to Qt QFile object
      *
      *     It will be used in copy/paste/remove Actions
      *
      * \param path
      * \return
      */
    virtual LocationItemFile    *newFile(const QString &path) = 0;

    /*!
      * \brief newDir() creates a LocationItemDir object which is simila to Qt QDir object
      *
      * It will be used in copy/paste/remove Actions
      *
      * \param dir
      * \return
      */
    virtual LocationItemDir     *newDir(const QString &dir = QLatin1String(0)) = 0;

    /*!
      * \brief urlBelongsToLocation() Returns a good  url if the \a urlPath is valid URL that belongs to its location
      *
      *  If the URL or Path in \a urlPath is valid and belongs to its location
      *
      * \param urlPath                  The input URL that is going to be parsed
      * \param indexOfColonAndSlash     The index of ":/"
      * \return                         The good URL (parsed with extra slashes removed)
      *                                 or an empty string if \a urlPath does not belong to its location
      *
      *Example regarding samba where both "cifs://" and "smb://" urls are supported
      * \code"
      *    For a urlPath like: "cifs://localhost/share/"
      *    The return will be: "smb://localhost/share"   -> "cifs" changed by "smb" and last slash removed
      *
      *    For a urlPath like: "trash:///"
      *    The return will be: an empty string meaning that this URL is not related to Samba
      *\endcode
      */
    virtual QString         urlBelongsToLocation(const QString &urlPath, int indexOfColonAndSlash) = 0;

public:
    /*!
     * \brief isThereDiskSpace()  Check if the filesystem has enough space to put a file with size \a requiredSize
     *
     *
     * \param pathname  is the full pathname of the new file that is going to be created
     *
     * \param requiredSize  the size required
     *
     *
     * \note  For remote locations if not is possible to get this value this function MUST return true
     *
     *        The default implementation just returns true and let the copy fail if there is enough space
     */
    virtual bool     isThereDiskSpace(const QString &pathname, qint64 requiredSize);

    /*!
     * \brief fetchItems() gets the content of the Location
     *
     * \param dirFilter   current Filter
     * \param recursive   should get the content all sub dirs or not, (hardly ever it is true)
     */
    virtual void        fetchItems(QDir::Filters dirFilter, bool recursive = false);

    /*!
     * \brief refreshInfo() It must refresh the DirItemInfo
     *
     *  It can be used for example after receiving the signal about external disk file system changes
     *  due to the current path permissions might have changed.
     */
    virtual void        refreshInfo();

    /*!
     * \brief becomeParent() The current path location becomes the parent Location
     *
     * When \ref isRoot() returns false  the current path location becomes the parent path location
     *   in order to make it the current.
     * It acts like a cdUp, but without fetching items; then calling \ref fetchItems() may get contents.
     *
     * \note It must take care of deleting \ref m_info when creating a new DirItemInfo/TrashItemInfo etc.
     *
     * \return true if it is possible to do like a cdUp.
     */
    virtual bool        becomeParent();

    /*!
      * \brief validateUrlPath()  Validates the urlPath (file or Directory) and creates a new Obeject from this path
      *
      *  If urlPath is a valid Directory it can be used later to set a new Location.
      *
      * \param urlPath
      * \return a valid pointer to DirItemInfo object or NULL indicating something wrong with the path
      */
    virtual DirItemInfo        *validateUrlPath(const QString &urlPath);

    /*!
      * \brief isRemote() It must return TRUE when type() is greater than Location::TrashDisk
      * \return
      */
    inline  bool       isRemote()     const
    {
        return m_type > TrashDisk;
    }
    inline  bool       isLocalDisk()  const
    {
        return m_type == LocalDisk;
    }
    inline  bool       isTrashDisk()  const
    {
        return m_type == TrashDisk;
    }

public: //virtual
    virtual void        fetchExternalChanges(const QString &urlPath,
                                             const DirItemInfoList &list,
                                             QDir::Filters dirFilter) ;
    virtual void        setInfoItem(const DirItemInfo &itemInfo);
    virtual void        setInfoItem(DirItemInfo *itemInfo);
    virtual bool        isRoot() const;
    virtual bool        isWritable() const;
    virtual bool        isReadable() const;
    virtual QString     urlPath() const;
    virtual void        startWorking();
    virtual void        stopWorking();
    virtual QString     currentAuthenticationUser();
    virtual QString     currentAuthenticationPassword();

public: //non virtual
    void                notifyItemNeedsAuthentication(const DirItemInfo *item = 0);
    bool                useAuthenticationDataIfExists(const DirItemInfo &item);

    inline const DirItemInfo  *info() const
    {
        return m_info;
    }
    inline int                 type() const
    {
        return m_type;
    }
    const DirItemInfo         *currentInfo(); //updated information about the current path

protected:
    DirItemInfo                 *m_info;
    int                          m_type;
    bool                         m_usingExternalWatcher;

#if defined(REGRESSION_TEST_FOLDERLISTMODEL)
    friend class TestDirModel;
#endif

};
#endif // LOCATION_H

/*
 * Copyright (C) 2012 Robin Burchell <robin+nemo@viroteck.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef DIRMODEL_H
#define DIRMODEL_H


#include <QStringList>
#include <QDir>
#include <QSet>
#include <QQmlParserStatus>

#include "iorequest.h"
#include "filecompare.h"
#include "diritemabstractlistmodel.h"
#include "diriteminfo.h"

class FileSystemAction;
class Clipboard;
class DirSelection;
class LocationsFactory;
class Location;
class ExternalFSWatcher;
class NetAuthenticationDataList;

class DirModel : public DirItemAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY canGoBackChanged)
    Q_PROPERTY(bool awaitingResults READ awaitingResults NOTIFY awaitingResultsChanged)
    Q_PROPERTY(bool filterDirectories READ filterDirectories WRITE setFilterDirectories NOTIFY filterDirectoriesChanged)
    Q_PROPERTY(bool isRecursive READ isRecursive WRITE setIsRecursive NOTIFY isRecursiveChanged)
    Q_PROPERTY(bool readsMediaMetadata READ readsMediaMetadata WRITE setReadsMediaMetadata NOTIFY readsMediaMetadataChanged)
    Q_PROPERTY(bool showDirectories READ showDirectories WRITE setShowDirectories NOTIFY showDirectoriesChanged)
    Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(DirSelection *selectionObject READ selectionObject CONSTANT)
    Q_PROPERTY(QString parentPath READ parentPath NOTIFY pathChanged)
    Q_PROPERTY(bool showHiddenFiles READ getShowHiddenFiles WRITE setShowHiddenFiles NOTIFY showHiddenFilesChanged)
    Q_PROPERTY(bool onlyAllowedPaths READ getOnlyAllowedPaths WRITE setOnlyAllowedPaths NOTIFY onlyAllowedPathsChanged)
    Q_PROPERTY(SortBy sortBy READ getSortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(SortOrder sortOrder READ getSortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(int clipboardUrlsCounter READ getClipboardUrlsCounter NOTIFY clipboardChanged)
    Q_PROPERTY(bool enableExternalFSWatcher READ getEnabledExternalFSWatcher WRITE setEnabledExternalFSWatcher NOTIFY enabledExternalFSWatcherChanged)

public:
    enum Roles {
        FileNameRole = Qt::UserRole,
        AccessedDateRole,
        CreationDateRole,
        ModifiedDateRole,
        FileSizeRole,
        IconSourceRole,
        IconNameRole,
        FilePathRole,
        MimeTypeRole,
        MimeTypeDescriptionRole,
        IsDirRole,
        IsHostRole,         //!< it can also be used for other protocols than smb/cifs
        IsRemoteRole,
        IsLocalRole,
        NeedsAuthenticationRole,
        IsSmbWorkgroupRole,
        IsSmbShareRole,
        IsSharedDirRole,    //!< it can also be used for other protocols than smb/cifs
        IsSharingAllowedRole,//!< true for local directories (not in Trash) and not IsSharedDirRole
        IsBrowsableRole,     //!< any Dir, Host, WorkGroup or Share
        IsFileRole,
        IsReadableRole,
        IsWritableRole,
        IsExecutableRole,
        IsSelectedRole,
        TrackTitleRole,
        TrackArtistRole,
        TrackAlbumRole,
        TrackYearRole,
        TrackNumberRole,
        TrackGenreRole,
        TrackLengthRole,
        TrackCoverRole
    };

public:
    explicit DirModel(QObject *parent = 0);
    ~DirModel();

    // TODO: Make them properties
    Q_INVOKABLE QDateTime curPathAccessedDate() const;
    Q_INVOKABLE QDateTime curPathCreatedDate()  const;
    Q_INVOKABLE QDateTime curPathModifiedDate() const;
    Q_INVOKABLE QString curPathAccessedDateLocaleShort() const;
    Q_INVOKABLE QString curPathCreatedDateLocaleShort()  const;
    Q_INVOKABLE QString curPathModifiedDateLocaleShort() const;
    Q_INVOKABLE bool curPathIsWritable() const;

    Q_INVOKABLE void rm(const QStringList &paths);
    Q_INVOKABLE bool rename(const QString &oldName, const QString &newName);
    Q_INVOKABLE bool rename(int row, const QString &newName);
    Q_INVOKABLE bool mkdir(const QString &newdir);
    Q_INVOKABLE bool touch(const QString &newfile);

    Q_INVOKABLE QString getIcon(const QString &path) const;

    Q_INVOKABLE QString homePath() const;

    Q_INVOKABLE QString lastFolderVisited() const;


    /*!
     *    \brief Tries to make the directory pointed by row as the current to be browsed
     *    \return true if row points to a directory and the directory is readble, false otherwise
     */
    Q_INVOKABLE  bool cdIntoIndex(int row);
    Q_INVOKABLE  bool cdIntoPath(const QString &filename);

    /*!
     * \brief copyIndex() puts the item pointed by \a row (dir or file) into the clipboard
     * \param row points to the item file or directory
     */
    Q_INVOKABLE void  copyIndex(int row);

    /*!
     *  \brief copyPaths(const QStringList& urls) several items (dirs or files) into the clipboard
     *  \param items  fullpathnames or names only
     */
    Q_INVOKABLE void  copyPaths(const QStringList &items);

    /*!
     * \brief cutIndex() puts the item into the clipboard as \ref copy(),
     *        mark the item to be removed after \ref paste()
     * \param row points to the item file or directory
     */
    Q_INVOKABLE void  cutIndex(int row);

    /*!
     *  \brief cut() puts several items (dirs or files) into the clipboard as \ref copy(),
     *         mark the item to be removed after \ref paste()
     *   \param items  fullpathnames or names only
     */
    Q_INVOKABLE void  cutPaths(const QStringList &items);

    /*!
     * \brief removeIndex();  remove a item file or directory
     *
     * I gets the item indicated by \row and calls \ref rm()
     *
     * \param row points to the item to b e removed
     * \return true if it was possible to remove the item
     */
    Q_INVOKABLE void removeIndex(int row);

    /*!
     *  Just calls \ref rm()
     */
    Q_INVOKABLE void removePaths(const QStringList &items);

    /*!
     *  Tries to open a file using a suitable application, if the index points to a directory
     *  it goes into it using \ref cdIntoIndex() or \ref cdIntoPath()
     *
     *  \note Qt uses Qt QDesktopServices::openUrl()
     */
    Q_INVOKABLE bool  openIndex(int row);

    /*!
     *  Same as \ref openIndex() but using a file name instead of index
     *
     *  It allows to open directories and files using absoulte paths
     *
     *  \sa \ref cdIntoPath()
     */
    Q_INVOKABLE bool  openPath(const QString &filename);

    /*!
     *   \brief getProgressCounter() returns the number of \ref progress() notifications an Action will perform
     *
     *   It may be useful to decide about showing or not a progress dialog for Remove/Copy/Cut/Paste Actions
     *
     *   This function can be called just after receiving first \ref progress() notification
     *
     *   \note In the future this \ref getProgressCounter() and \ref progress() will merge to single signal that
     *         will send the Action full information, it will allow to have multi thread Actions.
     *         Also \ref cancelAction() needs to  change
     */
    Q_INVOKABLE int   getProgressCounter() const;

    // some helper functions that can be useful to other QML applications than File Manager
    Q_INVOKABLE bool existsDir(const QString  &folderName) const;
    Q_INVOKABLE bool canReadDir(const QString &folderName) const;
    Q_INVOKABLE bool existsFile(const QString &fileName) const;
    Q_INVOKABLE bool canReadFile(const QString &fileName) const;

    // Trash functions
    Q_INVOKABLE  void  moveIndexToTrash(int index);

    Q_INVOKABLE  void  restoreIndexFromTrash(int index);


    Q_INVOKABLE  void  setPathWithAuthentication(const QString &path, const QString &user, const QString &password, bool  savePassword);

    //download functions
    //
    /*! \brief download(int index) download file pointed by \a index into standard Download location
     *
     * \return true if the download could be started, othewise false
     */
    Q_INVOKABLE  bool download(int index);

    /*! \brief downloadAndSaveAs(int index, const QString& filename) download file pointed by \a index and save it as \a filename
     *
     * \return true if the download could be started, othewise false
     *
     */
    Q_INVOKABLE  bool downloadAndSaveAs(int index, const QString &filename);

    /*! \brief downloadAsTemporaryFile(int index)  save download as temporary, useful to open remote files
     *
     *  At the end if download is OK the signal downloadTemporaryComplete(const QString& fullpathname) is emitted
     *
     * \return true if the download could be started, othewise false
     *
     */
    Q_INVOKABLE  bool downloadAsTemporaryFile(int index);

    // TODO: this won't be safe if the model can change under the holder of the row
    Q_INVOKABLE QVariant data(int row, const QByteArray &stringRole) const;
    Q_INVOKABLE void refresh()
    {
        // just some syntactical sugar really
        setPath(path());
    }

    static void registerMetaTypes();

    //DirItemAbstractListModel
    virtual int getIndex(const QString &name);
    virtual void notifyItemChanged(int row);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &index = QModelIndex()) const
    {
        if (index.parent() != QModelIndex())
            return 0;

        return mDirectoryContents.count();
    }

    inline QString path() const
    {
        return mCurrentDir;
    }
    void setPath(const QString &pathName, const QString &user = QString(),
                 const QString &password = QString(), bool savePassword = false);

    bool canGoBack() const;
    bool awaitingResults() const;
    bool filterDirectories() const;
    bool isRecursive() const;
    bool readsMediaMetadata() const;
    bool showDirectories() const;
    QStringList nameFilters() const;

    void setNameFilters(const QStringList &nameFilters);

    DirSelection *selectionObject() const;

    void classBegin();
    // WORKAROUND: check componentComplete() definition in .cpp file
    void componentComplete();

    static QString getIcon(QString absoluteFilePath, QMimeType mime, bool isSmbWorkgroup = false,
                           bool isBrowsable = false, bool isHost = false);

    QString parentPath() const;


    bool getShowHiddenFiles() const;
    bool getOnlyAllowedPaths() const;

    Q_ENUMS(SortBy)
    enum SortBy {
        SortByName,
        SortByDate,
        SortBySize
    };
    SortBy getSortBy() const;

    Q_ENUMS(SortOrder)
    enum SortOrder {
        SortAscending   = Qt::AscendingOrder,
        SortDescending = Qt::DescendingOrder
    };
    SortOrder getSortOrder() const;


    int getClipboardUrlsCounter() const;
    bool  getEnabledExternalFSWatcher() const;
    void  restoreIndexesFromTrash(const QList<int> &);
    void  moveIndexesToTrash(const QList<int> &);


public slots:   // Also invokable from QML
    /*!
       * \brief copySelection() copy selected items to the clipboard
       */
    void  copySelection();

    /*!
     * \brief cutSelection() cut selected items to the clipboard
     */
    void  cutSelection();

    /*!
     * \brief removeSelection() remove selected items, it handles Trash items
     */
    void  removeSelection();

    /*!
     * \brief moveSelectionToTrash() move selected items from Local Disk (only) to Local Trash
     */
    void  moveSelectionToTrash();

    /*!
     * \brief restoreSelectionFromTrash() restore selected trash items to their orginal location
     */
    void  restoreSelectionFromTrash();

    /*!
     * \brief restoreTrash() restore all items being actually browsed in the Trash
     *
     */
    void   restoreTrash();

    /*!
     * \brief emptyTrash() remove definitely all items being actually browsed in the Trash
     *
     *  \sa \ref removeSelection() and \ref rm()
     *
     */
    void  emptyTrash();

    /*!
     * \brief goHome() goes to user home dir
     *  Go to user home dir, we may have a tab for places or something like that
     */
    void  goHome();

    /*!
     * \brief goTrash() goes to logical folder trash:///
     */
    void goTrash();

    /*!
     * \brief goBack() goes to the previous folder if available
     *
     */
    void goBack();

    /*!
     * \brief cdUp() sets the parent directory as current directory
     *
     *  It can work as a back function if there is no user input path
     * \return true if it was possible to change to parent dir, otherwise false
     */
    bool  cdUp();

    /*!
     * \brief paste() copy item(s) from \ref copy() and \ref paste() into the current directory
     *
     *  If the operation was \ref cut(), then remove the original item
     */
    void paste();

    /*!
     * \brief clears clipboard entries
     */
    void clearClipboard();

    /*!
     * \brief cancelAction() any copy/cut/remove can be cancelled
     */
    void cancelAction();

    void setIsRecursive(bool isRecursive);
    void setReadsMediaMetadata(bool readsMediaMetadata);
    void setFilterDirectories(bool filterDirectories);
    void setShowDirectories(bool showDirectories);
    void setShowHiddenFiles(bool show);
    /*!
     * \brief if set to true then only Allowed paths are shown or be modified
     */
    void setOnlyAllowedPaths(bool onlyAllowedPaths);
    void setSortBy(SortBy field);
    void setSortOrder(SortOrder order);
    void setEnabledExternalFSWatcher(bool enable);


    void toggleShowDirectories();
    void toggleShowHiddenFiles();
    void toggleSortOrder();
    void toggleSortBy();

    /*!
     * \brief Adds a directory to the set of directories that are accessible when "onlyAllowedPaths" property is set.
     */
    inline void addAllowedDirectory(const QString &allowedDirAbsolutePath)
    {
        m_allowedDirs << allowedDirAbsolutePath;
    }

    inline void removeAllowedDirectory(const QString &allowedDirAbsolutePath)
    {
        m_allowedDirs.remove(allowedDirAbsolutePath);
    }
    bool isAllowedPath(const QString &absolutePath) const;

public slots:
    void onItemsAdded(const DirItemInfoList &newFiles);
    void onItemsFetched();

private:
    QHash<int, QByteArray> buildRoleNames() const;
    QHash<int, QByteArray> roleNames() const;
    QStringList mNameFilters;
    bool mFilterDirectories;
    bool mShowDirectories;
    bool mAwaitingResults;
    bool mIsRecursive;
    bool mReadsMediaMetadata;
    QString mCurrentDir;
    DirItemInfoList  mDirectoryContents;

signals:
    void countChanged();
    void canGoBackChanged();
    void awaitingResultsChanged();
    void nameFiltersChanged();
    void filterDirectoriesChanged();
    void isRecursiveChanged();
    void readsMediaMetadataChanged();
    void showDirectoriesChanged();
    void pathChanged(const QString &newPath);
    void error(const QString &errorTitle, const QString &errorMessage);

    /*!
     * \brief needsAuthentication()
     *  This notifies the UI that the current URL being browsed needs to set
     *   user/password to perform an authentication
     *
     *  The UI must ask for "user" and "password" for the current URL and then call
     *  \ref setAuthentication()
     *
     * \param user       current user being used
     * \param urlPath    the current URL asked to be browsed
     */
    void     needsAuthentication(const QString &user, const QString &urlPath);

    /*!
     * \brief insertedRow()
     *
     *  It happens when a new file is inserted in an existent view,
     *  for example from  \ref mkdir() or \ref paste()
     *
     *  It can be used to make the new row visible to the user doing a scroll to
     */
    void  insertedRow(int row);

    /*!
     * \brief progress()
     *  Sends status about recursive and multi-items remove/move/copy
     *
     * \param curItem     current item being handled
     * \param totalItems  total of items including recursive directories content
     * \param percent     a percent done
     */
    void     progress(int curItem, int totalItems, int percent);

    void     showHiddenFilesChanged();
    void     onlyAllowedPathsChanged();
    void     sortByChanged();
    void     sortOrderChanged();
    void     clipboardChanged();
    void     enabledExternalFSWatcherChanged(bool);

    /*!
     * \brief downloadTemporaryComplete() says that download has been completed and
     *    the \a filename is ready to be used, filename is a full pathname
     */
    void     downloadTemporaryComplete(const QString &filename);

private slots:
    void onItemRemoved(const DirItemInfo &);
    void onItemAdded(const DirItemInfo &);
    void onItemChanged(const DirItemInfo &);

private:
    int           addItem(const DirItemInfo &fi);
    void          setCompareAndReorder();
    int           rowOfItem(const DirItemInfo &fi);
    QDir::Filters currentDirFilter()  const;
    QString       dirItems(const DirItemInfo &fi) const;
    bool          cdIntoItem(const DirItemInfo &fi);
    bool          openItem(const DirItemInfo &fi);
    DirItemInfo   setParentIfRelative(const QString &fileOrDir) const;
    void          setPathFromCurrentLocation();

private:
    void          startExternalFsWatcher();
    void          stoptExternalFsWatcher();
    void          clear();

private slots:
    void          onItemAddedOutsideFm(const DirItemInfo &fi);
    void          onItemRemovedOutSideFm(const DirItemInfo &);
    void          onItemChangedOutSideFm(const DirItemInfo &fi);
    void          onThereAreExternalChanges(const QString &);
    void          onExternalFsWorkerFinished(int);


private:
    bool                mShowHiddenFiles;
    bool                mOnlyAllowedPaths;
    SortBy              mSortBy;
    SortOrder           mSortOrder;
    CompareFunction     mCompareFunction;
    bool                mExtFSWatcher;
    Clipboard          *mClipboard;
    DirSelection       *mSelection;
    NetAuthenticationDataList *mAuthData;
    LocationsFactory   *mLocationFactory;
    Location           *mCurLocation;
    QStringList         mPathList;    //!< it will be used for goBack()

private:
    bool mQmlCompleted;
    QString mQmlCachePath;

private:
    FileSystemAction    *m_fsAction;  //!< it does file system recursive remove/copy/move
    QString  fileSize(qint64 size)  const;
#ifndef DO_NOT_USE_TAG_LIB
    QVariant getAudioMetaData(const QFileInfo &fi, int role) const;
#endif
    QSet<QString> m_allowedDirs;

//[0]

#if defined(REGRESSION_TEST_FOLDERLISTMODEL)
    ExternalFSWatcher *getExternalFSWatcher() const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant  headerData(int section, Qt::Orientation orientation, int role) const;
    friend class TestDirModel;
#endif

    bool allowAccess(const DirItemInfo &fi) const;
    bool allowCurrentPathAccess() const;
};


#endif // DIRMODEL_H

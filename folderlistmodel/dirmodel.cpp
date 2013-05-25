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

#include <errno.h>
#include <string.h>
#include "dirmodel.h"
#include "ioworkerthread.h"
#include "filesystemaction.h"

#include <QDirIterator>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QFileIconProvider>
#include <QUrl>
#include <QDesktopServices>

#if defined(REGRESSION_TEST_FOLDERLISTMODEL) || QT_VERSION >= 0x050000
# include <QMimeType>
# include <QMimeDatabase>
#endif

#define IS_VALID_ROW(row)             (row >=0 && row < mDirectoryContents.count())
#define WARN_ROW_OUT_OF_RANGE(row)    qWarning() << Q_FUNC_INFO << "row" << row << "Out of bounds access"


Q_GLOBAL_STATIC(IOWorkerThread, ioWorkerThread)

namespace {
    QHash<QByteArray, int> roleMapping;
}




static bool fileCompareExists(const QFileInfo &a, const QFileInfo &b)
{
    if (a.isDir() && !b.isDir())
        return true;

    if (b.isDir() && !a.isDir())
        return false;

    bool ret = QString::localeAwareCompare(a.absoluteFilePath(), b.absoluteFilePath()) < 0;
#if DEBUG_MESSAGES
    qDebug() <<  Q_FUNC_INFO << ret << a.absoluteFilePath() << b.absoluteFilePath();
#endif
    return ret;
}

static bool fileCompareAscending(const QFileInfo &a, const QFileInfo &b)
{
    if (a.isDir() && !b.isDir())
        return true;

    if (b.isDir() && !a.isDir())
        return false;

    return QString::localeAwareCompare(a.fileName(), b.fileName()) < 0;
}


static bool fileCompareDescending(const QFileInfo &a, const QFileInfo &b)
{
    if (a.isDir() && !b.isDir())
        return true;

    if (b.isDir() && !a.isDir())
        return false;

    return QString::localeAwareCompare(a.fileName(), b.fileName()) > 0;
}

static bool dateCompareDescending(const QFileInfo &a, const QFileInfo &b)
{
    if (a.isDir() && !b.isDir())
        return true;

    if (b.isDir() && !a.isDir())
        return false;

    return a.lastModified() > b.lastModified();
}

static bool dateCompareAscending(const QFileInfo &a, const QFileInfo &b)
{
    if (a.isDir() && !b.isDir())
        return true;

    if (b.isDir() && !a.isDir())
        return false;

    return a.lastModified() < b.lastModified();
}

/*!
 *  Sort was originally done in \ref onItemsAdded() and that code is now in \ref addItem(),
 *  the reason to keep doing sort and do not let QDir does it is that when adding new items
 *  by \ref mkdir() or \paste() it is not necessary to call refresh() to load the entire directory
 *  to organize it items again. New items order/position are organized by \ref addItem()
 *
 */
static CompareFunction availableCompareFunctions[2][2] =
{
    {fileCompareAscending, fileCompareDescending}
   ,{dateCompareAscending, dateCompareDescending}
};




class DirListWorker : public IORequest
{
    Q_OBJECT
public:
    DirListWorker(const QString &pathName, QDir::Filter filter)
        : mPathName(pathName)
        , mFilter(filter)
    { }

    void run()
    {
#if DEBUG_MESSAGES
        qDebug() << Q_FUNC_INFO << "Running on: " << QThread::currentThreadId();
#endif

        QDir tmpDir = QDir(mPathName, QString(), QDir::NoSort, mFilter);
        QDirIterator it(tmpDir);
        QVector<QFileInfo> directoryContents;

        while (it.hasNext()) {
            it.next();

            directoryContents.append(it.fileInfo());
            if (directoryContents.count() >= 50) {
                emit itemsAdded(directoryContents);

                // clear() would force a deallocation, micro-optimization
                directoryContents.erase(directoryContents.begin(), directoryContents.end());
            }
        }

        // last batch
        emit itemsAdded(directoryContents);
        emit workerFinished();
    }

signals:
    void itemsAdded(const QVector<QFileInfo> &files);
    void workerFinished();

private:
    QString       mPathName;
    QDir::Filter  mFilter;
};

DirModel::DirModel(QObject *parent)
    : QAbstractListModel(parent)
    , mShowDirectories(true)
    , mAwaitingResults(false)
    , mShowHiddenFiles(false)
    , mSortBy(SortByName)
    , mSortOrder(SortAscending)
    , mCompareFunction(0)
    , m_fsAction(new FileSystemAction(this) )
{
    mNameFilters = QStringList() << "*";

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // There's no setRoleNames in Qt5.
    setRoleNames(buildRoleNames());
#else
    // In Qt5, the roleNames() is virtual and will work just fine.
#endif

    connect(m_fsAction, SIGNAL(progress(int,int,int)),
            this,     SIGNAL(progress(int,int,int)));

    connect(m_fsAction, SIGNAL(added(QFileInfo)),
            this,     SLOT(onItemAdded(QFileInfo)));

    connect(m_fsAction, SIGNAL(added(QString)),
            this,     SLOT(onItemAdded(QString)));

    connect(m_fsAction, SIGNAL(removed(QFileInfo)),
            this,     SLOT(onItemRemoved(QFileInfo)));

    connect(m_fsAction, SIGNAL(removed(QString)),
            this,     SLOT(onItemRemoved(QString)));

    connect(m_fsAction, SIGNAL(error(QString,QString)),
            this,     SIGNAL(error(QString,QString)));

    connect(this,     SIGNAL(pathChanged(QString)),
            m_fsAction, SLOT(pathChanged(QString)));

    connect(m_fsAction, SIGNAL(clipboardChanged()),
            this,       SIGNAL(clipboardChanged()));

    setCompareAndReorder();
}

DirModel::~DirModel()
{
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
// roleNames has changed between Qt4 and Qt5. In Qt5 it is a virtual
 // function and setRoleNames should not be used.
QHash<int, QByteArray> DirModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles = buildRoleNames();
    }

    return roles;
}
#endif

QHash<int, QByteArray> DirModel::buildRoleNames() const
{
    QHash<int, QByteArray> roles;
        roles.insert(FileNameRole, QByteArray("fileName"));
        roles.insert(CreationDateRole, QByteArray("creationDate"));
        roles.insert(ModifiedDateRole, QByteArray("modifiedDate"));
        roles.insert(FileSizeRole, QByteArray("fileSize"));
        roles.insert(IconSourceRole, QByteArray("iconSource"));
        roles.insert(FilePathRole, QByteArray("filePath"));
        roles.insert(IsDirRole, QByteArray("isDir"));
        roles.insert(IsFileRole, QByteArray("isFile"));
        roles.insert(IsReadableRole, QByteArray("isReadable"));
        roles.insert(IsWritableRole, QByteArray("isWritable"));
        roles.insert(IsExecutableRole, QByteArray("isExecutable"));

    // populate reverse mapping
    if (roleMapping.isEmpty()) {
        QHash<int, QByteArray>::ConstIterator it = roles.constBegin();
        for (;it != roles.constEnd(); ++it)
            roleMapping.insert(it.value(), it.key());

        // make sure we cover all roles
        //    Q_ASSERT(roles.count() == IsFileRole - FileNameRole);
    }

    return roles;
}

QVariant DirModel::data(int row, const QByteArray &stringRole) const
{
    QHash<QByteArray, int>::ConstIterator it = roleMapping.constFind(stringRole);

    if (it == roleMapping.constEnd())
        return QVariant();

    return data(index(row, 0), *it);
}

QVariant DirModel::data(const QModelIndex &index, int role) const
{
#if defined(REGRESSION_TEST_FOLDERLISTMODEL)
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole) )
    {
        return QVariant();
    }
    if (role == Qt::DecorationRole)
    {
        if (index.column() == 0)
        {
            QMimeDatabase database;
            QIcon icon;
            QMimeType mime = database.mimeTypeForFile(mDirectoryContents.at(index.row()));
            if (mime.isValid()) {               
                icon = QIcon::fromTheme(mime.iconName());                
            }
            if (icon.isNull()) {
                icon = QFileIconProvider().icon(mDirectoryContents.at(index.row()));             
            }           
            return icon;
        }
        return QVariant();
    }
    role = FileNameRole + index.column();
#else
    if (role < FileNameRole || role > IsExecutableRole) {
        qWarning() << Q_FUNC_INFO << "Got an out of range role: " << role;
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= mDirectoryContents.count()) {
        qWarning() << "Attempted to access out of range row: " << index.row();
        return QVariant();
    }

    if (index.column() != 0)
        return QVariant();
#endif

    const QFileInfo &fi = mDirectoryContents.at(index.row());

    switch (role) {
        case FileNameRole:
            return fi.fileName();
        case CreationDateRole:
            return fi.created();
        case ModifiedDateRole:
            return fi.lastModified();
        case FileSizeRole: {
             if (fi.isDir())
             {
                return dirItems(fi);
             }
             return fileSize(fi.size());
        }
        case IconSourceRole: {
            const QString &fileName = fi.fileName();

            if (fi.isDir())
                return QLatin1String("image://theme/icon-m-common-directory");

            if (fileName.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive) ||
                fileName.endsWith(QLatin1String(".png"), Qt::CaseInsensitive)) {
                return QLatin1String("image://nemoThumbnail/") + fi.filePath();
            }

            return "image://theme/icon-m-content-document";
        }
        case FilePathRole:
            return fi.filePath();
        case IsDirRole:
            return fi.isDir();
        case IsFileRole:
            return !fi.isDir();
        case IsReadableRole:
            return fi.isReadable();
        case IsWritableRole:
            return fi.isWritable();
        case IsExecutableRole:
            return fi.isExecutable();
        default:
#if !defined(REGRESSION_TEST_FOLDERLISTMODEL)
            // this should not happen, ever
            Q_ASSERT(false);
            qWarning() << Q_FUNC_INFO << "Got an unknown role: " << role;
#endif
            return QVariant();
    }
}

void DirModel::setPath(const QString &pathName)
{
    if (pathName.isEmpty())
        return;

    if (mAwaitingResults) {
        // TODO: handle the case where pathName != our current path, cancel old
        // request, start a new one
        qDebug() << Q_FUNC_INFO << "Ignoring path change request, request already running";
        return;
    }

    mAwaitingResults = true;
    emit awaitingResultsChanged();
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "Changing to " << pathName << " on " << QThread::currentThreadId();
#endif
    beginResetModel();
    mDirectoryContents.clear();
    endResetModel();

    QDir::Filter dirFilter = currentDirFilter();
    // TODO: we need to set a spinner active before we start getting results from DirListWorker
    DirListWorker *dlw = new DirListWorker(pathName, dirFilter);
    connect(dlw, SIGNAL(itemsAdded(QVector<QFileInfo>)), SLOT(onItemsAdded(QVector<QFileInfo>)));
    connect(dlw, SIGNAL(workerFinished()), SLOT(onResultsFetched()));
    ioWorkerThread()->addRequest(dlw);

    mCurrentDir = pathName;
    emit pathChanged(pathName);
}


void DirModel::onResultsFetched() {
    if (mAwaitingResults) {
#if DEBUG_MESSAGES
        qDebug() << Q_FUNC_INFO << "No longer awaiting results";
#endif
        mAwaitingResults = false;
        emit awaitingResultsChanged();
    }
}

void DirModel::onItemsAdded(const QVector<QFileInfo> &newFiles)
{
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "Got new files: " << newFiles.count();
#endif

    foreach (const QFileInfo &fi, newFiles) {

        bool doAdd = true;
        foreach (const QString &nameFilter, mNameFilters) {
            // TODO: using QRegExp for wildcard matching is slow
            QRegExp re(nameFilter, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (!re.exactMatch(fi.fileName())) {
                doAdd = false;
                break;
            }
        }

        if (!doAdd)
            continue;

        addItem(fi);
    }
}

void DirModel::rm(const QStringList &paths)
{
   m_fsAction->remove(paths);
}

bool DirModel::rename(int row, const QString &newName)
{
#if DEBUG_MESSAGES
    qDebug() << Q_FUNC_INFO << "Renaming " << row << " to " << newName;
#endif

    if (!IS_VALID_ROW(row)) {
        WARN_ROW_OUT_OF_RANGE(row);
        return false;
    }

    const QFileInfo &fi = mDirectoryContents.at(row);
    QString newFullFilename(fi.absolutePath() + QDir::separator() + newName);

    //QFile::rename() works for File and Dir
    QFile f(fi.absoluteFilePath());
    bool retval = f.rename(newFullFilename);
    if (!retval)
    {
        qDebug() << Q_FUNC_INFO << "Rename returned error code: " << f.error() << f.errorString();
        emit(QObject::tr("Rename error"), f.errorString());
    }
    else
    {
        mDirectoryContents[row] = QFileInfo(newFullFilename);
        QModelIndex idx = createIndex(row,0);
        emit dataChanged(idx, idx);
    }
    return retval;
}

void DirModel::mkdir(const QString &newDir)
{
    QDir dir(mCurrentDir);
    bool retval = dir.mkdir(newDir);
    if (!retval) {
        const char *errorStr = strerror(errno);
        qDebug() << Q_FUNC_INFO << "Error creating new directory: " << errno << " (" << errorStr << ")";
        emit error(QObject::tr("Error creating new folder"), errorStr);
    } else {
        onItemAdded(dir.filePath(newDir));
    }
}

bool DirModel::showDirectories() const
{
    return mShowDirectories;
}

void DirModel::setShowDirectories(bool showDirectories)
{
    mShowDirectories = showDirectories;
    refresh();
    emit showDirectoriesChanged();
}

QStringList DirModel::nameFilters() const
{
    return mNameFilters;
}

void DirModel::setNameFilters(const QStringList &nameFilters)
{
    mNameFilters = nameFilters;
    refresh();
    emit nameFiltersChanged();
}

bool DirModel::awaitingResults() const
{
    return mAwaitingResults;
}


QString DirModel::parentPath() const
{
    QDir dir(mCurrentDir);
    if (dir.isRoot()) {
        qDebug() << Q_FUNC_INFO << "already at root";
        return mCurrentDir;
    }

    bool success = dir.cdUp();
    if (!success) {
        qWarning() << Q_FUNC_INFO << "Failed to to go to parent of " << mCurrentDir;
        return mCurrentDir;
    }
    qDebug() << Q_FUNC_INFO << "returning" << dir.absolutePath();
    return dir.absolutePath();
}

QString DirModel::homePath() const
{
    return QDir::homePath();
}

#if defined(REGRESSION_TEST_FOLDERLISTMODEL)
 QVariant  DirModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
   {
       QVariant ret;
       QHash<int, QByteArray> roles = this->roleNames();
       section += FileNameRole;
       if (roles.contains(section))
       {
           QString header=  QString(roles.value(section));
           ret = header;
       }
       return ret;
   }
   return QAbstractItemModel::headerData(section, orientation, role);
 }
#endif


void DirModel::goHome()
{
    setPath(QDir::homePath());
}


bool DirModel::cdUp()
{
    int ret = false;
    if (!mCurrentDir.isEmpty()) // we are in any dir
    {
        QDir current(mCurrentDir);
        if (current.cdUp())
        {
            setPath(current.absolutePath());
            ret = true;
        }
    }
    return ret;
}


void DirModel::removeIndex(int row)
{
    if (IS_VALID_ROW(row))
    {
        const QFileInfo &fi = mDirectoryContents.at(row);
        QStringList list(fi.absoluteFilePath());
        this->rm(list);
    }
    else
    {
        WARN_ROW_OUT_OF_RANGE(row);
    }
}

void DirModel::removePaths(const QStringList& items)
{
     this->rm(items);
}

void DirModel::copyIndex(int row)
{
    if (IS_VALID_ROW(row))
    {
        const QFileInfo &fi = mDirectoryContents.at(row);
        QStringList list(fi.absoluteFilePath());
        this->copyPaths(list);
    }
    else
    {
        WARN_ROW_OUT_OF_RANGE(row);
    }
}

void DirModel::copyPaths(const QStringList &items)
{
   m_fsAction->copy(items);
}


void DirModel::cutIndex(int row)
{
    if (IS_VALID_ROW(row))
    {
        const QFileInfo &fi = mDirectoryContents.at(row);
        QStringList list(fi.absoluteFilePath());
        m_fsAction->cut(list);
    }
    else
    {
        WARN_ROW_OUT_OF_RANGE(row);
    }
}


void DirModel::cutPaths(const QStringList &items)
{
     m_fsAction->cut(items);
}


void DirModel::paste()
{
   m_fsAction->paste();
}


bool  DirModel::cdIntoIndex(int row)
{
    bool ret = false;
    if (IS_VALID_ROW(row))
    {
        ret = cdInto(mDirectoryContents.at(row));
    }
    else
    {
        WARN_ROW_OUT_OF_RANGE(row);
    }
    return ret;
}


bool  DirModel::cdIntoPath(const QString &filename)
{
    QFileInfo fi(filename);
    if (fi.isRelative())
    {
        fi.setFile(mCurrentDir, filename);
    }
    return cdInto(fi);
}


bool  DirModel::cdInto(const QFileInfo &fi)
{
    bool ret = false;
    if (fi.exists() && fi.isDir() && fi.isReadable())
    {
        QDir childDir(mCurrentDir);
        if ( childDir.cd(fi.fileName()) )
        {
            setPath(childDir.absolutePath());
            ret = true;
        }
    }
    return ret;
}

/*!
 * \brief DirModel::onItemRemoved()
 * \param pathname full pathname of removed file
 */
void DirModel::onItemRemoved(const QString &pathname)
{
    QFileInfo info(pathname);
    onItemRemoved(info);
}


void DirModel::onItemRemoved(const QFileInfo &fi)
{  
    int row = rowOfItem(fi);   
    if (row >= 0)
    {
        beginRemoveRows(QModelIndex(), row, row);
        mDirectoryContents.remove(row,1);
        endRemoveRows();
    }
}

/*!
 * \brief DirModel::onItemAdded()
 * \param pathname full pathname of the added file
 */
void DirModel::onItemAdded(const QString &pathname)
{
    QFileInfo info(pathname);
    onItemAdded(info);
}


void DirModel::onItemAdded(const QFileInfo &fi)
{
    int newRow = addItem(fi);
    emit insertedRow(newRow);
}

/*!
 * \brief DirModel::addItem() adds an item into the model
 *        This code was moved from onItemsAdded(const QVector<QFileInfo> &newFiles),
 *           the reason is:  this code now is used for \ref mkdir() and for \ref paste() operations
 *           that inserts new items
 * \param fi
 * \return  the index where it was inserted, it can be used in the view
 * \sa insertedRow()
 */
int DirModel::addItem(const QFileInfo &fi)
{
    QVector<QFileInfo>::Iterator it = qLowerBound(mDirectoryContents.begin(),
                                                  mDirectoryContents.end(),
                                                  fi,
                                                  mCompareFunction);
    int idx =  mDirectoryContents.count();

    if (it == mDirectoryContents.end()) {
        beginInsertRows(QModelIndex(), mDirectoryContents.count(), mDirectoryContents.count());
        mDirectoryContents.append(fi);
        endInsertRows();
    } else {
        idx = it - mDirectoryContents.begin();
        beginInsertRows(QModelIndex(), idx, idx);
        mDirectoryContents.insert(it, fi);
        endInsertRows();
    }   
    return idx;
}



void DirModel::cancelAction()
{
    m_fsAction->cancel();
}


QString DirModel::fileSize(qint64 size) const
{
    struct UnitSizes
    {
        qint64      bytes;
        const char *name;
    };

    static UnitSizes m_unitBytes[5] =
    {
        { 1,           "Bytes" }
       ,{1024,         "KB"}
        // got it from http://wiki.answers.com/Q/How_many_bytes_are_in_a_megabyte
       ,{1000 * 1000,  "MB"}
       ,{1000 *  m_unitBytes[2].bytes,   "GB"}
       ,{1000 *  m_unitBytes[3].bytes, "TB"}
    };

    QString ret;
    int unit = sizeof(m_unitBytes)/sizeof(m_unitBytes[0]);
    while( unit-- > 1 && size < m_unitBytes[unit].bytes );
    if (unit > 0 )
    {
        ret.sprintf("%0.1f %s", (float)size/m_unitBytes[unit].bytes,
                    m_unitBytes[unit].name);
    }
    else
    {
        ret.sprintf("%ld %s", (long int)size, m_unitBytes[0].name);
    }
    return ret;
}



bool DirModel::getShowHiddenFiles() const
{
    return mShowHiddenFiles;
}


void DirModel::setShowHiddenFiles(bool show)
{
    if (show != mShowHiddenFiles)
    {
        mShowHiddenFiles = show;
        refresh();
        emit showHiddenFilesChanged();
    }
}


void DirModel::toggleShowDirectories()
{
    setShowDirectories(!mShowDirectories);
}


void DirModel::toggleShowHiddenFiles()
{
    setShowHiddenFiles(!mShowHiddenFiles);
}


DirModel::SortBy
DirModel::getSortBy()  const
{
    return mSortBy;
}


void DirModel::setSortBy(SortBy field)
{
    if (field != mSortBy)
    {
        mSortBy = field;
        setCompareAndReorder();
        emit sortByChanged();
    }
}


DirModel::SortOrder
DirModel::getSortOrder() const
{
    return mSortOrder;
}

void DirModel::setSortOrder(SortOrder order)
{
    if ( order != mSortOrder )
    {
        mSortOrder = order;
        setCompareAndReorder();
        emit sortOrderChanged();
    }
}


void DirModel::toggleSortOrder()
{
    SortOrder  order = static_cast<SortOrder> (mSortOrder ^ 1);
    setSortOrder(order);
}


void DirModel::toggleSortBy()
{
    SortBy by = static_cast<SortBy> (mSortBy ^ 1);
    setSortBy(by);
}

/*!
 * \brief DirModel::setCompareAndReorder() called when  SortOrder or SortBy change
 *
 *  It does not reload items from disk, just reorganize items from \a mDirectoryContents array
 */
void DirModel::setCompareAndReorder()
{
    mCompareFunction = availableCompareFunctions[mSortBy][mSortOrder];
    if (mDirectoryContents.count() > 0 && !mAwaitingResults )
    {
        QVector<QFileInfo> tmpDirectoryContents = mDirectoryContents;
        beginResetModel();
        mDirectoryContents.clear();
        endResetModel();
        for(int counter=0; counter < tmpDirectoryContents.count(); counter++)
        {
            addItem(tmpDirectoryContents.at(counter));
        }
    }
}


int DirModel::getClipboardUrlsCounter() const
{
    return m_fsAction->clipboardLocalUrlsConunter();
}


int DirModel::rowOfItem(const QFileInfo& fi)
{
    QVector<QFileInfo>::Iterator it = qBinaryFind(mDirectoryContents.begin(),
                                                  mDirectoryContents.end(),
                                                  fi,
                                                  fileCompareExists);
    int row;
    if (it == mDirectoryContents.end())
    {       
        row = -1;
    }
    else
    {
        row = it - mDirectoryContents.begin();
    }
    return row;
}


QDir::Filter DirModel::currentDirFilter() const
{
    int filter = QDir::AllEntries | QDir::NoDotAndDotDot ;
    if (!mShowDirectories)
    {
        filter &= ~QDir::AllDirs;
        filter &= ~QDir::Dirs;
    }
    if (mShowHiddenFiles)
    {
        filter |= QDir::Hidden;
    }
    QDir::Filter dirFilter = static_cast<QDir::Filter>(filter);
    return dirFilter;
}

QString DirModel::dirItems(const QFileInfo& fi) const
{
    int counter = 0;
    QDir d(fi.absoluteFilePath(), QString(), QDir::NoSort, currentDirFilter());
    counter = d.count();
    if (counter < 0)
    {
        counter = 0;
    }
    QString ret (QString::number(counter) + QLatin1Char(' '));
    ret += QObject::tr("items");
    return ret;
}


bool DirModel::openIndex(int row)
{
    bool ret = false;
    if (IS_VALID_ROW(row))
    {
        ret = openItem(mDirectoryContents.at(row));
    }
    else
    {
        WARN_ROW_OUT_OF_RANGE(row);
    }
    return ret;
}

bool DirModel::openPath(const QString &filename)
{
    QFileInfo fi(filename);
    if (fi.isRelative())
    {
        fi.setFile(mCurrentDir, filename);
    }
    return openItem(fi);
}

bool DirModel::openItem(const QFileInfo &fi)
{
    bool ret = false;
    if (fi.exists())
    {
        if (fi.isDir())
        {
            ret = cdInto(fi);
        }
        else
        {
            ret = QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
        }
    }
    return ret;
}

// for dirlistworker
#include "dirmodel.moc"

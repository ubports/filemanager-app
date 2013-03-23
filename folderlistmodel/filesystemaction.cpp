/**************************************************************************
 *
 * Copyright 2013 Canonical Ltd.
 * Copyright 2013 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: filesystemaction.cpp
 * Date: 3/13/2013
 */

#include "filesystemaction.h"
#include <sys/statvfs.h>
#include <errno.h>

#include <QDirIterator>
#include <QDebug>
#include <QTimer>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#define  STEP_FILES               10  // number of the files to work on a step, when this number is reached a signal is emited

#define SHOULD_EMIT_PROGRESS_SIGNAL(action)       (1)



RemoveNotifier   FileSystemAction::m_removeNotifier;



class DirModelMimeData : public QMimeData
{
public:
    enum DataOperation
    {
        Copy, Cut
    };
    explicit DirModelMimeData();
    virtual QStringList   formats() const    { return m_formats; }
    void                  setCutFormat();
    static DataOperation  operation(const QMimeData *mime);
    void setIntoClipboard(const QStringList& files, const QString &path, bool isCut = false);
    static const QMimeData *clipboardMimeData();
private:
    QStringList   m_formats;
};




DirModelMimeData::DirModelMimeData() :
    QMimeData()
{
    m_formats.append("text/uri-list");
    m_formats.append("x-special/gnome-copied-files");
    m_formats.append("text/plain");
    m_formats.append("COMPOUND_TEXT");
    m_formats.append("TARGETS");
    m_formats.append("MULTIPLE");
    m_formats.append("TIMESTAMP");
    m_formats.append("SAVE_TARGETS");
}

void DirModelMimeData::setCutFormat()
{
    m_formats.append("application/x-kde-cutselection");
}

DirModelMimeData::DataOperation
DirModelMimeData::operation(const QMimeData * mime)
{
    QStringList formats = mime->formats();
    int f = formats.count();
    while(f--)
    {
        const QString &mi = formats.at(f);
        if(mi.startsWith("x-special/") ||
           mi.startsWith("application/") )
        {
            if (mi.contains("cut"))
            {
                return Cut;
            }
        }
    }
    return Copy;
}


void DirModelMimeData::setIntoClipboard(const QStringList &files, const QString& path, bool isCut)
{
    if (isCut)  {
        setCutFormat();
    }

    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
    {
        if (clipboard->ownsClipboard())
        {
            //it used to crash, so leave it for now
            // clipboard->clear();
        }
        QList<QUrl> urls;
        QFileInfo fi;
        for(int counter = 0; counter < files.count(); counter++)
        {
            const QString& item = files.at(counter);
            fi.setFile(item);
            if (!fi.exists())
            {
                fi.setFile(path + QDir::separator() + item);
            }
            if (fi.exists())
            {
                urls.append(QUrl::fromLocalFile(fi.canonicalFilePath()));
            }
            else
            {                
                //emit error( QObject::tr("Item does not exist"), item);
            }
        }
        if (urls.count() > 0)
        {
            setUrls(urls);
            clipboard->setMimeData(this);
        }
    }
}


const QMimeData *DirModelMimeData::clipboardMimeData()
{
    const QMimeData *ret = 0;
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard)
    {
        ret = clipboard->mimeData();
    }
    return ret;
}


RemoveNotifier::RemoveNotifier(QObject *parent) :
    QObject(parent)
{
}

void RemoveNotifier::notifyRemoved(const QFileInfo &fi)
{
    emit removed(fi);
}


void RemoveNotifier::notifyRemoved(const QString &item)
{
    emit removed(item);
}



DirModelMimeData     mimeData;

FileSystemAction::FileSystemAction(QObject *parent) :
    QObject(parent)
  , m_curAction(0)
  , m_cancelCurrentAction(false)
  , m_busy(false)
  , m_mimeData ( & ::mimeData )  // using  static data because it used to crash
{
  //as m_removeNotifier is static it will send signals to all instances of
  //the model
    connect(&m_removeNotifier, SIGNAL(removed(QFileInfo)),
            this,              SIGNAL(removed(QFileInfo)));

    connect(&m_removeNotifier, SIGNAL(removed(QString)),
            this,              SIGNAL(removed(QString)));
}


FileSystemAction::~FileSystemAction()
{
   // m_mimeData using  static data because it used to crash
   // delete m_mimeData;
}

void FileSystemAction::remove(const QStringList &paths)
{  
    createAndProcessAction(ActionRemove, paths);
}


FileSystemAction::Action* FileSystemAction::createAction(ActionType type, int  origBase)
{
    Action * action = new Action();
    action->type         = type;
    action->baseOrigSize = origBase;
    action->totalItems   = 0;
    action->currItem     = 0;
    action->currEntry   = 0;
    action->totalBytes   = 0;
    action->bytesWritten = 0;

    return action;
}

void  FileSystemAction::addEntry(Action* action, const QString& pathname)
{
    QFileInfo info(pathname);
    if (!info.exists())
    {
        info.setFile(m_path, pathname);
    }
    if (!info.exists())
    {
        emit error(QObject::tr("File or Directory does not exist"),
                   pathname + QObject::tr(" does not exist")
                  );
        return;
    }
    ActionEntry * entry = new ActionEntry();
    QFileInfo   item;
    //ActionMove will perform a rename, so no Directory expanding is necessary
    if (action->type != ActionMove && info.isDir())
    {
        QDirIterator it(info.absoluteFilePath(),
                QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
                QDirIterator::Subdirectories);
        while (it.hasNext() &&  !it.next().isEmpty())
        {
            item = it.fileInfo();
            entry->reversedOrder.prepend(it.fileInfo());
            if (item.isFile() && action->type == ActionCopy)
            {
                action->totalBytes += info.size();
            }
        }
    }
    entry->reversedOrder.append(info);

    action->totalItems += entry->reversedOrder.count();
    if (info.isFile() && action->type == ActionCopy)
    {
        action->totalBytes += info.size();
    }
    entry->currItem  = 0;
    entry->currStep  = 0;
    action->entries.append(entry);
}


void FileSystemAction::processAction()
{   
    if (m_curAction)
    {
        //it will be ActionHardMoveRemove only when switched from ActionHardMoveCopy
        //in this case the move is done in two steps COPY and REMOVE
        if (m_curAction->type != ActionHardMoveCopy)
        {     
            delete m_curAction;
            m_curAction = 0;
        }
    }
    if (!m_curAction && m_queuedActions.count())
    {
        m_curAction = m_queuedActions.at(0);
        m_queuedActions.remove(0,1);
    }
    if (m_curAction)
    {
        m_busy = true;
        m_cancelCurrentAction = false;
        m_errorMsg.clear();
        m_errorTitle.clear();
        QTimer::singleShot(0, this, SLOT(processActionEntry()) );
        if (SHOULD_EMIT_PROGRESS_SIGNAL(m_curAction))
        {
            emit progress(0,m_curAction->totalBytes, 0);
        }
    }
    else
    {
         m_busy = false;
    }
}



void FileSystemAction::processActionEntry()
{
    ActionEntry * curEntry = static_cast<ActionEntry*>
            ( m_curAction->entries.at(m_curAction->currEntry) );

    if (!m_cancelCurrentAction)
    {
        switch(m_curAction->type)
        {
           case ActionRemove:
           case ActionHardMoveRemove:
                removeEntry(curEntry);
                break;
           case ActionCopy:
           case ActionHardMoveCopy:
                copyEntry(curEntry);
                break;
          case ActionMove:
                moveEntry(curEntry);
                break;
        }
    }
 // first of all check for any error or a cancel issued by the user
    if (m_cancelCurrentAction)
    {
        if (!m_errorTitle.isEmpty())
        {
            emit error(m_errorTitle, m_errorMsg);
        }
        //it may have other actions to do
        QTimer::singleShot(0, this, SLOT(processAction()));
        return;
    }
 // check if the current entry has finished
 // if so Views need to receive the notification about that
    if (curEntry->currItem == curEntry->reversedOrder.count())
    {
        const QFileInfo & mainItem = curEntry->reversedOrder.at(curEntry->currItem -1);
        if (m_curAction->type == ActionRemove || m_curAction->type == ActionMove ||
            m_curAction->type == ActionHardMoveRemove)
        {
            m_removeNotifier.notifyRemoved(mainItem); // notify all instances
        }
        if (m_curAction->type == ActionCopy || m_curAction->type == ActionMove ||
            m_curAction->type == ActionHardMoveCopy)
        {
            emit added( targetFom(mainItem.absoluteFilePath()) );
        }
        m_curAction->currEntry++;
        //check if is doing a hard move and the copy part has finished
        //if so switch the action to remove
        if (m_curAction->type == ActionHardMoveCopy &&
            m_curAction->currEntry == m_curAction->entries.count() )
        {
            m_curAction->type      = ActionHardMoveRemove;
            m_curAction->currEntry = 0;
            int entryCounter = m_curAction->entries.count();
            ActionEntry * entry;
            while (entryCounter--)
            {
                entry = m_curAction->entries.at(entryCounter);
                entry->currItem = 0;
                entry->currStep = 0;
            }
        }
    }
    if (curEntry->currStep == STEP_FILES)
    {
        curEntry->currStep = 0;
    }
 //work done
    if (SHOULD_EMIT_PROGRESS_SIGNAL(m_curAction))
    {
        int percent = (m_curAction->currItem * 100) / m_curAction->totalItems;
        if (percent > 100) percent = 100;
        emit progress(m_curAction->currItem, m_curAction->totalItems, percent);
    }
 //Check if the current action has finished or cancelled
    if (m_cancelCurrentAction || m_curAction->currItem == m_curAction->totalItems)
    {
        //it may have other actions to do
        QTimer::singleShot(0, this, SLOT(processAction()));
    }
    else
    {
        //keep working on current Action
        QTimer::singleShot(0, this, SLOT(processActionEntry()));
    }
}



void FileSystemAction::cancel()
{
    m_cancelCurrentAction = true;
}


void FileSystemAction::removeEntry(ActionEntry *entry)
{      
    QDir dir;
    //do one step at least
    for(; !m_cancelCurrentAction                          &&
          entry->currStep       < STEP_FILES              &&
          m_curAction->currItem < m_curAction->totalItems &&
          entry->currItem       < entry->reversedOrder.count()
        ; entry->currStep++,    m_curAction->currItem++, entry->currItem++
        )

    {
        const QFileInfo &fi = entry->reversedOrder.at(entry->currItem);
        if (fi.isDir())
        {
            m_cancelCurrentAction = !dir.rmdir(fi.absoluteFilePath());
        }
        else
        {
            m_cancelCurrentAction = !QFile::remove(fi.absoluteFilePath());
        }
        if (m_cancelCurrentAction)
        {
            m_errorTitle = QObject::tr("Could not remove the item ") +
                                       fi.absoluteFilePath();
            m_errorMsg   = ::strerror(errno);
        }
    }    
}


void  FileSystemAction::copyEntry(ActionEntry *entry)
{
    //do one step at least
    for(; !m_cancelCurrentAction                          &&
          entry->currStep       < STEP_FILES              &&
          m_curAction->currItem < m_curAction->totalItems &&
          entry->currItem       < entry->reversedOrder.count()
        ; entry->currStep++,    m_curAction->currItem++, entry->currItem++
        )

    {
        const QFileInfo &fi = entry->reversedOrder.at(entry->currItem);
        QString orig    = fi.absoluteFilePath();
        QString target = targetFom(orig);
        QString path(target);
        if (!fi.isDir())
        {
            QFileInfo  t(target);
            path = t.path();
        }
        QDir d(path);
        if (!d.exists() && !d.mkpath(path))
        {
            m_cancelCurrentAction = true;
            m_errorTitle = QObject::tr("Could not create the directory");
            m_errorMsg   = path;
            return;
        }
        if (!fi.isDir())
        {
            if (!QFile::copy(orig,target))
            {
                m_cancelCurrentAction = true;
                m_errorTitle = QObject::tr("Could not create the file ") + target;
                m_errorMsg   = ::strerror(errno);
            }
        }
    }//for
}


void FileSystemAction::moveEntry(ActionEntry *entry)
{
    QFile file;

    for(; !m_cancelCurrentAction                          &&
          entry->currStep       < STEP_FILES              &&
          m_curAction->currItem < m_curAction->totalItems &&
          entry->currItem       < entry->reversedOrder.count()
        ; entry->currStep++,    m_curAction->currItem++, entry->currItem++
        )

    {
        const QFileInfo &fi = entry->reversedOrder.at(entry->currItem);
        file.setFileName(fi.absoluteFilePath());
        QString target = targetFom(fi.absoluteFilePath());
        if (!file.rename(target))
        {
            m_cancelCurrentAction = true;
            m_errorTitle = QObject::tr("Could not move the directory/file ") + target;
            m_errorMsg   = ::strerror(errno);
        }
    }//for
}


void FileSystemAction::pathChanged(const QString &path)
{
    m_path = path;
}

void FileSystemAction::copy(const QStringList &pathnames)
{
    m_mimeData->setIntoClipboard(pathnames, m_path, false);
}


void FileSystemAction::cut(const QStringList &pathnames)
{
     m_mimeData->setIntoClipboard(pathnames, m_path, true);
}

void FileSystemAction::paste()
{
    const QMimeData *mime = DirModelMimeData::clipboardMimeData();
    QStringList paths;
    //it may have external urls
    if (mime && mime->hasUrls())
    {
        QList<QUrl> urls =  mime->urls();
        for (int counter=0; counter < urls.count(); counter++)
        {
            if (urls.at(counter).isLocalFile())
            {
                paths.append(urls.at(counter).toLocalFile());
            }
        }
    }
    if (paths.count() > 0)
    {   
       ActionType   actionType  = ActionCopy; // start with Copy and check for Cut
       if (DirModelMimeData::operation(mime) == DirModelMimeData::Cut)
       {
            if ( moveUsingSameFileSystem(paths.at(0)) ) {
                actionType = ActionMove;
            } else {
                actionType = ActionHardMoveCopy; // first step              
            }
       }
       createAndProcessAction(actionType, paths);
    }   
}


void  FileSystemAction::createAndProcessAction(ActionType actionType, const QStringList& paths)
{
    Action       *myAction       = 0;
    int           origPathLen    = 0;
    if (actionType != ActionRemove && actionType != ActionHardMoveRemove)
    {
        origPathLen = QFileInfo(paths.at(0)).absolutePath().length() ;
    }
    myAction = createAction(actionType, origPathLen);
    for (int counter=0; counter < paths.count(); counter++)
    {
        addEntry(myAction, paths.at(counter));
    }
    if (actionType == ActionHardMoveCopy)
    {
        myAction->totalItems *= 2; //duplicate this
    }
    m_queuedActions.append(myAction);
    if (!m_busy)
    {
        processAction();
    }
}

/*!
 * \brief FileSystemAction::targetFom() makes a destination full pathname from \a origItem
 * \param origItem full pathname from a item intended to be copied or moved into current path
 * \return full pathname of target
 */
QString FileSystemAction::targetFom(const QString& origItem)
{
    QString target(m_path + origItem.mid(m_curAction->baseOrigSize));
    return target;
}

/*!
 * \brief FileSystemAction::moveUsingSameFileSystem() Checks if the item begin moved to current m_path
 *        belongs to the same File System
 *
 * \param itemToMovePathname  first item being moved from a paste operation
 *
 * \return true if the item being moved to the current m_path belongs to the same file system as m_path
 */
bool FileSystemAction::moveUsingSameFileSystem(const QString& itemToMovePathname)
{
    unsigned long targetFsId = 0xffff;
    unsigned long originFsId = 0xfffe;
    struct statvfs  vfs;
    if ( ::statvfs(m_path.toLatin1().constData(), &vfs) == 0 )
    {
        targetFsId = vfs.f_fsid;
    }
    if ( ::statvfs(itemToMovePathname.toLatin1().constData(), &vfs) == 0)
    {
        originFsId = vfs.f_fsid;
    }
    return targetFsId == originFsId;
}

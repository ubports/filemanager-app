/**************************************************************************
 *
 * Copyright 2013 Canonical Ltd.
 * Copyright 2013 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
#include <QThread>

#define  STEP_FILES               5  // number of the files to work on a step, when this number is reached a signal is emited

#define SHOULD_EMIT_PROGRESS_SIGNAL(action)       (1)



RemoveNotifier   FileSystemAction::m_removeNotifier;

static  QLatin1String GNOME_COPIED_MIME_TYPE  ("x-special/gnome-copied-files");
static  QLatin1String KDE_CUT_MIME_TYPE       ("application/x-kde-cutselection");


class DirModelMimeData : public QMimeData
{
public:   
    explicit DirModelMimeData();
    virtual QStringList     formats() const    { return m_formats; }
public:    
    ClipboardOperation      clipBoardOperation();
    void                    setIntoClipboard(const QStringList& files, const QString &path, ClipboardOperation operation);
    static const QMimeData *clipboardMimeData();
    QStringList             localUrls();
private:
    static QList<QUrl>      gnomeUrls(const QMimeData *mime, ClipboardOperation& operation);
private:
    QStringList              m_formats;
    const QMimeData *        m_appMime;
};

//===============================================================================================
/*!
 * \brief DirModelMimeData::DirModelMimeData
 */
DirModelMimeData::DirModelMimeData() :
    QMimeData() 
{
    m_formats.append("text/uri-list");
    m_formats.append(GNOME_COPIED_MIME_TYPE);
    m_formats.append("text/plain");
    m_formats.append("COMPOUND_TEXT");
    m_formats.append("TARGETS");
    m_formats.append("MULTIPLE");
    m_formats.append("TIMESTAMP");
    m_formats.append("SAVE_TARGETS");

    m_appMime = 0;
}


//===============================================================================================
/*!
 * \brief DirModelMimeData::gnomeUrls
 * \param mime
 * \param operation
 * \return
 */
QList<QUrl>
DirModelMimeData::gnomeUrls(const QMimeData * mime,
                            ClipboardOperation& operation)
{
    QList<QUrl>  urls;
    if (mime->hasFormat(GNOME_COPIED_MIME_TYPE))
    {
       QByteArray  bytes = mime->data(GNOME_COPIED_MIME_TYPE);
       QList<QString>  d = QString(bytes).split(QLatin1String("\r\n"),
                                                QString::SkipEmptyParts);
       for (int counter= 0; counter < d.count(); counter++)
       {
           if (counter==0)
           {
               QStringList couple = d.at(0).split(QLatin1Char('\n'));
               urls.append(couple[1]);
               if (couple[0] == QLatin1String("cut"))  {
                   operation = ClipboardCut;
               }
               else  {
                   operation = ClipboardCopy;
               }
           }
           else {
               urls.append(d.at(counter));
           }
       }
    }
    return urls;
}

//===============================================================================================
/*!
 * \brief DirModelMimeData::clipBoardOperation()
 * \param mime
 * \return
 */
ClipboardOperation DirModelMimeData::clipBoardOperation()
{
    ClipboardOperation op = ClipboardCopy;
    m_appMime = clipboardMimeData();
    if (m_appMime)
    {
       //first check for GNOME clipboard format, op comes with Copy/Cut
        if (gnomeUrls(m_appMime, op).count() == 0)
        { // there is no gnome format, tries KDE format
            QStringList formats = m_appMime->formats();
            int f = formats.count();
            while(f--)
            {
                const QString &mi = formats.at(f);
                if(mi.startsWith("application/x-kde") )
                {
                    if (mi.contains("cut"))
                    {
                        op = ClipboardCut;
                        break;
                    }
                }
            }
        }
    }
    return op;
}


//===============================================================================================
/*!
 * \brief DirModelMimeData::setIntoClipboard
 * \param files
 * \param path
 * \param isCut
 */
void DirModelMimeData::setIntoClipboard(const QStringList &files, const QString& path, ClipboardOperation operation)
{   
    int index = m_formats.indexOf(KDE_CUT_MIME_TYPE);
    if (index != -1 && operation != ClipboardCut)
    {
        m_formats.removeAt(index);
    }
    else
    if (operation == ClipboardCut)  {
        m_formats.append(KDE_CUT_MIME_TYPE);
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
        QByteArray gnomeData;
        gnomeData += operation == ClipboardCut ? "cut\n" : "copy\n";
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
                QUrl item = QUrl::fromLocalFile(fi.canonicalFilePath());
                urls.append(item);
                gnomeData += item.toEncoded() + "\r\n";
            }
            else
            {                
                //emit error( QObject::tr("Item does not exist"), item);
            }
        }
        if (urls.count() > 0)
        {          
            setData(GNOME_COPIED_MIME_TYPE, gnomeData);
            setUrls(urls);
            clipboard->setMimeData(this);
        }
    }
}

//===============================================================================================
/*!
 * \brief DirModelMimeData::clipboardMimeData
 * \return
 */
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

//===============================================================================================
/*!
 * \brief DirModelMimeData::localUrls
 * \return
 */
QStringList
DirModelMimeData::localUrls()
{
     m_appMime = DirModelMimeData::clipboardMimeData();
     QStringList paths;
     //it may have external urls
     if (m_appMime)
     {
         QList<QUrl> urls;
         if (m_appMime->hasUrls())
         {
             urls =  m_appMime->urls();
         }
         else
         {
             ClipboardOperation op;
             urls = gnomeUrls(m_appMime, op);
         }
         for (int counter=0; counter < urls.count(); counter++)
         {
             if (urls.at(counter).isLocalFile())
             {
                 paths.append(urls.at(counter).toLocalFile());
             }
         }
     }
     return paths;
}

/*!
 * \brief RemoveNotifier::RemoveNotifier
 * \param parent
 */
RemoveNotifier::RemoveNotifier(QObject *parent) :
    QObject(parent)
{
}

//===============================================================================================
/*!
 * \brief RemoveNotifier::notifyRemoved
 * \param fi
 */
void RemoveNotifier::notifyRemoved(const QFileInfo &fi)
{
    emit removed(fi);
}

//===============================================================================================
/*!
 * \brief RemoveNotifier::notifyRemoved
 * \param item
 */
void RemoveNotifier::notifyRemoved(const QString &item)
{
    emit removed(item);
}


/* static QMimeData */
DirModelMimeData     mimeData;


//===============================================================================================
/*!
 * \brief FileSystemAction::FileSystemAction
 * \param parent
 */
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

    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SIGNAL(clipboardChanged()));
}

//===============================================================================================
/*!
 * \brief FileSystemAction::~FileSystemAction
 */
FileSystemAction::~FileSystemAction()
{
   // m_mimeData using  static data because it used to crash
   // delete m_mimeData;
}

//===============================================================================================
/*!
 * \brief FileSystemAction::remove
 * \param paths
 */
void FileSystemAction::remove(const QStringList &paths)
{  
    createAndProcessAction(ActionRemove, paths, NoClipboard);
}

//===============================================================================================
/*!
 * \brief FileSystemAction::createAction
 * \param type
 * \param origBase
 * \return
 */
FileSystemAction::Action* FileSystemAction::createAction(ActionType type, int  origBase)
{
    Action * action = new Action();
    action->type         = type;
    action->baseOrigSize = origBase;
    action->targetPath   = m_path;
    action->totalItems   = 0;
    action->currItem     = 0;
    action->currEntry    = 0;
    action->totalBytes   = 0;
    action->bytesWritten = 0;

    return action;
}

//===============================================================================================
/*!
 * \brief FileSystemAction::addEntry
 * \param action
 * \param pathname
 */
void  FileSystemAction::addEntry(Action* action, const QString& pathname)
{
    QFileInfo info(pathname);
    if (!info.exists())
    {
        info.setFile(action->targetPath, pathname);
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
                        QDir::AllEntries | QDir::System |
                              QDir::NoDotAndDotDot | QDir::Hidden,
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

//===============================================================================================
/*!
 * \brief FileSystemAction::processAction
 */
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
            emit progress(0,m_curAction->totalItems, 0);
        }
    }
    else
    {
         m_busy = false;
    }
}


//===============================================================================================
/*!
 * \brief FileSystemAction::processActionEntry
 */
void FileSystemAction::processActionEntry()
{
    ActionEntry * curEntry = static_cast<ActionEntry*>
            ( m_curAction->entries.at(m_curAction->currEntry) );

#if defined(SIMULATE_LONG_ACTION)
    {
        unsigned int delay = SIMULATE_LONG_ACTION;
        if (delay == 1)
        {
            delay = 100;           //each (10 * STEP_FILES) files will waits a second
            QThread::msleep(delay);
        }
    }
#endif
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
        if (percent >= 100)
        {
            percent = 100;
            endCurrentAction();
        }
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


//===============================================================================================
/*!
 * \brief FileSystemAction::cancel
 */
void FileSystemAction::cancel()
{
    m_cancelCurrentAction = true;
}

//===============================================================================================
/*!
 * \brief FileSystemAction::removeEntry
 * \param entry
 */
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
        if (fi.isDir() && !fi.isSymLink())
        {
            m_cancelCurrentAction = !dir.rmdir(fi.absoluteFilePath());
        }
        else
        {
            m_cancelCurrentAction = !QFile::remove(fi.absoluteFilePath());
        }
#if DEBUG_REMOVE
        qDebug() << "remove ret=" << !m_cancelCurrentAction << fi.absoluteFilePath();
#endif
        if (m_cancelCurrentAction)
        {
            m_errorTitle = QObject::tr("Could not remove the item ") +
                                       fi.absoluteFilePath();
            m_errorMsg   = ::strerror(errno);
        }
    }    
}

//===============================================================================================
/*!
 * \brief FileSystemAction::copyEntry
 * \param entry
 */
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

//===============================================================================================
/*!
 * \brief FileSystemAction::moveEntry
 * \param entry
 */
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

//===============================================================================================
/*!
 * \brief FileSystemAction::pathChanged
 * \param path
 */
void FileSystemAction::pathChanged(const QString &path)
{
    m_path = path;
}

//===============================================================================================
/*!
 * \brief FileSystemAction::copy
 * \param pathnames
 */
void FileSystemAction::copy(const QStringList &pathnames)
{
    m_mimeData->setIntoClipboard(pathnames, m_path, ClipboardCopy);
}

//===============================================================================================
/*!
 * \brief FileSystemAction::cut
 * \param pathnames
 */
void FileSystemAction::cut(const QStringList &pathnames)
{
     m_mimeData->setIntoClipboard(pathnames, m_path, ClipboardCut);
}

//===============================================================================================
/*!
 * \brief FileSystemAction::paste
 */
void FileSystemAction::paste()
{
    QStringList paths     = m_mimeData->localUrls();
    if (paths.count())
    {         
       ActionType actionType  = ActionCopy; // start with Copy and check for Cut
       ClipboardOperation  operation = m_mimeData->clipBoardOperation();
       if (operation == ClipboardCut)
       {
            if ( moveUsingSameFileSystem(paths.at(0)) ) {
                actionType = ActionMove;
            } else {
                actionType = ActionHardMoveCopy; // first step              
            }
       }
       createAndProcessAction(actionType, paths, operation);
    }   
}

//===============================================================================================
/*!
 * \brief FileSystemAction::createAndProcessAction
 * \param actionType
 * \param paths
 * \param operation
 */
void  FileSystemAction::createAndProcessAction(ActionType actionType, const QStringList& paths, ClipboardOperation operation)
{
    Action       *myAction       = 0;
    int           origPathLen    = 0;   
    myAction                     = createAction(actionType, origPathLen);
    myAction->operation          = operation;
    myAction->origPath           = QFileInfo(paths.at(0)).absolutePath();
    myAction->baseOrigSize       = myAction->origPath.length();
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

//===============================================================================================
/*!
 * \brief FileSystemAction::targetFom() makes a destination full pathname from \a origItem
 * \param origItem full pathname from a item intended to be copied or moved into current path
 * \return full pathname of target
 */
QString FileSystemAction::targetFom(const QString& origItem)
{
    QString target(m_curAction->targetPath + origItem.mid(m_curAction->baseOrigSize));
    return target;
}

//===============================================================================================
/*!
 * \brief FileSystemAction::moveUsingSameFileSystem() Checks if the item being moved to
 *   current m_path belongs to the same File System
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

//=======================================================
/*!
 * \brief FileSystemAction::clipboardLocalUrlsConunter
 * \return
 */
int FileSystemAction::clipboardLocalUrlsConunter()
{
    return m_mimeData->localUrls().count();
}

//================================================================================
/*!
 * \brief FileSystemAction::endCurrentAction() finishes an Action
 *
 *  If a Paste was made from a Cut operation, items pasted become avaialable in the clipboard
 *   as from Copy source operation, so items can be now Pasted again, but with no source removal
 */
void FileSystemAction::endCurrentAction()
{
    if ( m_curAction->origPath != m_curAction->targetPath &&
         m_curAction->operation == ClipboardCut)
    {
         QStringList items;
         const ActionEntry *entry;
         int   last;
         for(int e = 0; e < m_curAction->entries.count(); e++)
         {
             entry   = m_curAction->entries.at(e);
             last    = entry->reversedOrder.count() -1;
             QString item(targetFom(entry->reversedOrder.at(last).absoluteFilePath()));
             items.append(item);
         }
         if (items.count())
         {
             QString targetPath = m_curAction->targetPath;
             m_mimeData->setIntoClipboard(items, targetPath, ClipboardCopy);
         }
    }
}

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

#include <QDirIterator>
#include <QDebug>
#include <QTimer>

#define  STEP_FILES               10  // number of the files to work on a step, when this number is reached a signal is emited

#define SHOULD_EMIT_PROGRESS_SIGNAL(action)       (1)


RemoveNotifier   FileSystemAction::m_removeNotifier;


FileSystemAction::FileSystemAction(QObject *parent) :
    QObject(parent)
  , m_curAction(0)
  , m_cancelCurrentAction(false)
  , m_busy(false)
{
  //as m_removeNotifier is static it will send signals to all instances of
  //the model
    connect(&m_removeNotifier, SIGNAL(removed(QFileInfo)),
            this,              SIGNAL(removed(QFileInfo)));

    connect(&m_removeNotifier, SIGNAL(removed(QString)),
            this,              SIGNAL(removed(QString)));
}


void FileSystemAction::remove(const QStringList &paths)
{  
    Action* myAction = createAction(ActionRemove);
    for (int counter=0; counter < paths.count(); counter++)
    {       
        addEntry(myAction, paths.at(counter));
    }
    m_queuedActions.append(myAction);
    if (!m_busy)
    {
        processAction();
    }
    else
    {
        //processAction() will called automatically
    }
}


FileSystemAction::Action* FileSystemAction::createAction(ActionType type, const QString& target)
{
    Action * action = new Action();
    action->type         = type;
    action->target       = target;
    action->totalItems   = 0;
    action->currItem     = 0;
    action->currtEntry   = 0;
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
    if (info.isDir())
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
    m_busy = false;
    if (m_curAction)
    {
        delete m_curAction;
        m_curAction = 0;
    }
    if (m_queuedActions.count())
    {
        m_busy = true;
        m_cancelCurrentAction = false;
        m_curAction = m_queuedActions.at(0);
        m_queuedActions.remove(0,1);
        QTimer::singleShot(0, this, SLOT(processActionEntry()) );
        if (SHOULD_EMIT_PROGRESS_SIGNAL(m_curAction))
        {
            emit progress(0,m_curAction->totalBytes, 0);
        }
    }
}



void FileSystemAction::processActionEntry()
{
    ActionEntry * curEntry = static_cast<ActionEntry*> ( m_curAction->entries.at(m_curAction->currtEntry) );

  //do one step at least
    for(; !m_cancelCurrentAction                          &&
          curEntry->currStep    < STEP_FILES              &&
          m_curAction->currItem < m_curAction->totalItems &&
          curEntry->currItem    < curEntry->reversedOrder.count()
        ; curEntry->currStep++,  m_curAction->currItem++, curEntry->currItem++
       )

    {
         doCurrentEntry(curEntry);
    }

  // check if the current entry has finished
  // if so Views need to receive the notification about that
    if (curEntry->currItem == curEntry->reversedOrder.count())
    {
        const QFileInfo & mainItem = curEntry->reversedOrder.at(curEntry->currItem -1);
        if (m_curAction->type == ActionRemove || m_curAction->type == ActionMove)
        {
            m_removeNotifier.notifyRemoved(mainItem); // notify all instances
        }
        else
        {
            emit added(mainItem);
        }
        m_curAction->currtEntry++;
    }

    if (curEntry->currStep == STEP_FILES)
    {
        curEntry->currStep = 0;
    }

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


void FileSystemAction::doCurrentEntry(ActionEntry *entry)
{
    const QFileInfo &fi = entry->reversedOrder.at(entry->currItem);
    bool ok = false;
    if (m_curAction->type == ActionRemove)
    {
        if (fi.isDir())
        {
            ok = QDir().rmdir(fi.absoluteFilePath());
        }
        else
        {
            ok = QFile::remove(fi.absoluteFilePath());
        }
    }    
}


void FileSystemAction::pathChanged(const QString &path)
{
    m_path = path;
}

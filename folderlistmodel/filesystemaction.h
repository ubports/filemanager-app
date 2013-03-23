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
 * File: filesystemaction.h
 * Date: 3/13/2013
 */

#ifndef FILESYSTEMACTION_H
#define FILESYSTEMACTION_H

//#include "removenotifier.h"

#include <QObject>
#include <QFileInfo>
#include <QVector>

class DirModelMimeData;
class RemoveNotifier;

/*!
 * \brief The FileSystemAction class
 *
 *
 */

class FileSystemAction : public QObject
{
    Q_OBJECT
public:
    explicit FileSystemAction(QObject *parent = 0);
    ~FileSystemAction();
public slots:
    void     cancel();
    void     remove(const QStringList & filePaths);

signals:
    void     error(const QString& errorTitle, const QString &errorMessage);
    void     removed(const QString& item);   //must be sent to all Model instances
    void     removed(const QFileInfo&);
    void     added(const QString& );
    void     added(const QFileInfo& );
    void     progress(int curItem, int totalItems, int percent);

public slots:
    void     pathChanged(const QString& path);
    void     paste();
    void     cut(const QStringList& );
    void     copy(const QStringList&);

private slots:
    void     processAction();
    void     processActionEntry();   

#if defined(REGRESSION_TEST_FOLDERLISTMODEL) //used in Unit/Regression tests
 public:
#else
 private:
#endif
   enum ActionType
   {
       ActionRemove,
       ActionCopy,
       ActionMove,
       ActionHardMoveCopy,
       ActionHardMoveRemove
   };
   void     createAndProcessAction(ActionType actionType, const QStringList& paths);

private:
   /*!
       An ActionEntry represents a high level item as a File or a Directory which an Action is required

       For directories \a reversedOrder keeps all children
    */
   struct ActionEntry
   {
     public:
       ~ActionEntry()
       { reversedOrder.clear(); }
       QList<QFileInfo>   reversedOrder;   //!< last item must be the item from the list
       int                currStep;
       int                currItem;
   };

   struct Action
   {
    public:
       ~Action()           {qDeleteAll(entries); entries.clear();}
       ActionType          type;
       QList<ActionEntry*> entries;
       int                 totalItems;
       int                 currItem;
       int                 baseOrigSize;
       quint64             totalBytes;
       quint64             bytesWritten;
       int                 currEntry;
   };

   QVector<Action*>        m_queuedActions;  //!< work always at item 0, after finishing taking item 0 out
   Action            *     m_curAction;
   bool                    m_cancelCurrentAction;
   bool                    m_busy;
   static RemoveNotifier   m_removeNotifier;
   QString                 m_path;
   DirModelMimeData  *     m_mimeData;
   QString                 m_errorTitle;
   QString                 m_errorMsg;

private:  
   Action * createAction(ActionType, int origBase = 0);
   void     addEntry(Action* action, const QString &pathname);
   void     removeEntry(ActionEntry *);
   void     copyEntry(ActionEntry *);
   void     moveEntry(ActionEntry *entry);
   bool     moveUsingSameFileSystem(const QString& itemToMovePathname);
   QString  targetFom(const QString& origItem);
};
    

/*!
 * \brief The RemoveNotifier is a utility class for \ref FileSystemAction to send
 *         notifications about removed files/dir
 *
 *  This class must have a unique instance to notify all instances of \ref FileSystemAction and \ref DirModel
 */
class RemoveNotifier : public QObject
{
    Q_OBJECT

    friend class FileSystemAction;
private:
    explicit RemoveNotifier(QObject *parent = 0);
    void notifyRemoved(const QString& item);
    void notifyRemoved(const QFileInfo& fi);

signals:
    void     removed(const QString& item);
    void     removed(const QFileInfo&);
};

#endif // FILESYSTEMACTION_H

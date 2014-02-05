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
 * File: dirselection.h
 * Date: 29/01/2014
 */

#ifndef DIRSELECTION_H
#define DIRSELECTION_H

#include "diriteminfo.h"

#include <QObject>
#include <QStringList>


class DirItemAbstractListModel;

class DirSelection : public QObject
{
    Q_OBJECT
public:
    explicit DirSelection(DirItemAbstractListModel *parent,  DirItemInfoList *listItems);
    explicit DirSelection(QObject *parent = 0);
public slots:
        void        selectAll();
        void        clear();
        void        toggle(const QString &name);
        void        toggleIndex(int index);
        void        set(const QString& name, bool selected);
        void        setIndex(int index, bool selected);

public:
        Q_ENUMS(Mode)
        enum Mode
        {
            Single,
            Multi   //<! this is the default
        };
        Q_PROPERTY(int counter   READ counter   NOTIFY selectionChanged)
        Q_PROPERTY(Mode mode  READ mode WRITE setMode NOTIFY modeChanged)
        Q_INVOKABLE  QStringList selectedNames()      const;
        Q_INVOKABLE  void        setMode(Mode m);
        Q_INVOKABLE  QStringList selectedAbsFilePaths()  const;   //full path
        Q_INVOKABLE  QList<int>  selectedIndexes()    const;
        int                      counter()            const;
        Mode                     mode()               const;

public:
        void        itemGoingToBeRemoved(int index);

signals:
        void        selectionChanged(int);
        void        modeChanged(int);

private:
        int                        m_selectedCounter;
        DirItemAbstractListModel*  m_model;
        DirItemInfoList *          m_listItems;
        Mode                       m_mode;
};

#endif // DIRSELECTION_H

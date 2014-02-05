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
 * File: dirselection.cpp
 * Date: 29/01/2014
 */

#include "dirselection.h"
#include "diritemabstractlistmodel.h"
#include <QTimer>
#include <QDebug>


#define  VALID_INDEX(index)   (index >= 0 && index < m_model->rowCount())

DirSelection::DirSelection(QObject *parent) :  QObject(parent)
{
}

DirSelection::DirSelection(DirItemAbstractListModel *parent, DirItemInfoList *listItems) :
    QObject(parent)
   ,m_selectedCounter(0)
   ,m_model(parent)
   ,m_listItems(listItems)
   ,m_mode(Multi)
{
}



QStringList DirSelection::selectedAbsFilePaths() const
{
    QStringList ret;
    int counter = m_model->rowCount();
    for(int index = 0 ; index < counter; ++index)
    {
        if (m_listItems->at(index).isSelected())
        {
            ret.append(m_listItems->at(index).absoluteFilePath());
        }
    }
    return ret;
}

QStringList DirSelection::selectedNames() const
{
    QStringList ret;
    int counter = m_model->rowCount();
    for(int index = 0 ; index < counter; ++index)
    {
        if (m_listItems->at(index).isSelected())
        {
            ret.append(m_listItems->at(index).fileName());
        }
    }
    return ret;
}



QList<int>  DirSelection::selectedIndexes()    const
{
    QList<int> ret;
    int counter = m_model->rowCount();
    for(int index = 0 ; index < counter; ++index)
    {
        if (m_listItems->at(index).isSelected())
        {
            ret.append(index);
        }
    }
    return ret;
}


void DirSelection::clear()
{
    bool notify = m_selectedCounter != 0;
    int counter = m_model->rowCount();
    DirItemInfo *data =  m_listItems->data();
    while (m_selectedCounter > 0  && counter-- )
    {
        if ( data[counter].setSelection(false) )
        {
           --m_selectedCounter;
           m_model->notifyItemChanged(counter);
        }        
    }
    //force it to zero, works when cleaning the buffer first
    m_selectedCounter = 0;
    if (notify)
    {
      emit selectionChanged(m_selectedCounter);
    }
}


void DirSelection::selectAll()
{
    bool notify = m_selectedCounter != m_model->rowCount();
    int counter = m_model->rowCount();
    DirItemInfo *data =  m_listItems->data();
    while ( counter-- )
    {
        if ( data[counter].setSelection(true) )
        {
           ++m_selectedCounter;
           m_model->notifyItemChanged(counter);
        }
    }
    if (notify)
    {
      emit selectionChanged(m_selectedCounter);
    }
}


int DirSelection::counter() const
{
    return m_selectedCounter;
}


DirSelection::Mode DirSelection::mode() const
{
    return m_mode;
}

void DirSelection::itemGoingToBeRemoved(int index)
{
    if (VALID_INDEX(index))
    {
        if (m_selectedCounter > 0 && m_listItems->at(index).isSelected())
        {
           emit selectionChanged(--m_selectedCounter);
        }
        // item is going to be removed, no signal is necessary
    }
}


void DirSelection::setIndex(int index, bool selected)
{
     if (VALID_INDEX(index))
     {
         if (selected && m_mode == Single &&
             m_selectedCounter > 0
            )
         {
             clear();
         }
         DirItemInfo *data  = m_listItems->data();
         if (data[index].setSelection(selected))
         {
             m_model->notifyItemChanged(index);
             if (selected) ++m_selectedCounter;
             else          --m_selectedCounter;
             emit selectionChanged(m_selectedCounter);
         }
     }
}


void DirSelection::toggleIndex(int index)
{
    if (VALID_INDEX(index))
    {
        setIndex(index, !m_listItems->at(index).isSelected());
    }
}


void DirSelection::set(const QString &name, bool selected)
{
    return setIndex(m_model->getIndex(name), selected);
}


void DirSelection::toggle(const QString &name)
{
    return toggleIndex(m_model->getIndex(name));
}


void DirSelection::setMode(Mode m)
{
    if (m != m_mode)
    {
        m_mode = m;
        emit modeChanged(m_mode);
    }
}

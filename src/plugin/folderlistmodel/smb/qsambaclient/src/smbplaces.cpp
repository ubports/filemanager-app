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
 * File: smbplaces.cpp
 * Date: 27/12/2014
 */

#include "smbplaces.h"
#include "smbutil.h"
#include <QThread>


#define SECOND_IN_MILLISECONDS 60000

/*!
 * \brief The SmbPlacesThread class gets the list of shares on a secondary thread
 */
class SmbPlacesThread : public QThread
{
public:
  SmbPlacesThread(QObject *parent = 0): QThread(parent)
  {}
  inline QStringList getShareList() { return shares; }
protected:
   virtual void run()
   {
      SmbUtil  smb;
      shares  = smb.lisShares();
   }

private:
   QStringList shares;
};



//===============================================================================================
/*!
 * \brief SmbPlaces::SmbPlaces
 * \param parent
 */
SmbPlaces::SmbPlaces(QObject *parent) :
    QObject(parent)
   ,m_thread(0)
{

}


//===============================================================================================
/*!
 * \brief SmbPlaces::~SmbPlaces
 */
SmbPlaces::~SmbPlaces()
{
    if (m_thread)
    {
        m_thread->quit();                                 // finish the thead, the information is no longer necessary
        m_thread->wait( 3600 * SECOND_IN_MILLISECONDS );  // wait up to an hour
        delete m_thread;
    }
}

//===============================================================================================
/*!
 * \brief SmbPlaces::listPlacesSync()
 * \return the new list of the shares
 */
QStringList SmbPlaces::listPlacesSync()
{
    SmbUtil  smb; 
    m_sharesList = smb.lisShares();  
    return m_sharesList;
}

//===============================================================================================
/*!
 * \brief SmbPlaces::listPlacesAsync()
 *
 * Creates a SmbPlacesThread object (the secondary thread) to get the shares list
 */
void SmbPlaces::listPlacesAsync()
{
    if (m_thread == 0)
    {
       m_thread = new SmbPlacesThread(this);
       connect(m_thread, SIGNAL(finished()),
               this,     SLOT(onSmbPlacesThreadFinished()));
    }
    if (!m_thread->isRunning())
    {
      m_thread->start();
    }
}


//===============================================================================================
/*!
 * \brief SmbPlaces::gePlaces()
 * \return the current list of shares
 */
QStringList SmbPlaces::gePlaces() const
{
    return m_sharesList;
}


//===============================================================================================
/*!
 * \brief SmbPlaces::onSmbPlacesThreadFinished
 *
 *  called when SmbPlacesThread thread finishes, SmbPlacesThread is deleted
 */
void SmbPlaces::onSmbPlacesThreadFinished()
{
   m_sharesList = m_thread->getShareList();
   m_thread->deleteLater();
   m_thread = 0;
   emit sharesList(m_sharesList);
}

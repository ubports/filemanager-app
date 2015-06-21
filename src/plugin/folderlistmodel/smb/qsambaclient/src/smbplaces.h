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
 * File: smbplaces.h
 * Date: 27/12/2014
 */

#ifndef SMBPLACES_H
#define SMBPLACES_H

#include <QObject>
#include <QStringList>

class SmbPlacesThread;


/*!
 * \brief The SmbPlaces class gets the list of current Samba/CIFS shares
 */

class SmbPlaces : public QObject
{
    Q_OBJECT
public:
    explicit SmbPlaces(QObject *parent = 0);
    ~SmbPlaces();

public:
    /*!
     *  list all Samba/Cifs shares in sync mode, that means it may block any active UI
     *
     *  \return the list of the shares in the current network
     */
    Q_INVOKABLE   QStringList listPlacesSync();

    Q_INVOKABLE   QStringList gePlaces() const;

public slots:
    /*!
     *  list all Samba/Cifs shares in async mode, the job is made on a secondary thread
     *  that means it does not block any active UI
     *
     *  After the job is done the signal \ref sharesList is emitted within the current shares  list
     */
    void        listPlacesAsync();

signals:  
    void        sharesList(QStringList);

private slots:
    void        onSmbPlacesThreadFinished();

private:
    QStringList       m_sharesList;
    SmbPlacesThread * m_thread;

#if defined(REGRESSION_TEST_QSAMBACLIENT)
    friend class TestQSambaSuite;
#endif

};

#endif // SMBPLACES_H

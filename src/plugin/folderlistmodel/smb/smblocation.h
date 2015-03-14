/**************************************************************************
 *
 * Copyright 2015 Canonical Ltd.
 * Copyright 2015 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: smblocation.h
 * Date: 17/01/2015
 */

#ifndef SMBLOCATION_H
#define SMBLOCATION_H

#include "location.h"
#include "smblocationauthentication.h"
#include "smbobject.h"


class SmbLocation : public Location, public SmbLocationAuthentication
{
    Q_OBJECT
public:
    explicit SmbLocation(int type, QObject *parent=0);
    ~SmbLocation();

public:   
    virtual DirItemInfo * newItemInfo(const QString& urlPath);
    virtual DirListWorker * newListWorker(const QString &urlPath,
                                          QDir::Filter filter,
                                          const bool isRecursive);
    virtual QString     currentAuthenticationUser();
    virtual QString     currentAuthenticationPassword();

public slots:   
    virtual void setAuthentication(const QString& user,
                                   const QString& password);


private:
   SmbUtil_Ptr m_smb;
};

#endif // SMBLOCATION_H

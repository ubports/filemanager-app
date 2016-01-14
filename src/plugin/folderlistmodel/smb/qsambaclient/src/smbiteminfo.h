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
 * File: smbiteminfo.h
 * Date: 08/12/2014
 */

#ifndef SMBITEMINFO_H
#define SMBITEMINFO_H

#include "urliteminfo.h"
#include "smbobject.h"

class QUrl;

class SmbItemInfo : public UrlItemInfo, public SmbObject
{
public:
    /*!
     * \brief SmbItemInfo
     * \param urlPath  a url
     * \param smb   an instance of \ref SmbUtil that has an authentication callback already set
     */
    SmbItemInfo(const QString& urlPath, Const_SmbUtil_Ptr  smb  = 0);
    SmbItemInfo();
    ~SmbItemInfo();

public:
    QString          sharePath() const;   
    virtual void     setFile(const QString &dir, const QString & file);
    virtual void     setFile(const QString &smb_path);

protected:    
     void            setInfo(const QString &smb_path);
};

#endif // SMBITEMINFO_H

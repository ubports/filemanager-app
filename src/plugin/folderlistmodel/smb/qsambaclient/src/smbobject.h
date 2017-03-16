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
 * File: smbobject.h
 * Date: 02/01/2015
 */

#ifndef SMBOBJECT_H
#define SMBOBJECT_H

#include "cleanurl.h"

#include <QString>

class SmbUtil;
typedef SmbUtil const * Const_SmbUtil_Ptr;
typedef SmbUtil       * SmbUtil_Ptr;

class SmbObject : public CleanUrl
{
protected:
    SmbObject(const QString &urlPath, Const_SmbUtil_Ptr smb = 0);
    SmbUtil_Ptr     smbObj() const;
public:
    virtual ~SmbObject();
protected:
    Const_SmbUtil_Ptr   m_smb;
    SmbUtil_Ptr         m_smbOwnInstance; //!<  not zero only if smb == 0
};

#endif // SMBOBJECT_H

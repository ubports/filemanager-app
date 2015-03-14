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
 * File: smbobject.cpp
 * Date: 02/01/2015
 */

#include "smbobject.h"
#include "smbutil.h"

#include <QUrl>

SmbObject::SmbObject(const QString &urlPath, Const_SmbUtil_Ptr smb)
  : CleanUrl(urlPath), m_smb(smb), m_smbOwnInstance(0)
{
    if (m_smb == 0)
    {
        m_smb = m_smbOwnInstance = new SmbUtil(cleanUrl());
    }    
}


SmbObject::~SmbObject()
{
    if (m_smbOwnInstance != 0)
    {
        delete m_smbOwnInstance;
        m_smbOwnInstance = 0;
    }
}


SmbUtil_Ptr SmbObject::smbObj() const
{
    return const_cast<SmbUtil_Ptr>(m_smb);
}

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
 * File: smblistworker.h
 * Date: 17/01/2015
 */

#ifndef SMBLISTWORKER_H
#define SMBLISTWORKER_H

#include "iorequestworker.h"
#include "smbiteminfo.h"

class SmbListWorker : public DirListWorker, public SmbObject
{
    Q_OBJECT
public:
    explicit SmbListWorker(const QString &pathName, QDir::Filter filter, const bool isRecursive, bool parentIsHost, Const_SmbUtil_Ptr smb);

private:
    DirItemInfoList getNetworkContent();
private:
    bool               m_parentIsHost;
};

#endif // SMBLISTWORKER_H

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
 * File: smbutil.h
 * Date: 20/11/2014
 */

#ifndef SMBUTIL_H
#define SMBUTIL_H

#include <libsmbclient.h>

#include <QStringList>
#include <QDir>

class QUrl;
class NetAuthenticationData;
class NetAuthenticationDataList;
struct stat;


namespace Smb
{
 typedef SMBCCTX     * Context;
 typedef SMBCFILE    * FileHandler; 
 typedef void (*AuthenticationFunction) (const char *server,
                                         const char *share,
                                         char *wrkgrp,
                                         int wrkgrplen,
                                         char *user,
                                         int userlen,
                                         char *passwd,
                                         int passwdlen);
}


/*!
 * \brief The SmbUtil class provides the interface through the libsmbclient functions
 *
 * Some documentation can found at:
 * \link  http://www.samba.org/samba/docs/man/manpages-3/libsmbclient.7.html
 * \link  https://github.com/Zentyal/samba/tree/master/examples/libsmbclient
 *
 */
class SmbUtil
{
public:
     SmbUtil();
     SmbUtil(Smb::AuthenticationFunction fn);
     SmbUtil(const QString& authUser, const QString& authPassword);
     SmbUtil(const QUrl& smbUrl, Smb::AuthenticationFunction fn = 0);  //may have smb:://user::password@host/..
     ~SmbUtil();

public:
     enum StatReturn
     {
         StatInvalid = -3,
         StatDoesNotExist= -2,
         StatNoAccess= -1,
         StatDone=0,      // already done
         StatDir,
         StatHost,
         StatWorkgroup,
         StatShare
     };

public:
    Smb::Context     createContext();
    void             deleteContext(Smb::Context context);
    void             setAuthenticationCallback(Smb::AuthenticationFunction fn);
    StatReturn       getStatInfo(const QString &smb_path, struct stat *st);
    StatReturn       getStatvfsInfo(const QString& smb_path, struct statvfs *st);
    Smb::FileHandler openDir(Smb::Context context, const QString& smb_string);
    Smb::FileHandler openFile(Smb::Context context,const QString& smb_path,
                              int flags = O_RDONLY, mode_t mode = 0);
    bool             changePermissions(Smb::Context context, const QString& smb_path, mode_t mode);
    void             closeHandle(Smb::Context context, Smb::FileHandler fd);
    QStringList      lisShares();
    QStringList      listContent(QString smb_path,
                                 bool recursive = false,
                                 QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot,
                                 const QStringList& filterNames = QStringList());
    int              getFstat(Smb::Context context, Smb::FileHandler fd, struct stat*  st);
    int              getStat(Smb::Context context, const QString& smb_path, struct stat*  st);

private:   
    StatReturn      guessDirType(Smb::Context context, Smb::FileHandler fd);
    bool            checkValidShareName(const char *shareName);
    QStringList     walkForShares(QString smb_path);
    QString         findSmBServer(const smbc_dirent&);


private:
    static void     authenticateCallBack(
                                const char  *server,
                                const char  *share,
                                char        *wrkgrp,
                                int         wrkgrplen,
                                char        *user,
                                int         userlen,
                                char        *passwd,
                                int         passwdlen);

protected:
    void            init(const QString& user, const QString& password, Smb::AuthenticationFunction fn);
    bool            namesMatchFilter(const QString& str, const QStringList& filterNames);

private:  
   Smb::AuthenticationFunction   m_authCallBack;


#if defined(REGRESSION_TEST_QSAMBACLIENT)
    friend class TestQSambaSuite;
#endif

};

#endif // SMBUTIL_H

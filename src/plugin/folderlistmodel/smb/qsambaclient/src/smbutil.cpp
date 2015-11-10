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
 * File: smbutil.cpp
 * Date: 20/11/2014
 */

#include "smbutil.h"
#include "locationurl.h"
#include "netutil.h"
#include <sys/stat.h>
#include <sys/statfs.h>
#include <errno.h>

#include <QUrl>
#include <QDebug>
#include <QRegExp>

// set debug level at compilation time
#ifndef SMB_DEBUG_LEVEL
#define SMB_DEBUG_LEVEL 0
#endif


#if defined(SHOW_MESSAGES)
#  define DBG(more_items) qDebug() << Q_FUNC_INFO  more_items
#  define DBG_STAT(ret) qDebug() << Q_FUNC_INFO \
                                 << "return:" << ret \
                                 << "mode:"   << st->st_mode \
                                 << "mtime:"  << st->st_mtime \
                                 << "size:"   << st->st_size
#else
#define DBG(none)
#define DBG_STAT(ret)
#endif

#define SHOW_ERRNO(path)        if (errno != 0) \
                                { \
                                    qWarning() << Q_FUNC_INFO << "path:" << path << "errno:" << errno << strerror(errno); \
                                }

#define  URL_SLASHES_NUMBER_FOR_SHARES  3

namespace
{
   QByteArray   s_user("guest");
   QByteArray   s_passwd;
   QByteArray   s_workGroup("WORKGROUP");
}

//===============================================================================================
/*!
 * \brief SmbUtil::SmbUtil() This is the default constructor that provides the default authentication method
 *
 *  The user is the current user, password is "passwd" and the authentication function is \ref authenticateCallBack()
 */
SmbUtil::SmbUtil()
{
   init(::qgetenv("USER"), QLatin1String("passwd"), &SmbUtil::authenticateCallBack);
}


//===============================================================================================
/*!
 * \brief SmbUtil::SmbUtil() This constructor accepts an \a user and \a password for authentication
 *
 *   The authentication function is \ref authenticateCallBack()
 *
 * \param authUser
 * \param authPassword
 */
SmbUtil::SmbUtil(const QString& authUser, const QString& authPassword)
{
    init(authUser, authPassword, &SmbUtil::authenticateCallBack);
}


//===============================================================================================
/*!
 * \brief SmbUtil::SmbUtil() This constructor accepts another authentication function other than the default
 *
 *  No users no password is providedm the function should be able to provide everything
 *
 * \param ptAuthenticateCallBack
 */
SmbUtil::SmbUtil(Smb::AuthenticationFunction ptAuthenticateCallBack):
         m_authCallBack(ptAuthenticateCallBack)
{

}


//===============================================================================================
/*!
 * \brief SmbUtil::SmbUtil() This is the more complete constructor where user and password can come from the \a smbUrl
 * \param smbUrl
 * \param fn
 */
SmbUtil::SmbUtil(const QUrl& smbUrl, Smb::AuthenticationFunction fn)
{
    m_authCallBack  = fn ? fn : &SmbUtil::authenticateCallBack;
    if (!smbUrl.userName().isEmpty())
    {
        init(smbUrl.userName(), smbUrl.password(), m_authCallBack);
    }
}


//===============================================================================================
/*!
 * \brief SmbUtil::~SmbUtil() destructor
 */
SmbUtil::~SmbUtil()
{   
}


//===============================================================================================
/*!
 * \brief SmbUtil::authenticateCallBack() Default authentication function, it uses static variables to keep user/password
 * \param server
 * \param share
 * \param wrkgrp
 * \param wrkgrplen
 * \param user
 * \param userlen
 * \param passwd
 * \param passwdlen
 */
void SmbUtil::authenticateCallBack( const char *server,
                                    const char *share,
                                    char *wrkgrp,
                                    int wrkgrplen,
                                    char *user,
                                    int userlen,
                                    char *passwd,
                                    int passwdlen)
{
    Q_UNUSED(server);
    Q_UNUSED(share);
    DBG(<< "server:" << server << "share:" << share << "wrkgrp:" << wrkgrp << "user:" << user << "passwd:" << passwd);

#if 0
    //this may not be necessary
    ::strncpy(wrkgrp, s_workGroup.constData(), --wrkgrplen);
#else
    Q_UNUSED(wrkgrp);
    Q_UNUSED(wrkgrplen);
#endif

 // check some environment variables to help test authentication
#if defined(REGRESSION_TEST_QSAMBACLIENT)
    QByteArray  env = ::qgetenv("SMB_DEFAULT_USER");
    if (env.size() > 0)
    {
        s_user = env;
    }
    env = ::qgetenv("SMB_DEFAULT_PASSWORD");
    if (env.size() > 0)
    {
        s_passwd = env;
    }
#endif

    ::strncpy(user,   s_user.constData(),      --userlen);
    ::strncpy(passwd, s_passwd.constData(),    --passwdlen);
}



//===============================================================================================
/*!
 * \brief SmbUtil::init() Just stores user/password and authentication function
 * \param user
 * \param password
 * \param fn
 */
void SmbUtil::init(const QString &user, const QString &password, Smb::AuthenticationFunction fn)
{
    s_user   = user.toLocal8Bit();
    s_passwd = password.toLocal8Bit();
    m_authCallBack = fn;
}



//===============================================================================================
/*!
 * \brief SmbUtil::createContext() It creates a SMB context which is necessary to all operations
 *
 *     It sets the current authentication function callback
 *
 * \return the context created
 */
Smb::Context SmbUtil::createContext()
{
    Smb::Context ctx = smbc_new_context();
    if (ctx)
    {
        smbc_setDebug(ctx, SMB_DEBUG_LEVEL);                 
        smbc_setFunctionAuthData(ctx, m_authCallBack);
        if (smbc_init_context(ctx) == NULL)
        {
            smbc_free_context(ctx, 1);
            ctx = 0;
        }
    }
    DBG(<< "ctx:" << ctx);
    return ctx;
}


//===============================================================================================
/*!
 * \brief SmbUtil::deleteContext() Just deletes a context created by \ref createContext()
 * \param context
 */
void SmbUtil::deleteContext(Smb::Context context)
{   
    smbc_getFunctionPurgeCachedServers(context)(context);
    smbc_free_context(context, 1);
    DBG();
}


//===============================================================================================
/*!
 * \brief SmbUtil::openFile() opens a file
 * \param context
 * \param smb_path  it must point to a file full pathname
 * \param flags
 * \param mode
 * \return the FileHandler or NULL when it is not possible to open the file
 */
Smb::FileHandler
SmbUtil::openFile(Smb::Context context, const QString &smb_path, int flags , mode_t mode)
{   
    Smb::FileHandler fd = ::smbc_getFunctionOpen(context)
                            (context, smb_path.toLocal8Bit().constData(), flags, mode);

    if (fd == 0 && errno != EISDIR)
    {
        QString ipUrl = NetUtil::urlConvertHostnameToIP(smb_path);
        if (!ipUrl.isEmpty())
        {
            fd = ::smbc_getFunctionOpen(context)
                    (context, ipUrl.toLocal8Bit().constData(), flags, mode);
        }
    }
    if (fd == 0)
    {
        SHOW_ERRNO(smb_path);
    }
    return fd;
}


//===============================================================================================
/*!
 * \brief SmbUtil::openDir() opens a directory
 * \param context
 * \param smb_string it must point to a directory full pathname
 * \return  the FileHandler or NULL when it is not possible to open the directory
 */
Smb::FileHandler
SmbUtil::openDir(Smb::Context context, const QString &smb_string)
{ 
  Smb::FileHandler fd = ::smbc_getFunctionOpendir(context)
                             (context, smb_string.toLocal8Bit().constData());

  if (fd == 0)
  {
      //try to use an IP address if possible
       QString ipUrl = NetUtil::urlConvertHostnameToIP(smb_string);
       if (!ipUrl.isEmpty())
       {
           fd = ::smbc_getFunctionOpendir(context)
                                        (context, ipUrl.toLocal8Bit().constData());
       }
  }
  if (fd == 0)
  {
      SHOW_ERRNO(smb_string);
  }
  return fd;
}


//===============================================================================================
/*!
 * \brief SmbUtil::closeHandle() closes a open FileHandler created by \ref openDir() or \ref openFile()
 * \param context
 * \param fd
 */
void SmbUtil::closeHandle(Smb::Context context, Smb::FileHandler fd)
{
    if (fd)
    {
        ::smbc_getFunctionClose(context)(context, fd);
    }
}

//===============================================================================================
/*!
 * \brief SmbUtil::setAuthenticationCallback() Just sets the authentication function
 * \param fn
 */
void SmbUtil::setAuthenticationCallback(Smb::AuthenticationFunction fn)
{
    m_authCallBack = fn;
}

//===============================================================================================
/*!
 * \brief SmbUtil::getStatInfo() It gets information about files and directories, similar to POSIX stat(2)
 *
 *
 * The distintion between files and directories is made by \ref openDir() and \ref openFile(), as just one
 *  of them should open the \a smb_path.
 * For directories which it is supposed to have a content it is necessary to know if it is a
 *   host/share/workgroup or a single directory, for this the \ref guessDirType() is used.
 *
 * \param smb_path  it must point to a file full pathname file or directory
 * \param st        pointer to a struct stat which will receive the information
 * \return   one of the \ref StatReturn
 */
SmbUtil::StatReturn
SmbUtil::getStatInfo(const QString &smb_path, struct stat* st)
{   
    Smb::Context context = createContext();       
    Q_ASSERT(context);  
    StatReturn ret = StatInvalid;
    int slashes = smb_path.count(QDir::separator());
    Smb::FileHandler fd = 0;
    ::memset(st, 0, sizeof(struct stat));
    if ((fd=openDir(context, smb_path)) )
    {
        ret = guessDirType(context,fd);
        closeHandle(context, fd);
        if (ret == StatDir)
        {
            //  smb:// -> slahes=2   smb://workgroup -> slahes=2   smb://host/share -> slashes=3=URL_SLASHES_NUMBER_FOR_SHARES
            if (slashes == URL_SLASHES_NUMBER_FOR_SHARES)
            {
                ret = StatShare;
            }
            (void)getStat(context,smb_path,st);
        }
    }
    else if (errno != EACCES && errno != ECONNREFUSED && slashes >= URL_SLASHES_NUMBER_FOR_SHARES) // perhaps is a file
    {
        errno = 0;        
        if (getStat(context,smb_path,st) == 0)
        {
            ret = StatDone;
        }
    }

    if (errno != 0)
    {
        SHOW_ERRNO(smb_path);
        switch(errno)
        {
           case EACCES:
                //force shares to have Directory attribute
                if (slashes == URL_SLASHES_NUMBER_FOR_SHARES)
                {
                     st->st_mode |= S_IFDIR;
                }
                ret = StatNoAccess; //authentication should have failed
                break;
           case ENOENT:
           case ENODEV:
           case ECONNREFUSED:
                ret = StatDoesNotExist; //item does not exist
                break;
           default:
                break;
        }
    }
    deleteContext(context);
    return ret;
}


//===============================================================================================
/*!
 * \brief SmbUtil::guessDirType() gets the first directory item to guess the content type
 * \param context
 * \param fd an already opened FileHandler of a directory
 * \return the item type in \ref StatReturn
 */
SmbUtil::StatReturn
SmbUtil::guessDirType(Smb::Context context, Smb::FileHandler fd)
{
    struct smbc_dirent	*dirent=0;
    StatReturn ret = StatDone;
    while (ret == StatDone &&
           (dirent = smbc_getFunctionReaddir(context)(context, fd)) )
    {
        if (!dirent->name[0] && dirent->smbc_type != SMBC_SERVER )
        {
            continue;
        }
        switch(dirent->smbc_type)
        {
            //current item is a Host
            case SMBC_FILE_SHARE:                
                 ret = StatHost;
            break;
            //current item is a Workgroup
            case SMBC_SERVER:
                 ret = StatWorkgroup;
            break;
            //current item is Root smb://
            case SMBC_WORKGROUP:
            break;
            //ignore system shares
            case SMBC_PRINTER_SHARE:
            case SMBC_COMMS_SHARE:
            case SMBC_IPC_SHARE:
            break;
            //current item is Common directory
            // or a share, shares are handdled by the caller
            default:
                 ret = StatDir;
            break;
        }
    }
    return ret;
}

//===============================================================================================
/*!
 * \brief SmbUtil::listContent() Just lists the content of a directory/share/workgroup/hostname
 * \param smb_path it must point to full pathname directory/share/workgroup/hostname
 * \param recursive
 * \param filters a QDir like filter
 * \return The string list that matches \a filters
 */
QStringList SmbUtil::listContent(QString smb_path, bool recursive, QDir::Filters filters , const QStringList &filterNames)
{
    QStringList content;
    Smb::Context context = createContext();
    Q_ASSERT(context);   
    QStringList  paths_Dot_or_DotDot;
    Smb::FileHandler fd = openDir(context,smb_path);
    if (fd)
    {
        struct smbc_dirent	*dirent = 0;
        const char *cur_name        = 0;
        while ((dirent = smbc_getFunctionReaddir(context)(context, fd)) )
        {           
            //first check for hidden files
            if (!(filters & QDir::Hidden) && dirent->name[0] == '.')
            {
                continue;
            }           
            if ( !dirent->name[0] && dirent->smbc_type != SMBC_SERVER)
            {
                //it may be a libsmbclient bug                
                continue;
            }
            cur_name = dirent->name;
            QString path;
            bool itemHasContent = false;
            switch(dirent->smbc_type)
            {
               case SMBC_PRINTER_SHARE:
               case SMBC_COMMS_SHARE:
               case SMBC_IPC_SHARE:
                    continue;
                    break;
               case SMBC_WORKGROUP:
               case SMBC_SERVER:                    
                    itemHasContent = true;
                    path = LocationUrl::SmbURL;
                    if (dirent->smbc_type == SMBC_SERVER)
                    {
                         QString goodHostName = findSmBServer(*dirent);
                         //path += NetUtil::normalizeHostName(goodHostName);
                         path += goodHostName;
                    }
                    else
                    {
                        path += cur_name;
                    }
                    break;
               case SMBC_DIR:                   
                    if (filters & QDir::Dirs)
                    {
                        bool isDot     = ::strcmp(".", cur_name) == 0;
                        bool isDotDot  = ::strcmp("..", cur_name) == 0;
                        if(      (!(filters & QDir::NoDot)    && isDot)
                              || (!(filters & QDir::NoDotDot) && isDotDot)
                              || (!isDot && !isDotDot))
                        {                            
                             if (!isDot && !isDotDot)
                             {
                                 itemHasContent = true;
                                 path = smb_path + QDir::separator() + cur_name;
                             }
                             else // (isDot || isDotDot)
                             {
                                 paths_Dot_or_DotDot.append(smb_path + QDir::separator() + cur_name);
                             }
                        }
                    }
                    break;
               case SMBC_FILE:
               case SMBC_LINK:
                    if (filters & QDir::Files)
                    {                     
                        path = smb_path + QDir::separator() + cur_name;
                    }
                    break;
               case SMBC_FILE_SHARE:                   
                    if (checkValidShareName(cur_name))
                    {
                        itemHasContent = true;
                        path = smb_path + QDir::separator() + cur_name;
                    }
                    break;
            }//switch
            if (!path.isEmpty())
            {
                if (filterNames.isEmpty() || namesMatchFilter(cur_name, filterNames))
                {
                    content.append(path);
                }
                if (recursive && itemHasContent )
                {
                    content += listContent(path, true, filters, filterNames);
                }
            }
        }//while
        closeHandle(context, fd);
    }//if (fd)
    else
    {
        SHOW_ERRNO(smb_path);
    }
    deleteContext(context);
    if (paths_Dot_or_DotDot.count() > 0)
    {
        content += paths_Dot_or_DotDot;
    }
    return content;
}


//===============================================================================================
/*!
 * \brief SmbUtil::lisShares() Brings the list of all available file shares in the network
 * \return all available file shares
 */
QStringList SmbUtil::lisShares()
{
    return walkForShares(LocationUrl::SmbURL);
}

//===============================================================================================
/*!
 * \brief SmbUtil::walkForShares() Just a helper function that can be recursive, called by  \ref lisShares()
 * \param smb_path
 * \return list of shares from a single hostname
 */
QStringList SmbUtil::walkForShares(QString smb_path)
{
    QStringList content;
    Smb::Context context = createContext();
    Q_ASSERT(context);
    Smb::FileHandler fd = openDir(context,smb_path);
    if (fd)
    {
        struct smbc_dirent	*dirent = 0;
        const char *cur_name        = 0;
        QString path;
        while ((dirent = smbc_getFunctionReaddir(context)(context, fd)))
        {          
            cur_name = dirent->name;
            if ( !dirent->name[0] && dirent->smbc_type != SMBC_SERVER)
            {
                //it may be a libsmbclient bug               
                    continue;             
            }
            switch(dirent->smbc_type)
            {
               case SMBC_WORKGROUP:
               case SMBC_SERVER:
                    path = LocationUrl::SmbURL;
                    if (dirent->smbc_type == SMBC_SERVER)
                    {
                        QString goodHostName = findSmBServer(*dirent);
                        //path += NetUtil::normalizeHostName(goodHostName);
                        path += goodHostName;
                    }
                    else
                    {
                        path += cur_name;
                    }
                    content += walkForShares(path);
                    break;
               case SMBC_FILE_SHARE:                   
                    if (checkValidShareName(cur_name))
                    {
                        path = smb_path + QDir::separator() + cur_name;
                        content.append(path);
                    }
                    break;
            }//switch
        }//while
    }//if (fd)
    deleteContext(context);
    return content;
}


//===============================================================================================
/*!
 * \brief SmbUtil::checkValidShareName()  Helper function to ignore some system shares that should not contain any file
 *
 *   It is used for \ref lisShares() and \ref listContent()
 *
 * \param shareName
 * \return TRUE if the share has a good name (should contain files), FALSE if it is supposed to be a system share
 */
bool SmbUtil::checkValidShareName(const char *shareName)
{
    if (::strcmp(shareName, "print$") == 0)
    {
      return false;
    }
    if (::strcmp(shareName, "ADMIN$") == 0)
    {
      return false;
    }

    return true;
}


//===============================================================================================
/*!
 * \brief SmbUtil::getStatvfsInfo
 * \param smb_path it must point to a file full pathname file or directory
 * \param st pointer to a struct statvfs
 * \return StatDone in case of success or StatInvalid.
 */
SmbUtil::StatReturn
SmbUtil::getStatvfsInfo(const QString &smb_path, struct statvfs *st)
{
    Smb::Context context = createContext();
    Q_ASSERT(context);
    ::memset(st,0,sizeof(struct statvfs));
    StatReturn ret = StatInvalid;
    Smb::FileHandler fd = openDir(context,smb_path);
    if (fd == 0)
    {
        fd = openFile(context,smb_path);
    }
    if (fd == 0) // item does not exist neither dir nor file
    {
        //usually smb_path is a file that does not exist yet
        //so using the path
        int lastSlash = smb_path.lastIndexOf(QDir::separator());
        if (lastSlash != -1)
        {
             QString path (smb_path.mid(0,lastSlash));
             fd = openDir(context,path);
        }
    }
    if (fd != 0)
    {
        ret = static_cast<StatReturn> (::smbc_getFunctionFstatVFS(context)(context,fd, st));
        closeHandle(context, fd);
    }
    deleteContext(context);
    return ret;
}


bool SmbUtil::namesMatchFilter(const QString & str, const QStringList &filterNames)
{
    bool ret = true;
    int counter = filterNames.count();
    while (ret && counter--)
    {
        QRegExp regExp(filterNames.at(counter), Qt::CaseSensitive, QRegExp::Wildcard);
        ret = regExp.exactMatch(str);
    }
    return ret;
}

/*!
 * \brief SmbUtil::findSmBServer() Helper function to find the server name
 * \param dirent smbc_dirent & dirent result of smbc_getFunctionReaddir()
 *
 *   1. Some smbclient versions (or host configuration) bring dirent.name empty when browsing localhost
 *
 *   2. When dirent.name brings the hostname, usually it is limited to 16 characters which will not reacheable in the network
 *      in this case try to get the name from the comment
 *
 * \return the hostname
 */
QString SmbUtil::findSmBServer(const smbc_dirent & dirent)
{
    QString host;
    if (dirent.name[0] != 0)
    {
        QString name(dirent.name);
        host = name;
    }
    if (host.isEmpty())
    {
        QString comment(dirent.comment);
        if (!comment.isEmpty())
        {
            QString fullName = comment.split(QLatin1Char(' '), QString::SkipEmptyParts).first();
            if (!fullName.isEmpty())
            {
                host = fullName;
            }
        }
    }
    if (host.isEmpty())
    {
        host = QLatin1String("localhost");
    }
    return host.toLower();
}


bool SmbUtil::changePermissions(Smb::Context context, const QString& smb_path, mode_t mode)
{
    int ret = ::smbc_getFunctionChmod(context)(context, smb_path.toLocal8Bit().constData(), mode);
    if (ret < 0)
    {
        SHOW_ERRNO(smb_path);
    }
    return ret == 0;
}


int
SmbUtil::getFstat(Smb::Context context, Smb::FileHandler fd, struct stat*  st)
{
     ::memset(st,0,sizeof(struct stat));
     int ret = ::smbc_getFunctionFstat(context)(context,fd, st);
     DBG_STAT(ret);
     return ret;
}


int
SmbUtil::getStat(Smb::Context context, const QString& smb_path, struct stat*  st)
{
    ::memset(st,0,sizeof(struct stat));
    DBG(<< smb_path);
    int ret = ::smbc_getFunctionStat(context)(context,smb_path.toLocal8Bit().constData(), st);
    DBG_STAT(ret);
    return ret;
}

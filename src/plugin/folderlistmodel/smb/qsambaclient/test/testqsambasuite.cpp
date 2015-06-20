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
 * File: testqsambasuite.cpp
 * Date: 03/12/2014
 */

#include "testqsambasuite.h"
#include "smbusershare.h"
#include "smbiteminfo.h"
#include "smbplaces.h"
#include "locationurl.h"
#include "smbutil.h"
#include "smblocationdiriterator.h"
#include <sys/stat.h>

#include <QTest>
#include <QFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDebug>


#define RETURN_SHARE_STATUS_WHEN_FALSE(x,y) if (! (x == y) ) { \
                                             qDebug() << "ERROR:" << Q_FUNC_INFO << "line:" << __LINE__ << "compare failed"; \
                                             return ret; \
                                            }

ShareCreationStatus::ShareCreationStatus(const QString& dirNameMask) :
  status(false),
  tempDir(new QTemporaryDir(QDir::tempPath() + QDir::separator() + dirNameMask))
{
   sharedDirPath = tempDir->path();
}

ShareCreationStatus::ShareCreationStatus(const ShareCreationStatus & o)
{  
    *this = o;
}


ShareCreationStatus::~ShareCreationStatus()
{
    if (tempDir)
    {
        delete tempDir;
    }
}

ShareCreationStatus &
ShareCreationStatus::operator=(const ShareCreationStatus & o)
{
    ShareCreationStatus *other = const_cast<ShareCreationStatus*> (&o);
    *this = *other;
    return *this;
}

ShareCreationStatus &
ShareCreationStatus::operator=(ShareCreationStatus & o)
{

    sharedDirPath  = o.sharedDirPath;
    shareName      = o.shareName;
    fileContent    = o.fileContent;
    status         = o.status;
    tempDir        = o.tempDir;
    url            = o.url;
    if (tempDir)
    {
      tempDir->setAutoRemove(true);
    }
    o.tempDir      = 0;
    return *this;
}


TestQSambaSuite::TestQSambaSuite(QObject *parent) :
    QObject(parent)
   ,m_smbShares( new SmbPlaces() )
{
}


TestQSambaSuite::~TestQSambaSuite()
{
    delete m_smbShares;
}


void TestQSambaSuite::initTestCase()
{   
    QCOMPARE(SmbUserShare::canCreateShares(), true);
}


void TestQSambaSuite::cleanupTestCase()
{

}


void TestQSambaSuite::init()
{
    m_curShareName.clear();
}

/*!
 * \brief TestQSambaSuite::cleanup()
 *
 *  It is called after every test case, removes the shared indicated by m_curShareName
 *    and checks if it was removed
 *
 */
void TestQSambaSuite::cleanup()
{
    if (!m_curShareName.isEmpty())
    {
        SmbUserShare::removeShare(m_curShareName);
        SmbUserShare::UserShareFile share = SmbUserShare::search(m_curShareName);
        QCOMPARE(share.exists() , false);
    }
}

// ============== utils

/*!
 * \brief TestQSambaSuite::createTempFile() creates a file \a name  inside \a path
 * \param path
 * \param name
 * \param content some content or nothing
 *
 * \return fullpath of the file if it was able to create
 */
QString TestQSambaSuite::createTempFile(const QString &path,
                                        const QString &name,
                                        const QByteArray &content)

{
    QString ret;
    QFile f( path + QDir::separator() + name );
    if (f.open(QFile::WriteOnly))
    {
        if (f.write(content) == content.size())
        {
            ret = f.fileName();
        }
        f.close();
    }
    return ret;
}


/*!
 * \brief TestQSambaSuite::existShare(0 checks is share name exist in a such list of shares
 * \param sharesFullPathList someting like ("smb://localhost/samba-devel", "smb://localhost/mac-devel")
 * \param shareName          e.g. "samba-devel"
 * \return true if exists
 */
bool TestQSambaSuite::existsShare(const QStringList& sharesFullPathList, const QString &shareName)
{
    bool ret = false;
    int counter = sharesFullPathList.count();
    while (counter--)
    {
        if (sharesFullPathList.at(counter).contains(shareName))
        {
            ret = true;
            break;
        }
    }
    return ret;
}


ShareCreationStatus TestQSambaSuite::createTempShare(const QString &maskName,
                                                     bool fullAccess,
                                                     bool allowGuests)
{

    ShareCreationStatus ret(maskName);
    QTemporaryDir * shareDir = ret.tempDir;
    RETURN_SHARE_STATUS_WHEN_FALSE(shareDir->isValid(),   true);

    m_curShareName = QFileInfo(shareDir->path()).fileName();

    //put some content in it
    QString filename = createTempFile(shareDir->path(), "somecontent.txt", "hello Samba");
    RETURN_SHARE_STATUS_WHEN_FALSE(filename.isEmpty(),    false);

    ret.fileContent = filename;

    //save current samba shares list
    QStringList currentShares = m_smbShares->listPlacesSync();

    //get the supposed share name that is going to be created
    QString     shareDirName  = SmbUserShare::proposedName(shareDir->path());
    ret.shareName = shareDirName;

    //make sure shareDirName does not exist in the current samba share list
    RETURN_SHARE_STATUS_WHEN_FALSE(shareDirName.isEmpty(),  false);
    RETURN_SHARE_STATUS_WHEN_FALSE(shareDirName,  m_curShareName);
    bool exists = existsShare(currentShares,shareDirName);
    RETURN_SHARE_STATUS_WHEN_FALSE(exists, false);

    //create the share
    bool created = SmbUserShare::createShareForFolder(shareDir->path(),
                                                      fullAccess ? SmbUserShare::ReadWrite : SmbUserShare::Readonly,
                                                      allowGuests);
    RETURN_SHARE_STATUS_WHEN_FALSE(created,   true);

    // now make sure the new share is created
    QStringList moreOneShare =  m_smbShares->listPlacesSync();
    exists = existsShare(moreOneShare, shareDirName);
    RETURN_SHARE_STATUS_WHEN_FALSE(exists,   true);

    //everything is OK
    ret.status = true;

    //let the share be removed by its path instead of the name
    m_curShareName = shareDir->path();
    ret.url = LocationUrl::SmbURL + "localhost/" + ret.shareName;

    //remove this
    //=============================================================
    m_curShareName.clear();

    return ret;
}


//================ test  cases==================
void TestQSambaSuite::positive_createReadOnlyUserShare()
{
    //create a directory to share its content
    QTemporaryDir shareDir (QDir::tempPath() + QDir::separator() + "positive_createReadOnlyUserShare");
    m_curShareName = QFileInfo(shareDir.path()).fileName();
    QCOMPARE(shareDir.isValid(),   true);

    //put some content in it
    QString filename = createTempFile(shareDir.path(), "sometext.xt", "hello Samba");
    QCOMPARE(filename.isEmpty(),    false);

    //save current samba shares list
    QStringList currentShares = m_smbShares->listPlacesSync();
    qDebug() << "currentShares:" << currentShares;

    //get the supposed share name that is going to be created
    QString     shareDirName  = SmbUserShare::proposedName(shareDir.path());

    //make sure shareDirName does not exist in the current samba share list
    QCOMPARE(shareDirName.isEmpty(),  false);
    QCOMPARE(shareDirName,  m_curShareName);
    bool exists = existsShare(currentShares,shareDirName);
    QCOMPARE(exists, false);

    //create the share
    bool created = SmbUserShare::createShareForFolder(shareDir.path());
    QCOMPARE(created,   true);

    // now make sure the new share is created
    QStringList moreOneShare =  m_smbShares->listPlacesSync();
    exists = existsShare(moreOneShare, shareDirName);
    qDebug() << "moreOneShare:" << moreOneShare;
    QCOMPARE(exists,   true);

    //using share name
    SmbUserShare::Access  readAccess = SmbUserShare::getEveryoneAccess(m_curShareName);
    bool isReadOnlyAccess = readAccess == SmbUserShare::Readonly;
    QCOMPARE(isReadOnlyAccess,   true);
}



void TestQSambaSuite::positive_createFullAccessUserShare()
{
    //create a directory to share its content
    QTemporaryDir shareDir (QDir::tempPath() + QDir::separator() + "positive_createFullAccessUserShare");
    m_curShareName = QFileInfo(shareDir.path()).fileName();
    QCOMPARE(shareDir.isValid(),   true);

    //put some content in it
    QString filename = createTempFile(shareDir.path(), "sometext.xt", "hello Samba");
    QCOMPARE(filename.isEmpty(),    false);

    //save current samba shares list
    QStringList currentShares = m_smbShares->listPlacesSync();
    qDebug() << "currentShares:" << currentShares;

    //get the supposed share name that is going to be created
    QString     shareDirName  = SmbUserShare::proposedName(shareDir.path());

    //make sure shareDirName does not exist in the current samba share list
    QCOMPARE(shareDirName.isEmpty(),  false);
    QCOMPARE(shareDirName,  m_curShareName);
    bool exists = existsShare(currentShares,shareDirName);
    QCOMPARE(exists, false);

    //create the share
    bool created = SmbUserShare::createShareForFolder(shareDir.path(), SmbUserShare::ReadWrite);
    QCOMPARE(created,   true);

    // now make sure the new share is created
    QStringList moreOneShare =  m_smbShares->listPlacesSync();
    exists = existsShare(moreOneShare, shareDirName);
    qDebug() << "moreOneShare:" << moreOneShare;
    QCOMPARE(exists,   true);

    //using dir full path
    SmbUserShare::Access  readWrite = SmbUserShare::getEveryoneAccess(shareDir.path());
    bool isReadOnlyAccess = readWrite == SmbUserShare::ReadWrite;
    QCOMPARE(isReadOnlyAccess,   true);

    //let the share be removed by its path instead of the name
    m_curShareName = shareDir.path();
}


void TestQSambaSuite::positive_itemInfoRoot()
{
    SmbItemInfo root(LocationUrl::SmbURL);
    QCOMPARE(root.isLocal() , false);
    QCOMPARE(root.isHost(),   false);
    QCOMPARE(root.exists(),   true);
    QCOMPARE(root.isRelative(),false);
    QCOMPARE(root.isAbsolute(), true);
    QCOMPARE(root.isRoot(), true);
    QCOMPARE(root.isSharedDir(), false);
    QCOMPARE(root.isHost(),   false);
    QCOMPARE(root.isWorkGroup(), false);
    QCOMPARE(root.isShare(), false);
    QCOMPARE(root.absoluteFilePath(), LocationUrl::SmbURL);
    QCOMPARE(root.fileName().isEmpty(),  true);
    QCOMPARE(root.absolutePath(),  LocationUrl::SmbURL);
    QCOMPARE(root.sharePath().isEmpty(),  true);
}


void TestQSambaSuite::positive_itemInfoLocalHost()
{
    SmbItemInfo localhost(LocationUrl::SmbURL + "localhost");
    QCOMPARE(localhost.isLocal() , false);
    QCOMPARE(localhost.isHost(),   true);
    QCOMPARE(localhost.exists(),   true);
    QCOMPARE(localhost.isRelative(),false);
    QCOMPARE(localhost.isAbsolute(), true);
    QCOMPARE(localhost.isRoot(),   false);
    QCOMPARE(localhost.isSharedDir(), false);
    QCOMPARE(localhost.isWorkGroup(), false);
    QCOMPARE(localhost.isShare(), false);
    QCOMPARE(localhost.path(), LocationUrl::SmbURL);
    QCOMPARE(localhost.filePath(), LocationUrl::SmbURL + "localhost");
    QCOMPARE(localhost.absoluteFilePath(), LocationUrl::SmbURL + "localhost");
    //for hosts sharePath() are the hostitself
    QCOMPARE(localhost.sharePath(), localhost.absoluteFilePath());
}


void TestQSambaSuite::positive_itemInfoShare()
{
    ShareCreationStatus share(createTempShare("positive_itemInfoShare"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);
    QFileInfo file(share.fileContent);
    QCOMPARE(file.exists(),   true);

    QString urlPath("smb://localhost/" + share.shareName);
    //basic share/dir information
    SmbItemInfo dirinfo(urlPath);
    QCOMPARE(dirinfo.isLocal() , false);
    QCOMPARE(dirinfo.isHost(),   false);
    QCOMPARE(dirinfo.exists(),   true);
    QCOMPARE(dirinfo.isRelative(),false);
    QCOMPARE(dirinfo.isAbsolute(), true);
    QCOMPARE(dirinfo.isDir(),    true);   // shares also are directories
    QCOMPARE(dirinfo.isFile(),   false);
    QCOMPARE(dirinfo.isReadable(), true);
    QCOMPARE(dirinfo.isWritable(), true);
    QCOMPARE(dirinfo.isExecutable(), true);
    QCOMPARE(dirinfo.isSelected(), false);
    QCOMPARE(dirinfo.isSharedDir(), false);
    QCOMPARE(dirinfo.isRoot(), false);
    QCOMPARE(dirinfo.isHost(),   false);
    QCOMPARE(dirinfo.isWorkGroup(), false);
    QCOMPARE(dirinfo.isShare(), true);
    QCOMPARE(dirinfo.sharePath(), dirinfo.absoluteFilePath());
}

void TestQSambaSuite::positive_itemInfoCommonPermissions()
{
    ShareCreationStatus share(createTempShare("iteminfoCommonPermission"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);
    QFileInfo file(share.fileContent);
    QCOMPARE(file.exists(),   true);

    //set a common permission to the file
    QFile::Permissions  myPermissions = QFile::ReadOwner | QFile::WriteOwner |
                                        QFile::ReadGroup | QFile::ReadOther;
    QCOMPARE(QFile::setPermissions(share.fileContent, myPermissions), true);

    QString urlPath("smb://localhost/" + share.shareName);
    QString url(urlPath + QDir::separator() + file.fileName());
    //basic file information from share
    SmbItemInfo fileinfo(url);
    QCOMPARE(fileinfo.isLocal() , false);
    QCOMPARE(fileinfo.isHost(),   false);
    QCOMPARE(fileinfo.exists(),   true);
    QCOMPARE(fileinfo.isRelative(),false);
    QCOMPARE(fileinfo.isAbsolute(), true);
    QCOMPARE(fileinfo.isDir(),    false);
    QCOMPARE(fileinfo.isFile(),   true);
    QCOMPARE(fileinfo.isReadable(), true);
    QCOMPARE(fileinfo.isWritable(), true);
    QCOMPARE(fileinfo.isExecutable(), false);
    QCOMPARE(fileinfo.isSelected(), false);
    QCOMPARE(fileinfo.isSharedDir(), false);
    QCOMPARE(fileinfo.isRoot(), false);
    QCOMPARE(fileinfo.isRemote(), true);
    QCOMPARE(fileinfo.isSymLink(), false);
    QCOMPARE(fileinfo.absolutePath(), urlPath);
    QCOMPARE(fileinfo.filePath(), url);
    QCOMPARE(fileinfo.fileName(), file.fileName());
    QCOMPARE(fileinfo.absoluteFilePath(), url);
    QCOMPARE(fileinfo.permissions(), myPermissions);
    QCOMPARE(fileinfo.sharePath(),  urlPath);
}


void TestQSambaSuite::negative_emptyItemInfo()
{
    SmbItemInfo notSmb;
    QCOMPARE(notSmb.isValid(),  false);
    QCOMPARE(notSmb.exists(),   false);
    QCOMPARE(notSmb.isHost(),   false);
}


void TestQSambaSuite::negative_itemInfo()
{
    SmbItemInfo notSmb("test://localhost");
    QCOMPARE(notSmb.isValid(),  false);
    QCOMPARE(notSmb.exists(),   false);

    SmbItemInfo notHost("smb://__this_host_must_not_exist_");
    QCOMPARE(notHost.isValid(),  true);
    QCOMPARE(notHost.exists(),   false);

    SmbItemInfo notShare("smb://localhost/__this_share_must_not_exist_");
    QCOMPARE(notShare.isValid(),  true);
    QCOMPARE(notShare.exists(),   false);

    SmbPlaces  shares;
    QStringList existentShares = shares.listPlacesSync();
    if (existentShares.count() > 0)
    {
        SmbItemInfo parent(existentShares.at(0));
        QCOMPARE(parent.isValid(),  true);
        QCOMPARE(parent.exists(),   true);
        // it may not have permission to access
        if (parent.isReadable())
        {
            QCOMPARE(parent.isShare(),  true);
            SmbItemInfo dir(existentShares.at(0) +
                            QDir::separator() +
                            "__this_dir_must_not_exist_");
            QCOMPARE(dir.isValid(),  true);
            QCOMPARE(dir.exists(),   false);
        }
    }
}

void TestQSambaSuite::negative_createShareDirDoesNotExist()
{
   QDir d(QDir::homePath() + "___2323_hope_this_does_not_exist_");
   while (d.exists())
   {
       d.setPath(d.path() + QChar('1'));
   }

   bool ret = SmbUserShare::createShareForFolder(d.path());

   QCOMPARE(ret,  false);
}


void TestQSambaSuite::negative_dirIterator()
{
    SmbLocationDirIterator iterator1("Nome");
    QCOMPARE(iterator1.hasNext() , false);

    SmbLocationDirIterator iterator2("smb://localhost/_IT_MUST_NOT_EXIST_I_HOPE_");
    QCOMPARE(iterator2.hasNext() , false);
}


void TestQSambaSuite::positive_dirIterator()
{
    ShareCreationStatus share(createTempShare("positive_dirIterator"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);
    QFileInfo file(share.fileContent);
    QCOMPARE(file.exists(),   true);

    //create a second directory inside the temporary share
    QString secondDirStr("secondDir");
    QString secondElementPath = share.sharedDirPath + QDir::separator() + secondDirStr;
    QCOMPARE(QDir().mkpath(secondElementPath),  true);

    //create a file inside secondElementPath
    QString secondElementContent(secondElementPath + QDir::separator() + "test.readme");
    QFile f(secondElementContent);
    QCOMPARE(f.open(QFile::WriteOnly), true);
    f.close();

    //recursive 3 items
    int counter;
    SmbLocationDirIterator tree(share.url, QDir::AllEntries, QDirIterator::Subdirectories);
    for(counter=0; tree.hasNext() ; ++counter)
    {
        tree.next();
    }
    QCOMPARE(counter,  3);

    //using a mask
    QStringList nameFilter = QStringList() << "*.readme";
    SmbLocationDirIterator  onlyReadme(share.url, nameFilter,QDir::AllEntries, QDirIterator::Subdirectories);
    for(counter=0; onlyReadme.hasNext() ; ++counter)
    {
        onlyReadme.next();
    }
    QCOMPARE(counter,  1);

    //2 items
    SmbLocationDirIterator lowLevelIterator(share.url);   
    for(counter=0; lowLevelIterator.hasNext() ; ++counter)
    {
        lowLevelIterator.next();
    }
    QCOMPARE(counter,  2);

    //2 more "." and "."
    SmbLocationDirIterator lowLevelIterator2(share.url,QDir::AllEntries | QDir::Hidden);
    for(counter=0; lowLevelIterator2.hasNext() ; ++counter)
    {
        lowLevelIterator2.next();
    }
    QCOMPARE(counter,  4);

    //2 more ".."
    bool dotdot = false;
    SmbLocationDirIterator lowLevelIterator3(share.url,QDir::AllEntries | QDir::Hidden | QDir::NoDot);
    for(counter=0; lowLevelIterator3.hasNext() ; ++counter)
    {      
        lowLevelIterator3.next();
        if (lowLevelIterator3.fileName() == "..")
        {
            dotdot = true;
        }
    }
    QCOMPARE(counter,  3);
    QCOMPARE(dotdot, true);

     //2 more "."
    bool dot = false;
    SmbLocationDirIterator lowLevelIterator4(share.url,QDir::AllEntries | QDir::Hidden | QDir::NoDotDot);
    for(counter=0; lowLevelIterator4.hasNext() ; ++counter)
    {
        lowLevelIterator4.next();
        if (lowLevelIterator4.fileName() == ".")
        {
            dot = true;
        }
    }
    QCOMPARE(counter,  3);
    QCOMPARE(dot, true);

    bool secondDir = false;
    SmbLocationDirIterator dirOnly(share.url,QDir::Dirs | QDir::NoDotAndDotDot);
    for(counter=0; dirOnly.hasNext() ; ++counter)
    {
        dirOnly.next();
        if (dirOnly.fileName() == secondDirStr)
        {
            secondDir = true;
        }
    }
    QCOMPARE(counter,  1);
    QCOMPARE(secondDir, true);

    bool fileContent = false;
    SmbLocationDirIterator fileOnly(share.url,QDir::Files);
    for(counter=0; fileOnly.hasNext() ; ++counter)
    {
        fileOnly.next();
        if (fileOnly.fileName() == QFileInfo(share.fileContent).fileName())
        {
            fileContent = true;
        }
    }
    QCOMPARE(counter,  1);
    QCOMPARE(fileContent, true);


}


/*!
 * This is not a test case
 *
 * It intends to create a real user share in the local host
 */
bool TestQSambaSuite::createPermanentShare(const QString &path, bool fullAccess)
{
    SmbUserShare::Access access =  fullAccess ?  SmbUserShare::ReadWrite
                                              :  SmbUserShare::Readonly;
    SmbUserShare::createShareForFolder(path, access);
    SmbUserShare::UserShareFile ret = SmbUserShare::search(path);
    return ret.exists();
}


/*!
 * \brief TestQSambaSuite::openPermanenteShare()
 *
 *  It is not a test case, just shows some information about an URL
 *
 * \param smb_path
 * \return
 */
bool TestQSambaSuite::openPermanenteShare(const QString &smb_path)
{
    SmbUtil smb;  
    struct stat st;
    bool ok = smb.getStatInfo(smb_path, &st) != -1;
    qDebug() << st.st_mode
             << st.st_size
             << st.st_rdev
             << st.st_mtime
             << st.st_uid
             << st.st_blocks
             << st.st_ino
             ;
    return ok;
}

/*!
 * \brief TestQSambaSuite::listLocalhost()
 *
 *  It is not a test case
 */
bool TestQSambaSuite::listLocalhost()
{   
    QString smb_path("smb://localhost");  
    SmbLocationDirIterator listIterator(smb_path);
    QStringList shares;
    while (listIterator.hasNext())
    {
        shares.append(listIterator.next());
    }
    qDebug() << shares;
    return shares.count() > 0 ? true : false;
}


bool TestQSambaSuite::listRecursive()
{
    SmbUtil smb;
    QStringList all = smb.listContent(LocationUrl::SmbURL, true);
    if (all.count() == 0)
    {
        return false;
    }
    for (int counter=0 ; counter < all.count(); ++ counter)
    {
        qDebug() << all.at(counter);
    }
    return true;
}

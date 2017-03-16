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
#include "smblocationitemfile.h"
#include "smblocationitemdir.h"
#include <sys/stat.h>
#include <errno.h>

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
    fileContent.diskPathname    = o.fileContent.diskPathname;
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

SmbSharedPathAccess
ShareCreationStatus::createPathForItem(const QString &item)
{
  SmbSharedPathAccess pathItem;
  pathItem.diskPathname = sharedDirPath + QLatin1Char('/') + item;
  pathItem.smbUrl       = url + QLatin1Char('/') + item;
  return pathItem;
}

SmbSharedPathAccess
ShareCreationStatus::createPathForItems(const QStringList &items)
{
    SmbSharedPathAccess pathItem;
    pathItem.diskPathname = sharedDirPath;
    pathItem.smbUrl       = url;
    for (int counter=0; counter < items.count(); ++counter)
    {
        pathItem.diskPathname += QLatin1Char('/') + items.at(counter);
        pathItem.smbUrl       += QLatin1Char('/') + items.at(counter);
    }
    return pathItem;
}


TestQSambaSuite::TestQSambaSuite(QObject *parent) :
    QObject(parent)
   ,m_smbShares( new SmbPlaces() )
   ,m_curUmask(0)
{
}


TestQSambaSuite::~TestQSambaSuite()
{
    delete m_smbShares;
    if (!m_curShareName.isEmpty())
    {
        SmbUserShare::removeShare(m_curShareName);
    }
}


void TestQSambaSuite::initTestCase()
{   
    QCOMPARE(SmbUserShare::canCreateShares(), true);
    m_curUmask = umask(0);
}


void TestQSambaSuite::cleanupTestCase()
{
    umask(m_curUmask);
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
        m_curShareName.clear();
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
    QString fileContentName("somecontent.txt");
    QString filename = createTempFile(shareDir->path(), fileContentName, "hello Samba");
    RETURN_SHARE_STATUS_WHEN_FALSE(filename.isEmpty(),    false);

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

    //first remove the share if it already exists, perhaps due to a failure in a previous test
    SmbUserShare::removeShare(shareDir->path());
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
    ret.fileContent = ret.createPathForItem(fileContentName);

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
    QFileInfo file(share.fileContent.diskPathname);
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
    QFileInfo file(share.fileContent.diskPathname);
    QCOMPARE(file.exists(),   true);

    //set a common permission to the file
    QFile::Permissions  myPermissions = QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser |
                                        QFile::ReadGroup | QFile::ReadOther;
    QCOMPARE(QFile::setPermissions(share.fileContent.diskPathname, myPermissions), true);

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
    QFileInfo file(share.fileContent.diskPathname);
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
        if (fileOnly.fileName() == QFileInfo(share.fileContent.diskPathname).fileName())
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


void TestQSambaSuite::unit_QFile_rename()
{
    ShareCreationStatus share(createTempShare("unit_QFile_rename"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    //empty object
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QString newName("this_was_renamed.txt");
    QCOMPARE(fileNameEmptyDoesNotExist.rename(newName), false);
    QCOMPARE(fileNameEmptyDoesNotExist.rename(QLatin1String(0), newName), false);
    //orig file does not exist
    QString nameDoesNotExist("_it_must_not_exist");
    SmbSharedPathAccess item = share.createPathForItem(nameDoesNotExist);
    QFileInfo diskFileContent(item.diskPathname);
    QCOMPARE(diskFileContent.exists(), false);
    SmbLocationItemFile fileNameDoesNotExist(item.smbUrl);
    QCOMPARE(fileNameDoesNotExist.rename(newName), false);
    QCOMPARE(fileNameDoesNotExist.rename(nameDoesNotExist, newName), false);

    //positive tests
    SmbSharedPathAccess newItem = share.createPathForItem(newName);
    QFileInfo newFileInfo(newItem.diskPathname);
    //make sure target does not exist
    QCOMPARE(newFileInfo.exists(), false);
    //now rename
    SmbLocationItemFile fileNameExists(share.fileContent.smbUrl);
    QCOMPARE(fileNameExists.rename(newItem.smbUrl), true);
    //now target must exist in local file system
    QCOMPARE(newFileInfo.exists(), true);
    diskFileContent.setFile(share.fileContent.diskPathname);
    //make sure orignal file no longer exist
    QCOMPARE(diskFileContent.exists(), false);
    //now back to original name using an empty object
    SmbLocationItemFile empty;
    QCOMPARE(empty.rename(newItem.smbUrl, share.fileContent.smbUrl), true);
    //make sure orignal exists again
    QCOMPARE(diskFileContent.exists(), true);
}


void TestQSambaSuite::unit_QFile_remove()
{
    ShareCreationStatus share(createTempShare("unit_QFile_remove"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    //empty object
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.remove(), false);
    //file name does not exist
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileNameDoesNotExist(item.smbUrl);
    QCOMPARE(fileNameDoesNotExist.remove(), false);
    QFileInfo diskFileInfo(share.fileContent.diskPathname);
//    //now try to remove a file which does not have write permission
//    QCOMPARE(diskFileInfo.exists(), true);
//    QFile::Permissions originalPermissions = diskFileInfo.permissions();
//    QCOMPARE(QFile::setPermissions(share.fileContent.diskPathname, QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther), true);
//    SmbLocationItemFile existentFileButNoWritePermission(share.fileContent.smbUrl);
//    //remove must fail
//    QCOMPARE(existentFileButNoWritePermission.remove(), false);
//    QCOMPARE(errno, EPERM);
//    QCOMPARE(QFile::setPermissions(share.fileContent.diskPathname, originalPermissions), true);

    //positive test, remove fileContent
    SmbLocationItemFile existentFile(share.fileContent.smbUrl);
    SmbItemInfo smbExistentItem(share.fileContent.smbUrl);
    QCOMPARE(smbExistentItem.exists(), true);
    QCOMPARE(existentFile.remove(), true);
    SmbItemInfo smbItem(share.fileContent.smbUrl);
    //samba url for this file must not exist
    QCOMPARE(smbItem.exists(),  false);
    //now origin file in file system must no longer exist
    QCOMPARE(diskFileInfo.exists(), false);
}


void TestQSambaSuite::unit_QFile_open()
{
    ShareCreationStatus share(createTempShare("unit_QFile_open"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.open(QFile::WriteOnly), false);
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileNameDoesNotExist(item.smbUrl);
    QCOMPARE(fileNameDoesNotExist.open(QFile::ReadOnly), false);

    // ---- positive tests
    //create a smb file using open
    item = share.createPathForItem("now_it_must_be_created");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileCreated(item.smbUrl);
    QCOMPARE(fileCreated.open(QFile::WriteOnly), true);
    if (QFileInfo(item.diskPathname).exists() == false)
    {
        fileCreated.close(); // force to close if necessary
    }
    QCOMPARE(fileCreated.isOpen(), true);
    //now check in the local disk to see if it was really created
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    //as QFile it closes itself in the destructor
}


void TestQSambaSuite::unit_QFile_read()
{
    ShareCreationStatus share(createTempShare("unit_QFile_read"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    char buffer[100];

    //get Qt read return when there is no file
    qint64 qt_read_no_file = QFile().read(buffer, sizeof(buffer));

    QCOMPARE(fileNameEmptyDoesNotExist.read(buffer, sizeof(buffer)), qt_read_no_file);
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileMustNotExist(item.smbUrl);
    QCOMPARE(fileMustNotExist.read(buffer, sizeof(buffer)), qt_read_no_file);

    //positive test
    //create a file in the local folder which is shared by Samba
    item = share.createPathForItem("created_in_the_disk.txt");
    QFile inDisk(item.diskPathname);
    QCOMPARE(inDisk.open(QFile::WriteOnly),  true);
    QByteArray diskContent("This a simple content used to test reading files from samba: ");
    QByteArray initialDiskContent(diskContent);
    QByteArray moreDiskContent(100, 'a');
    diskContent += moreDiskContent;
    QCOMPARE(inDisk.write(diskContent),  (qint64)diskContent.size());
    inDisk.close();
    //now read from Samba
    SmbLocationItemFile existentFile(item.smbUrl);
    QCOMPARE(existentFile.open(QFile::ReadOnly),  true);
    char smbBuffer [diskContent.size() + 100];
    //before first read
    QCOMPARE(existentFile.atEnd(),  false);
    qint64 bytesRead    = existentFile.read(smbBuffer, (qint64)initialDiskContent.size());
    qint64 expectedRead = (qint64)initialDiskContent.size();
    QCOMPARE(bytesRead, expectedRead);
    QByteArray  smbContent(smbBuffer,initialDiskContent.size());
    //compare the intial content
    QCOMPARE(smbContent, initialDiskContent);
    //before second read
    QCOMPARE(existentFile.atEnd(),  false);
    expectedRead = (qint64) (diskContent.size() - initialDiskContent.size());
    //read remaining data in the file
    bytesRead    = existentFile.read(smbBuffer, expectedRead);
    QCOMPARE(bytesRead, expectedRead);
    QByteArray   moreSmbContent(smbBuffer, expectedRead);
    smbContent += moreSmbContent;
    //now compare the whole content
    QCOMPARE(smbContent, diskContent);
    //now atEnd() must be true
    QCOMPARE(existentFile.atEnd(),  true);
}


void TestQSambaSuite::unit_QFile_write()
{
    ShareCreationStatus share(createTempShare("unit_QFile_write"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QByteArray someContent("This is a simple content\n");
    //get Qt write return when there is no file
    qint64 qt_write_no_file = QFile().write(someContent.constData(), (qint64)someContent.size());
    QCOMPARE(fileNameEmptyDoesNotExist.write(someContent.constData(), (qint64)someContent.size()), qt_write_no_file);
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileMustNotExist(item.smbUrl);
    QCOMPARE(fileMustNotExist.write(someContent.constData(), (qint64)someContent.size()), qt_write_no_file);

    //positive tests
    //work with 2 items saving in Samba and saving in local file system
    SmbSharedPathAccess items[2];
    items[0]  = share.createPathForItem("first_item.txt");
    items[1]  = share.createPathForItem("second_item.txt");
    QByteArray       content;     // starts empty
    QFile::OpenMode  openMode = QFile::WriteOnly; //after first write it receives Append mode
    const int diskIndex = 0;
    const int smbindex  = 1;
    QByteArray  savedContent;
    for(int counter = 0;  counter < 4;  ++counter, content += someContent)
    {
        QFile                 diskFile(items[diskIndex].diskPathname);
        SmbLocationItemFile   smbFile(items[smbindex].smbUrl);
        //open both files
        QCOMPARE(diskFile.open(openMode), true);
        QCOMPARE(smbFile.open(openMode),  true);
        //first write should return 0 as content is empty
        qint64 toSave = content.size();
        qint64 wrote  = diskFile.write(content.constData(), toSave);
        QCOMPARE(wrote, toSave);
        wrote  = smbFile.write(content.constData(), toSave);
        QCOMPARE(wrote, toSave);
        savedContent += content;
        diskFile.close();
        smbFile.close();
        if (counter > 0)
        {
            openMode = QFile::Append;
        }
    }
   //now check size and content
   QFile                  diskFile(items[diskIndex].diskPathname);
   SmbLocationItemFile    smbFile(items[smbindex].smbUrl);
   QCOMPARE(smbFile.size(),  diskFile.size());
   QCOMPARE(diskFile.open(QFile::ReadOnly), true);
   QCOMPARE(smbFile.open(QFile::ReadOnly), true);
   //read file from disk, check the content
   char buffer [1024];
   qint64 gotBytes = diskFile.read(buffer, sizeof(buffer));
   QCOMPARE((qint64)savedContent.size(),  gotBytes);
   QByteArray otherContent(buffer,  (int)gotBytes);
   QCOMPARE(otherContent, savedContent);
   //read SMB file from disk, check the content
   gotBytes = smbFile.read(buffer, sizeof(buffer));
   QCOMPARE((qint64)savedContent.size(),  gotBytes);
   otherContent.setRawData(buffer, (int)gotBytes);
   QCOMPARE(otherContent, savedContent);
}


void TestQSambaSuite::unit_QFile_atEnd()
{
    ShareCreationStatus share(createTempShare("unit_QFile_atEnd"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.atEnd(), true);
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileMustNotExist(item.smbUrl);
    QCOMPARE(fileMustNotExist.atEnd(), true);
    QFileInfo fileContentInfo(share.fileContent.diskPathname);
    QCOMPARE(fileContentInfo.exists(), true);
    SmbLocationItemFile existentFile(share.fileContent.smbUrl);
    //first at end when file is still closed
    QCOMPARE(existentFile.atEnd(),  true);
    QCOMPARE(existentFile.open(QFile::ReadOnly),  true);
    char buffer [fileContentInfo.size()];
    QCOMPARE(existentFile.read(buffer, (qint64)10), (qint64)10);
    //file is opened and not at end
    QCOMPARE(existentFile.atEnd(),  false);
    QVERIFY(existentFile.read(buffer, fileContentInfo.size()) > 0);
    QCOMPARE(existentFile.atEnd(),  true);
}


void TestQSambaSuite::unit_QFile_size()
{
    ShareCreationStatus share(createTempShare("unit_QFile_size"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    //empty objects
    qint64 qFile_size_for_disk = QFileInfo().size();
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.size(), qFile_size_for_disk);
    //file does not exist
    SmbSharedPathAccess item = share.createPathForItem("it_must_not_exist.txt");
    QFileInfo diskFileInfo(item.diskPathname);
    QCOMPARE(diskFileInfo.exists(), false);
    qFile_size_for_disk = diskFileInfo.size();
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileMustNotExist(item.smbUrl);
    QCOMPARE(fileMustNotExist.size(), qFile_size_for_disk);

    //positive tests
    diskFileInfo.setFile(share.fileContent.diskPathname);
    QCOMPARE(diskFileInfo.exists(), true);
    qFile_size_for_disk = diskFileInfo.size();
    QVERIFY(qFile_size_for_disk > 0);
    SmbLocationItemFile existentFile(share.fileContent.smbUrl);
    //first time the file is closed
    QCOMPARE(existentFile.size(), qFile_size_for_disk);
    QCOMPARE(existentFile.open(QFile::ReadOnly),  true);
    //second time the file is opened
    QCOMPARE(existentFile.size(), qFile_size_for_disk);
    //now append data using local file system
    QFile moreDataFile(share.fileContent.diskPathname);
    QCOMPARE(moreDataFile.open(QFile::Append),  true);
    QByteArray moreData("just a more bytes");
    QCOMPARE(moreDataFile.write(moreData),  (qint64)moreData.size());
    moreDataFile.close();
    //other QFileInfo object to get new information
    QFileInfo newDiskFileInfo(share.fileContent.diskPathname);
    QVERIFY(newDiskFileInfo.size() > qFile_size_for_disk); // has more data now
    QCOMPARE(existentFile.size(), newDiskFileInfo.size());
}


void TestQSambaSuite::unit_QFile_isOpen()
{
    ShareCreationStatus share(createTempShare("unit_QFile_isOpen"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.isOpen(), false);
    SmbSharedPathAccess item = share.createPathForItem("_it_must_not_exist");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemFile fileMustNotExist(item.smbUrl);
    QCOMPARE(fileMustNotExist.isOpen(), false);

    //positive tests
    SmbLocationDirIterator fileContentIterator(share.url, QDir::Files);
    QCOMPARE(fileContentIterator.hasNext(),  true);
    fileContentIterator.next();
    DirItemInfo iteminfo = fileContentIterator.fileInfo();
    QCOMPARE(iteminfo.exists(),  true);
    SmbLocationItemFile  smbFile(iteminfo.urlPath());
    QCOMPARE(smbFile.isOpen(), false);
    QCOMPARE(smbFile.open(QFile::ReadOnly), true);
    QCOMPARE(smbFile.isOpen(), true);
}


void TestQSambaSuite::unit_QFile_permissions()
{
    ShareCreationStatus share(createTempShare("unit_QFile_permissions"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    //empty objects
    QFile::Permissions qt_empty_permissions = QFileInfo().permissions();
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.permissions(), qt_empty_permissions);
    //file does not exist
    SmbSharedPathAccess item = share.createPathForItem("it_must_not_exist.txt");
    QFileInfo diskFileInfo(item.diskPathname);
    QCOMPARE(diskFileInfo.exists()  ,false);
    SmbLocationItemFile fileNameDoesNotExist(item.smbUrl);
    QCOMPARE(fileNameDoesNotExist.permissions(), qt_empty_permissions);

    //positive test, change in the local disk , check for permission in Samba
    QFile::Permissions perm_readOnly = QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther  | QFile::ReadUser;

    QFileInfo diskExistentFile(share.fileContent.diskPathname);
    QCOMPARE(diskExistentFile.exists(), true);
    //first make sure permissions are different
    QVERIFY(diskExistentFile.permissions() != perm_readOnly);
    //set permission using file in the local file system
    QCOMPARE(QFile::setPermissions(share.fileContent.diskPathname, perm_readOnly), true);
    //check the permission in the local file system
    QTest::qWait(100);
    QFile::Permissions curr_permissions = QFile(share.fileContent.diskPathname).permissions();
    QCOMPARE(curr_permissions, perm_readOnly);
    //now the same permission must come using Samba
    SmbLocationItemFile smbExistentFile(share.fileContent.smbUrl);
    QFile::Permissions curr_smb_ermissions = smbExistentFile.permissions();
    QCOMPARE(curr_smb_ermissions, perm_readOnly);
}


void TestQSambaSuite::unit_QFile_setPermissions()
{    
    ShareCreationStatus share(createTempShare("unit_QFile_setPermissions"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    //empty objects
    QFile::Permissions qt_permissions = QFile::ReadOwner | QFile::ReadUser | QFile::WriteOwner | QFile::WriteUser;
    SmbLocationItemFile fileNameEmptyDoesNotExist;
    QCOMPARE(fileNameEmptyDoesNotExist.setPermissions(qt_permissions), false);
    //file does not exist
    SmbSharedPathAccess item = share.createPathForItem("it_must_not_exist.txt");
    QFileInfo diskFileInfo(item.diskPathname);
    QCOMPARE(diskFileInfo.exists()  ,false);
    SmbLocationItemFile fileNameDoesNotExist(item.smbUrl);
    QCOMPARE(fileNameDoesNotExist.setPermissions(qt_permissions), false);

    QCOMPARE(fileNameDoesNotExist.setPermissions(share.fileContent.smbUrl, qt_permissions), true);
    SmbLocationItemFile smbFile(share.fileContent.smbUrl);
    QCOMPARE(smbFile.setPermissions(qt_permissions), true);
}


void TestQSambaSuite::unit_QDir_exists()
{
    ShareCreationStatus share(createTempShare("unit_QDir_mkdir"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemDir directoryEmptyName;
    QCOMPARE(directoryEmptyName.exists(),  false);
    SmbSharedPathAccess item =  share.createPathForItem("dirName");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemDir directoryDoesNotExist(item.smbUrl);
    QCOMPARE(directoryDoesNotExist.exists(),  false);

    //positve test
    //item.smbUrl does not exist, check above
    //create it in the disk
    QCOMPARE(QDir().mkpath(item.diskPathname), true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    SmbLocationItemDir  directoyExists(item.smbUrl);
    QCOMPARE(directoyExists.exists(),  true);
}




void TestQSambaSuite::unit_QDir_mkdir()
{
    ShareCreationStatus share(createTempShare("unit_QDir_mkdir"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemDir directoryDoesNotExist;
    QCOMPARE(directoryDoesNotExist.mkdir("relativeDir"), false);
    QCOMPARE(directoryDoesNotExist.mkdir("smb://localhost/share_does_not_exist"), false);
    QCOMPARE(directoryDoesNotExist.mkdir("smb://localhost/share_does_not_exist/invalid_share"), false);

    //positive tests

    //create relative directory
    QString relativeName("relativeDir");
    SmbSharedPathAccess item =  share.createPathForItem(relativeName);
    SmbLocationItemDir testRelative(share.url);
    SmbItemInfo smbDirBeforeCreation(item.smbUrl);
    //make sure directory does not exist
    QCOMPARE(smbDirBeforeCreation.exists(), false);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    //create directory using Samba
    QCOMPARE(testRelative.mkdir(relativeName), true);
    //make sure it exists now
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    SmbItemInfo smbDirAfterCreation(item.smbUrl);
    QCOMPARE(smbDirAfterCreation.exists(), true);

    //directory already exists, should return true
    QCOMPARE(testRelative.mkdir(relativeName), true);

    //mkdir using absolute path
    item = share.createPathForItem("using_absolute_path");
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemDir testAbsoluteDir;
    QCOMPARE(testAbsoluteDir.mkdir(item.smbUrl), true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
}



void TestQSambaSuite::unit_QDir_mkpath()
{
     ShareCreationStatus share(createTempShare("unit_QDir_mkpath"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //create multiple paths
    QStringList paths("path1");
    paths.append("path2");
    paths.append("path3");
    //item will have smbUrl=smb://localhost/shareName/path1/path2/path3 and diskPathname=/tmp/tmpdir/path1/path2/path3
    //using absolute path
    SmbSharedPathAccess item =  share.createPathForItems(paths);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemDir testAbsoluteDir;
    QCOMPARE(testAbsoluteDir.mkpath(item.smbUrl),  true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    //test when it already exists
    QCOMPARE(testAbsoluteDir.mkpath(item.smbUrl),  true);

    //relative paths
    paths.clear();
    paths.append("relative1");
    paths.append("relative2");
    item =  share.createPathForItems(paths);
    SmbLocationItemDir  relativeDir(share.url);
    QString multiplePaths = paths.join(QDir::separator());
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    QCOMPARE(relativeDir.mkpath(multiplePaths), true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
}



void TestQSambaSuite::unit_QDir_rmdir()
{
    ShareCreationStatus share(createTempShare("unit_QDir_rmdir"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    //negative tests
    SmbLocationItemDir directoryEmptyName;
    QCOMPARE(directoryEmptyName.rmdir("none"),  false);
    QString dirName("dirName");
    SmbSharedPathAccess item =  share.createPathForItem(dirName);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    SmbLocationItemDir directoryDoesNotExist(share.url);
    QCOMPARE(directoryDoesNotExist.rmdir(dirName),  false);

    //create a directory
    SmbLocationItemDir   directory(share.url);
    //create using relative
    QCOMPARE(directory.mkdir(dirName),  true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    //remove the relative directory
    QCOMPARE(directory.rmdir(dirName), true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
    //create it again and now remove using absolute url
    QCOMPARE(directory.mkdir(dirName),  true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), true);
    SmbLocationItemDir emptyObject;
    QCOMPARE(emptyObject.rmdir(item.smbUrl), true);
    QCOMPARE(QFileInfo(item.diskPathname).exists(), false);
}


void TestQSambaSuite::unit_QDirIterator_path()
{
    ShareCreationStatus share(createTempShare("unit_QDirIterator_path"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);
    QStringList paths = QStringList() << "path1"    << "anotherPath" << "testdir"      << "passThru"   << "haha";
    QStringList files = QStringList() << "text.txt" << "hiper.html"  << "document.odf" << "spread.odx" << "tx.list";

    QStringList curPath;
    for (int counter=0; counter < paths.count() && counter < files.count(); ++counter)
    {
        curPath.append(paths.at(counter));
        SmbSharedPathAccess item =  share.createPathForItems(curPath);
        QCOMPARE(QDir().mkpath(item.diskPathname), true);
        QString fileFullPath = item.diskPathname + QDir::separator() + files.at(counter);
        QFile file(fileFullPath);
        QCOMPARE(file.open(QFile::WriteOnly),  true);
        file.close();
    }

    QDir::Filters dirFilter = QDir::NoDotAndDotDot;
    QDir::Filters moreFilter[] = { QDir::AllEntries,  QDir::Hidden,  QDir::System };

    for (uint counter=0; counter < sizeof(moreFilter)/sizeof(moreFilter[0]); ++counter)
    {
        dirFilter |= moreFilter[counter];
        QDirIterator dirIterator(share.sharedDirPath, dirFilter, QDirIterator::Subdirectories);
        SmbLocationDirIterator smbIterator(share.url, dirFilter, QDirIterator::Subdirectories);
        QStringList listDir;
        QStringList listSmb;
        qDebug() << "\n=================";
        while (dirIterator.hasNext() && smbIterator.hasNext())
        {
            qDebug() << dirIterator.next()  << smbIterator.next();
            listDir.append(dirIterator.fileName());
            listSmb.append(smbIterator.fileName());
        }
        QCOMPARE(listDir, listSmb);
    }
}


void TestQSambaSuite::positive_statvfs()
{
    ShareCreationStatus share(createTempShare("positive_statvfs"));
    if (share.tempDir)
    {
        share.tempDir->setAutoRemove(true);
    }
    QCOMPARE(share.status, true);

    struct statvfs diskVfs;
    struct statvfs smbVfs;
    ::memset(&diskVfs, 0, sizeof(struct statvfs));
    ::memset(&smbVfs, 0, sizeof(struct statvfs));

    //using a file that exists
    SmbUtil smb;
    QCOMPARE((int)smb.getStatvfsInfo(share.fileContent.smbUrl, &smbVfs), 0);
    QCOMPARE(::statvfs(share.fileContent.diskPathname.toLocal8Bit().constData(), &diskVfs), 0);
    QCOMPARE(smbVfs.f_blocks, diskVfs.f_blocks);
    QCOMPARE(smbVfs.f_ffree,  diskVfs.f_ffree);
    QCOMPARE(smbVfs.f_files,  diskVfs.f_files);
    QCOMPARE(smbVfs.f_bsize,  diskVfs.f_bsize);
    QVERIFY( qAbs(smbVfs.f_bfree - diskVfs.f_bfree) < 10 );

    //using a directory
    ::memset(&diskVfs, 0, sizeof(struct statvfs));
    ::memset(&smbVfs, 0, sizeof(struct statvfs));
    QCOMPARE((int)smb.getStatvfsInfo(share.url, &smbVfs), 0);
    QCOMPARE(::statvfs(share.sharedDirPath.toLocal8Bit().constData(), &diskVfs), 0);
    QCOMPARE(smbVfs.f_blocks, diskVfs.f_blocks);
    QCOMPARE(smbVfs.f_ffree,  diskVfs.f_ffree);
    QCOMPARE(smbVfs.f_files,  diskVfs.f_files);
    QCOMPARE(smbVfs.f_bsize,  diskVfs.f_bsize);
    QVERIFY( qAbs(smbVfs.f_bfree - diskVfs.f_bfree) < 10 );
}

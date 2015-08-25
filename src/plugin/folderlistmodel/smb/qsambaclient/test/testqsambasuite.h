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
 * File: testqsambasuite.h
 * Date: 03/12/2014
 */

#ifndef TESTQSAMBASUITE_H
#define TESTQSAMBASUITE_H

#include <QObject>

class SmbPlaces;
class QTemporaryDir;

//for a local share in the localhost
struct SmbSharedPathAccess
{
    QString diskPathname;  //full path name for disk access
    QString smbUrl;        //full path name for smb access
};

struct ShareCreationStatus
{
   ShareCreationStatus(const QString& dirNameMask);
   ShareCreationStatus(const ShareCreationStatus & o);
   ShareCreationStatus(ShareCreationStatus & o);
   ~ShareCreationStatus();
   ShareCreationStatus &operator=(const ShareCreationStatus & o);
   ShareCreationStatus &operator=(ShareCreationStatus & o);
   SmbSharedPathAccess createPathForItem(const QString& item);
   SmbSharedPathAccess createPathForItems(const QStringList& items);
   SmbSharedPathAccess fileContent; // a simple file is created in evey share
   QString sharedDirPath;  //share path in the localhost
   QString shareName;      //share name  
   QString url;            //share url
   bool    status;
   QTemporaryDir * tempDir;
private:
   ShareCreationStatus() {} // not allowed
};



class TestQSambaSuite : public QObject
{
    Q_OBJECT
public:
    explicit TestQSambaSuite(QObject *parent = 0);
     ~TestQSambaSuite();

public:
    bool     createPermanentShare(const QString& path, bool fullAccess=true);
    bool     openPermanenteShare(const QString& smb_path);
    bool     listLocalhost();
    bool     listRecursive();
    bool     existsShare(const QStringList& sharesFullPathList, const QString& shareName);
    ShareCreationStatus createTempShare(const QString& maskName, bool fullAccess = true, bool allowGuests = true);

private Q_SLOTS:
    void initTestCase();       //before all tests
    void cleanupTestCase();    //after all tests
    void init();               //before every test
    void cleanup();            //after every test

private Q_SLOTS: // test cases
    void positive_createReadOnlyUserShare();
    void positive_createFullAccessUserShare();
    void positive_itemInfoRoot();
    void positive_itemInfoLocalHost();
    void positive_itemInfoShare();
    void positive_itemInfoCommonPermissions();
    void positive_dirIterator();
    void negative_itemInfo();
    void negative_createShareDirDoesNotExist();
    void negative_emptyItemInfo();
    void negative_dirIterator();

private Q_SLOTS:   //unit test for SmbLocationItemFile class
    void unit_QFile_rename();
    void unit_QFile_remove();
    void unit_QFile_open();
    void unit_QFile_read();
    void unit_QFile_write();  
    void unit_QFile_atEnd();
    void unit_QFile_size();
    void unit_QFile_isOpen();
    void unit_QFile_setPermissions();
    void unit_QFile_permissions();

private Q_SLOTS:  //unit test for SmbLocationItemDir class
    void unit_QDir_exists();
    void unit_QDir_mkdir();
    void unit_QDir_mkpath();
    void unit_QDir_rmdir();

private Q_SLOTS: //unit test for SmbLocationItemDirIterator class
    void unit_QDirIterator_path();

private Q_SLOTS:
    void positive_statvfs();

protected:   
    QString createTempFile(const QString& path,
                           const QString& name,
                           const QByteArray& content = QByteArray(0));   

private:
    SmbPlaces      *m_smbShares;
    QString        m_curShareName;
    mode_t         m_curUmask;

};

#endif // TESTQSAMBASUITE_H

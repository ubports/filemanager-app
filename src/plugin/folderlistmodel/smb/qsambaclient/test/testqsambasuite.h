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

struct ShareCreationStatus
{
   ShareCreationStatus(const QString& dirNameMask);
   ShareCreationStatus(const ShareCreationStatus & o);
   ShareCreationStatus(ShareCreationStatus & o);
   ~ShareCreationStatus();
   ShareCreationStatus &operator=(const ShareCreationStatus & o);
   ShareCreationStatus &operator=(ShareCreationStatus & o);
   QString sharedDirPath;
   QString shareName;
   QString fileContent;
   QString url;
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


protected:   
    QString createTempFile(const QString& path,
                           const QString& name,
                           const QByteArray& content = QByteArray(0));   

private:
    SmbPlaces      *m_smbShares;
    QString        m_curShareName;

};

#endif // TESTQSAMBASUITE_H

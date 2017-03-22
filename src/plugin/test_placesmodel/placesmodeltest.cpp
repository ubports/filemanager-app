/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author : David Planella <david.planella@ubuntu.com>
 */

#include "placesmodel.h"

#include <QString>
#include <QtTest>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QTemporaryDir>


class SaveSettings: public QTemporaryFile
{
public:
       SaveSettings(QObject *parent = 0) : QTemporaryFile(parent) {}
       bool openReadOnly()
       {
           return open(QFile::ReadOnly);
       }
};

class PlacesmodelTest : public QObject
{
    Q_OBJECT

public:
    PlacesmodelTest();

private Q_SLOTS:
    void init();
    void cleanup();
    void cleanupTestCase();
    void addUserPlace();
    void removeUserPlace();
    void addExistentDefaultPlace(); // should fail
    void removeDefaultPlace();
    void addRemovedDefaultPlace();
private:
    SaveSettings *m_saved_settings;
};


PlacesmodelTest::PlacesmodelTest() : m_saved_settings(0)
{

}

void PlacesmodelTest::init()
{
    PlacesModel places;
    QFileInfo saved(places.m_settings->fileName());
    if (saved.exists())
    {
        QFile settings(saved.absoluteFilePath());
        QCOMPARE(settings.open(QFile::ReadOnly), true);
        m_saved_settings = new SaveSettings(this);
        QCOMPARE(m_saved_settings->open(), true);
        QByteArray settings_data = settings.readAll();
        QCOMPARE(m_saved_settings->write(settings_data), (qint64)settings_data.size());
        m_saved_settings->close();
    }
}

void PlacesmodelTest::cleanupTestCase()
{
    if (m_saved_settings)
    {
        QCOMPARE(m_saved_settings->openReadOnly(), true);
        PlacesModel places;
        QFile  saved(places.m_settings->fileName());
        QCOMPARE(saved.open(QFile::WriteOnly | QFile::Truncate), true);
        QByteArray saved_data = m_saved_settings->readAll();
        QCOMPARE(saved.write(saved_data), (qint64)saved_data.size());
    }
}

void PlacesmodelTest::cleanup()
{
    cleanupTestCase();
}

void PlacesmodelTest::addUserPlace()
{
    QTemporaryDir tempDir;
    PlacesModel   places;
    int  locations_counter = places.rowCount();
    QCOMPARE(places.indexOfLocation(tempDir.path()), -1);
    places.addLocation(tempDir.path());
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), locations_counter + 1);
    QVERIFY(places.indexOfLocation(tempDir.path()) != -1);

    //now try to add it again which must fail
    places.addLocation(tempDir.path());
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), locations_counter + 1);

    //another model instance
    PlacesModel places2;
    QVERIFY(places2.indexOfLocation(tempDir.path()) != -1);
    //added item must be in m_userSavedLocations
    QVERIFY(places2.m_userSavedLocations.indexOf(tempDir.path()) != -1);
    QCOMPARE(places2.rowCount(), locations_counter + 1);
}

void PlacesmodelTest::removeUserPlace()
{
    // first add a temporary place
    QTemporaryDir tempDir;
    PlacesModel   places;
    int  locations_counter = places.rowCount();
    QCOMPARE(places.indexOfLocation(tempDir.path()), -1);
    places.addLocation(tempDir.path());
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), locations_counter + 1);
    QVERIFY(places.indexOfLocation(tempDir.path()) != -1);
    //then remove it
    places.removeItem(tempDir.path());
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), locations_counter);
    QCOMPARE(places.indexOfLocation(tempDir.path()), -1);

    //another PlacesModel instance
    PlacesModel places2;
    QCOMPARE(places2.rowCount(), locations_counter);
    //item removed is not in the m_locations
    QCOMPARE(places2.indexOfLocation(tempDir.path()), -1);
    //item removed is in m_userRemovedLocations
    QVERIFY(places2.m_userRemovedLocations.indexOf(tempDir.path()) != -1);
}

void PlacesmodelTest::addExistentDefaultPlace()
{
    PlacesModel places;
    int home_index = places.indexOfLocation(QDir::homePath());
    QVERIFY(home_index != -1);
    int places_counter = places.rowCount();
    places.addLocation(QDir::homePath());
    QTest::qWait(50);
    //counter must be the same which indicates nothing was added
    QCOMPARE(places.rowCount(), places_counter);
}

void PlacesmodelTest::removeDefaultPlace()
{
    PlacesModel places;
    int home_index = places.indexOfLocation(QDir::homePath());
    QVERIFY(home_index != -1);
    int places_counter = places.rowCount();
    places.removeItem(home_index);
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), places_counter - 1);
    QCOMPARE(places.indexOfLocation(QDir::homePath()), -1);

    //use another instance to check
    PlacesModel places2;
    QCOMPARE(places2.rowCount(), places_counter - 1);
    QCOMPARE(places2.indexOfLocation(QDir::homePath()), -1);
    //default place is in m_userRemovedLocations
    QVERIFY(places2.m_userRemovedLocations.indexOf(QDir::homePath()) != -1);
}

void PlacesmodelTest::addRemovedDefaultPlace()
{
    //first remove a default place
    PlacesModel places;
    int home_index = places.indexOfLocation(QDir::homePath());
    QVERIFY(home_index != -1);
    int places_counter = places.rowCount();
    places.removeItem(home_index);
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), places_counter - 1);
    QCOMPARE(places.indexOfLocation(QDir::homePath()), -1);

    //now add the default location back
    places.addLocation(QDir::homePath());
    QTest::qWait(50);
    QCOMPARE(places.rowCount(), places_counter);
    QVERIFY(places.indexOfLocation(QDir::homePath()) != -1);
    //it must not exist neither on m_userSavedLocations nor m_userRemovedLocations
    QCOMPARE(places.m_userSavedLocations.indexOf(QDir::homePath()),   -1);
    QCOMPARE(places.m_userRemovedLocations.indexOf(QDir::homePath()), -1);

    //check on a second instance
    PlacesModel places2;
    QVERIFY(places2.indexOfLocation(QDir::homePath()) != -1);
    QCOMPARE(places2.m_userSavedLocations.indexOf(QDir::homePath()),   -1);
    QCOMPARE(places2.m_userRemovedLocations.indexOf(QDir::homePath()), -1);
}

QTEST_MAIN(PlacesmodelTest)

#include "placesmodeltest.moc"

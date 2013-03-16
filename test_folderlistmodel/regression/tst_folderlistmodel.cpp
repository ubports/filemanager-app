#include "filesystemaction.h"
#include "dirmodel.h"

#include <stdio.h>

#include <QApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QFileInfo>
#include <QMetaType>




class DeepDir
{
public:
    DeepDir(const QString& rootDir, int level);
    ~ DeepDir()
    {
        remove();
    }
    bool remove();
    QString path()   { return root;}
private:
    QString  root;
};


class TempFiles
{
public:
    TempFiles();
    bool addSubDirLevel(const QString&dir);
    bool create(int counter =1);
    bool create(const QString& name, int counter = 1);
    QString lastCreated();
    QStringList createdList()   { return m_filesCreated; }
    int      created()          { return m_filesCreated.count();}
    int      howManyExist();
    void removeAll();
    QStringList createdNames();
    QString   lastPath()        { return m_dir; }
private:
    QString       m_dir;
    QStringList   m_filesCreated;
    QByteArray    m_content;
};


TempFiles::TempFiles() : m_content(QByteArray(1010, 'z'))
{
    m_dir = QDir::tempPath();
}


bool TempFiles::addSubDirLevel(const QString &dir)
{
    QFileInfo d( m_dir + QDir::separator() + dir);
    if (d.exists()  || QDir().mkpath(d.absoluteFilePath()))
    {
        m_dir = d.absoluteFilePath();
        return true;
    }
    return false;
}

void TempFiles::removeAll()
{
    int counter = m_filesCreated.count();
    while(counter--)
    {
        if (QFileInfo(m_filesCreated.at(counter)).exists())
        {
            QFile::remove(m_filesCreated.at(counter));
            m_filesCreated.removeAt(counter);
        }
    }
}


QStringList TempFiles::createdNames()
{
    QStringList names;
    int counter = m_filesCreated.count();
    while(counter--) {
        names.append(QFileInfo(m_filesCreated.at(counter)).fileName());
    }
    return names;
}

int TempFiles::howManyExist()
{
    int ret = 0;
    int counter = m_filesCreated.count();
    while(counter--)
    {
        if (QFileInfo(m_filesCreated.at(counter)).exists())
        {
           ret++;
        }
    }
    return ret;
}

bool TempFiles::create(int counter)
{
   return  create(QLatin1String("tempfile"), counter);
}

bool TempFiles::create(const QString& name, int counter )
{
    QString myName;
    while(counter--)
    {
        myName.sprintf("%s%c%s_%02d", m_dir.toLatin1().constData(),
                       QDir::separator().toLatin1(),
                       name.toLatin1().constData(),
                       counter);
        QFile file(myName);
        if (file.open(QFile::WriteOnly))
        {
            if (file.write(m_content) == (qint64)m_content.size())
            {
                m_filesCreated.append(myName);
                m_content += "azaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }

    }
    return true;
}

QString TempFiles::lastCreated()
{
    QString ret;
    if (m_filesCreated.count() > 0)
    {
        ret = m_filesCreated.at(m_filesCreated.count() -1);
    }
    return ret;
}

DeepDir::DeepDir(const QString &rootDir, int level) :
    root(QDir::tempPath() + QDir::separator() + rootDir)
{  
    if (!rootDir.isEmpty())
    {
        remove(); // clear        
        QString levelStr;
        TempFiles temp;
        if (temp.addSubDirLevel(rootDir))
        {
            for(int counter=1 ; counter <= level; counter++)
            {
                levelStr.sprintf("level_%02d", counter);
                if ( !temp.addSubDirLevel(levelStr) || !temp.create(2) )
                {
                    break;
                }
            }
        }
    }
    else
    {
        root.clear();
    }
}

bool DeepDir::remove()
{
    bool ret = false;
    if (!root.isEmpty() && QFileInfo(root).exists())
    {
        QString cmd("/bin/rm -rf " + root);
        ret = ::system(cmd.toLatin1().constData()) == 0 ;
    }
    return ret;
}



class TestFolderModel : public QObject
{
   Q_OBJECT

public:
       TestFolderModel();
      ~TestFolderModel();

protected slots:
    void slotFileAdded(const QString& s)     {m_filesAdded.append(s); }
    void slotFileRemoved(const QString& s)   {m_filesRemoved.append(s); }
    void slotFileAdded(const QFileInfo& f)   {m_filesAdded.append(f.absoluteFilePath()); }
    void slotFileRemoved(const QFileInfo& f) {m_filesRemoved.append(f.absoluteFilePath()); }
    void progress(int, int, int);

private Q_SLOTS:
    void initTestCase();       //before all tests
    void cleanupTestCase();    //after all tests
    void init();               //before every test
    void cleanup();            //after every test

    void  fsActionRemoveSingleFile();
    void  fsActionRemoveSingleDir();
    void  fsActionRemoveOneFileOneDir();
    void  fsActionRemoveTwoFilesTwoDirs();
    void  modelRemoveRecursiveDirByIndex();
    void  modelRemoveMultiItemsByFullPathname();
    void  modelRemoveMultiItemsByName();

private:
    void initDeepDirs();
    void cleanDeepDirs();
    void initModels();
    void cleanModels();

private:
    FileSystemAction  fsAction;

    QStringList m_filesAdded;
    QStringList m_filesRemoved;

    DeepDir   *    m_deepDir_01;
    DeepDir   *    m_deepDir_02;
    DeepDir   *    m_deepDir_03;

    DirModel  *    m_dirModel_01;
    DirModel  *    m_dirModel_02;

    int            m_progressCounter;
    int            m_progressTotalItems;
    int            m_progressCurrentItem;
    int            m_progressPercentDone;
    
};

TestFolderModel::TestFolderModel() : m_deepDir_01(0)
                                    ,m_deepDir_02(0)
                                    ,m_deepDir_03(0)
                                    ,m_dirModel_01(0)
                                    ,m_dirModel_02(0)
{
    connect(&fsAction, SIGNAL(added(QString)),
            this,      SLOT(slotFileAdded(QString)) );
    connect(&fsAction, SIGNAL(removed(QString)),
            this,      SLOT(slotFileRemoved(QString)) );

    connect(&fsAction, SIGNAL(added(QFileInfo)),
            this,      SLOT(slotFileAdded(QFileInfo)));
    connect(&fsAction, SIGNAL(removed(QFileInfo)),
            this,      SLOT(slotFileRemoved(QFileInfo)));

    connect(&fsAction, SIGNAL(progress(int,int,int)),
            this,      SLOT(progress(int,int,int)));
}

void TestFolderModel::progress(int cur, int total, int percent)
{
    m_progressCounter++; 
}


TestFolderModel::~TestFolderModel()
{

}

void TestFolderModel::initDeepDirs()
{
    cleanDeepDirs();
}


void TestFolderModel::cleanDeepDirs()
{
    if (m_deepDir_01) delete m_deepDir_01;
    if (m_deepDir_02) delete m_deepDir_02;
    if (m_deepDir_03) delete m_deepDir_03;
    m_deepDir_01 = 0;
    m_deepDir_02 = 0;
    m_deepDir_03 = 0;
}


void TestFolderModel::initModels()
{
    cleanModels();
}


void TestFolderModel::cleanModels()
{
    if (m_dirModel_01) delete m_dirModel_01;
    if (m_dirModel_02) delete m_dirModel_02;
    m_dirModel_01 = 0;
    m_dirModel_02 = 0;
}

void TestFolderModel::initTestCase()
{
   qRegisterMetaType<QVector<QFileInfo> >();
}


void TestFolderModel::cleanupTestCase()
{
    cleanDeepDirs();
    cleanModels();
}


void TestFolderModel::init()
{  
   m_filesAdded.clear();
   m_filesRemoved.clear();
   initDeepDirs();
   initModels();
   m_progressCounter = 0;
   m_progressTotalItems = 0;
   m_progressCurrentItem = 0;
   m_progressPercentDone = 0;
}



void TestFolderModel::cleanup()
{   
    cleanDeepDirs();
    cleanModels();
}


void TestFolderModel::fsActionRemoveSingleFile()
{
    TempFiles file;
    QCOMPARE(file.create("fsAtion_removeSingleFile") , true);

    fsAction.remove(file.createdList());
    QTest::qWait(700);
    QCOMPARE(m_filesRemoved.count() , 1);

    QCOMPARE(m_filesRemoved.at(0), file.lastCreated());
    QFileInfo now(file.lastCreated());
    QCOMPARE(now.exists(),  false);
}



void TestFolderModel::fsActionRemoveSingleDir()
{
    const int level = 30;
    m_deepDir_01 = new DeepDir("removeSingleDir", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    QStringList myDeepDir(m_deepDir_01->path());
    fsAction.remove(myDeepDir);
    QTest::qWait(900);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  false);
    QVERIFY(m_progressCounter > 2);
}


void TestFolderModel::fsActionRemoveOneFileOneDir()
{
    const int level = 25;
    m_deepDir_01 = new DeepDir("removeSingleDir", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    TempFiles file;
    QCOMPARE(file.create("fsActionRemoveOneFileOneDir") , true);

    QStringList twoItems(m_deepDir_01->path());
    twoItems.append(file.lastCreated());
    fsAction.remove(twoItems);
    QTest::qWait(900);

    QCOMPARE(m_filesRemoved.count() , 2);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  false);
    QCOMPARE(file.howManyExist(),  0);
    QVERIFY(m_progressCounter > 2);
}



void TestFolderModel::fsActionRemoveTwoFilesTwoDirs()
{
    const int level = 12;
    m_deepDir_01 = new DeepDir("fsActionRemoveTwoFilesTwoDirs_01", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    m_deepDir_02 = new DeepDir("fsActionRemoveTwoFilesTwoDirs_02", level);
    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);

    TempFiles twoFiles;
    QCOMPARE(twoFiles.create(2) , true);

    QStringList fourItems(m_deepDir_01->path());
    fourItems.append(m_deepDir_02->path());
    fourItems.append(twoFiles.createdList());

    fsAction.remove(fourItems);
    QTest::qWait(900);

    QCOMPARE(m_filesRemoved.count() , 4);
    QCOMPARE(QFileInfo(m_deepDir_01->path()).exists(),  false);
    QCOMPARE(QFileInfo(m_deepDir_02->path()).exists(),  false);
    QCOMPARE(twoFiles.howManyExist(),  0);
    QVERIFY(m_progressCounter > 2);
}

void TestFolderModel::modelRemoveRecursiveDirByIndex()
{
    const int level = 5;
    m_deepDir_01 = new DeepDir("modelRemoveRecursiveDirByIndex", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(50);

    QCOMPARE(m_dirModel_01->rowCount(QModelIndex()), 1);
    m_dirModel_01->remove(0);
    QTest::qWait(500);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE(m_dirModel_01->rowCount(QModelIndex()), 0);
}

void TestFolderModel::modelRemoveMultiItemsByFullPathname()
{
    QString tmpDir("modelRemoveMultiItemsByFullPathame");
    m_deepDir_01 = new DeepDir(tmpDir, 0);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    TempFiles files;
    files.addSubDirLevel(tmpDir);
    files.create(2);  // 2 files

    QStringList items (files.createdList());

    files.addSubDirLevel("subDir"); // + 1 item
    files.create(1);
    items.append(files.lastPath());

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(50);

    QCOMPARE(m_dirModel_01->rowCount(QModelIndex()), 3);
    m_dirModel_01->rm(items);
    QTest::qWait(500);

    QCOMPARE(m_filesRemoved.count() , 3);
    QCOMPARE(m_dirModel_01->rowCount(QModelIndex()), 0);
    QCOMPARE(files.howManyExist(), 0);
}


void TestFolderModel::modelRemoveMultiItemsByName()
{
     QString tmpDir("modelRemoveMultiItemsByName");

     TempFiles files;
     files.addSubDirLevel(tmpDir);
     files.create(4);
     QCOMPARE(files.howManyExist(), 4);

     m_dirModel_01 = new DirModel();
     m_dirModel_01->setPath(files.lastPath());
     QTest::qWait(50);

     QStringList items(files.createdNames());
     m_dirModel_01->rm(items);
     QTest::qWait(500);

     QCOMPARE(m_filesRemoved.count() , 4);
     QCOMPARE(m_dirModel_01->rowCount(QModelIndex()),    0);
     QCOMPARE(files.howManyExist(), 0);
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestFolderModel tc;
    int ret = QTest::qExec(&tc, argc, argv);

    return ret;
}


#include "tst_folderlistmodel.moc"

#include "filesystemaction.h"
#include "dirmodel.h"
#include "tempfiles.h"
#include <stdio.h>

#include <QApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QFileInfo>
#include <QMetaType>

#define TIME_TO_PROCESS       1000
#define TIME_TO_REFRESH_DIR   80

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
    void cancel(int,int,int);

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
    void  modelCopyDirPasteIntoAnotherModel();
    void  modelCopyManyItemsPasteIntoAnotherModel();
    void  modelCutManyItemsPasteIntoAnotherModel();
    void  fsActionMoveItemsForcingCopyAndThenRemove();
    void  modelCancelRemoveAction();
    void  modelTestFileSize();

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


void TestFolderModel::cancel(int, int, int)
{
    DirModel * model = static_cast<DirModel*> (sender());
    model->cancelAction();
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
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(m_filesRemoved.count() , 1);

    QCOMPARE(m_filesRemoved.at(0), file.lastFileCreated());
    QFileInfo now(file.lastFileCreated());
    QCOMPARE(now.exists(),  false);
}



void TestFolderModel::fsActionRemoveSingleDir()
{
    const int level = 30;
    m_deepDir_01 = new DeepDir("removeSingleDir", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    QStringList myDeepDir(m_deepDir_01->path());
    fsAction.remove(myDeepDir);
    QTest::qWait(TIME_TO_PROCESS);

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
    twoItems.append(file.lastFileCreated());
    fsAction.remove(twoItems);
    QTest::qWait(TIME_TO_PROCESS);

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
    QTest::qWait(TIME_TO_PROCESS);

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
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE(m_dirModel_01->rowCount(), 1);
    m_dirModel_01->remove(0);
    QTest::qWait(500);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE(m_dirModel_01->rowCount(), 0);
}

void TestFolderModel::modelRemoveMultiItemsByFullPathname()
{
    QString tmpDir("modelRemoveMultiItemsByFullPathame");
    m_deepDir_01 = new DeepDir(tmpDir, 0);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    const int filesToCreate = 2;
    const int itemsToCreate = filesToCreate + 1;

    TempFiles files;
    files.addSubDirLevel(tmpDir);
    files.create(filesToCreate);

    QStringList items (files.createdList());

    files.addSubDirLevel("subDir"); // + 1 item
    files.create(1);
    items.append(files.lastPath());

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE(m_dirModel_01->rowCount(), itemsToCreate);
    m_dirModel_01->rm(items);
    QTest::qWait(500);

    QCOMPARE(m_filesRemoved.count() , itemsToCreate);
    QCOMPARE(m_dirModel_01->rowCount(), 0);
    QCOMPARE(files.howManyExist(), 0);
}


void TestFolderModel::modelRemoveMultiItemsByName()
{
     QString tmpDir("modelRemoveMultiItemsByName");
     const int filesToCreate = 4;

     TempFiles files;
     files.addSubDirLevel(tmpDir);
     files.create(4);
     QCOMPARE(files.howManyExist(), filesToCreate);

     m_dirModel_01 = new DirModel();
     m_dirModel_01->setPath(files.lastPath());
     QTest::qWait(TIME_TO_REFRESH_DIR);

     QStringList items(files.createdNames());
     m_dirModel_01->remove(items);
     QTest::qWait(500);

     QCOMPARE(m_filesRemoved.count() , filesToCreate);
     QCOMPARE(m_dirModel_01->rowCount(),    0);
     QCOMPARE(files.howManyExist(), 0);

     QDir().rmdir(m_dirModel_01->path());
}


void TestFolderModel::modelCopyDirPasteIntoAnotherModel()
{
    QString orig("modelCopyDirToAnotherModel_orig");

    m_deepDir_01 = new DeepDir(orig, 1);
    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(m_dirModel_01->rowCount(),  1);

    QString target("modelCopyDirToAnotherModel_target");
    m_deepDir_02 = new DeepDir(target, 0);
    m_dirModel_02 = new DirModel();
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);

    m_dirModel_01->copy(0);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  1);
}


void TestFolderModel::modelCopyManyItemsPasteIntoAnotherModel()
{
    QString orig("modelCopyManyItemstoAnotherModel_orig");

    m_deepDir_01  = new DeepDir(orig, 5);
    m_dirModel_01 = new DirModel();
    const int  filesCreated = 10;
    const int  itemsCreated = filesCreated + 1;

    TempFiles tempFiles;
    tempFiles.addSubDirLevel(orig);
    tempFiles.create(filesCreated);
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(m_dirModel_01->rowCount(),  itemsCreated);

    QString target("modelCopyManyItemstoAnotherModel_target");
    m_deepDir_02 = new DeepDir(target, 0);
    m_dirModel_02 = new DirModel();
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);

    QStringList allFiles(m_deepDir_01->firstLevel());
    allFiles.append(tempFiles.createdList());

    m_dirModel_01->copy(allFiles);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated);
    QCOMPARE(m_dirModel_01->rowCount(),  itemsCreated);
}

void TestFolderModel::modelCutManyItemsPasteIntoAnotherModel()
{
    QString orig("modelCutManyItemsPasteIntoAnotherModel_orig");

    m_deepDir_01 = new DeepDir(orig, 5);
    m_dirModel_01 = new DirModel();
    const int  filesCreated = 10;
    const int  itemsCreated = filesCreated + 1;

    TempFiles tempFiles;
    tempFiles.addSubDirLevel(orig);
    tempFiles.create(filesCreated);
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(m_dirModel_01->rowCount(),  itemsCreated);

    QString target("modelCutManyItemsPasteIntoAnotherModel_target");
    m_deepDir_02 = new DeepDir(target, 0);
    m_dirModel_02 = new DirModel();
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);

    QStringList allFiles(m_deepDir_01->firstLevel());
    allFiles.append(tempFiles.createdList());

    m_dirModel_01->cut(allFiles);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated); //pasted into
    QCOMPARE(m_dirModel_01->rowCount(),  0);  //cut from
}

void  TestFolderModel::fsActionMoveItemsForcingCopyAndThenRemove()
{
     QString orig("fsActionMoveItemsForcingCopyAndThenRemove_orig");

     m_deepDir_01 = new DeepDir(orig, 2);
     m_dirModel_01 = new DirModel();
     const int  filesCreated = 4;
     const int  itemsCreated = filesCreated + 1;

     TempFiles tempFiles;
     tempFiles.addSubDirLevel(orig);
     tempFiles.create(filesCreated);
     m_dirModel_01->setPath(m_deepDir_01->path());
     QTest::qWait(TIME_TO_REFRESH_DIR);
     QCOMPARE(m_dirModel_01->rowCount(),  itemsCreated);

     QString target("fsActionMoveItemsForcingCopyAndThenRemove_target");
     m_deepDir_02 = new DeepDir(target, 0);
     m_dirModel_02 = new DirModel();
     m_dirModel_02->setPath(m_deepDir_02->path());
     QTest::qWait(TIME_TO_REFRESH_DIR);

     QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
     QCOMPARE(m_dirModel_02->rowCount(),  0);

     QStringList allFiles(m_deepDir_01->firstLevel());
     allFiles.append(tempFiles.createdList());

     m_dirModel_02->m_fsAction->createAndProcessAction(FileSystemAction::ActionHardMoveCopy,
                                                       allFiles);

     QTest::qWait(TIME_TO_PROCESS);

     QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated); //pasted into
     QCOMPARE(m_dirModel_01->rowCount(),  0);  //cut from
}

void TestFolderModel::modelCancelRemoveAction()
{
     const int level = 30;
     m_deepDir_01 = new DeepDir("modelCancelRemoveAction", level);
     QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

     m_dirModel_01 = new DirModel();
     m_dirModel_01->setPath(m_deepDir_01->path());
     QTest::qWait(TIME_TO_REFRESH_DIR);

     QCOMPARE(m_dirModel_01->rowCount(), 1);
     connect(m_dirModel_01, SIGNAL(progress(int,int,int)),
             this,          SLOT(progress(int,int,int)));
     connect(m_dirModel_01, SIGNAL(progress(int,int,int)),
             this,          SLOT(cancel(int,int,int)));

     m_dirModel_01->remove(0);
     QTest::qWait(TIME_TO_PROCESS);
     QTest::qWait(5);

     QCOMPARE(m_filesRemoved.count() , 0);
     QCOMPARE(m_dirModel_01->rowCount(), 1);
     QCOMPARE(m_progressCounter, 1);
     QVERIFY(m_progressTotalItems < level);  // much more than level files were created
}

void TestFolderModel::modelTestFileSize()
{
     m_dirModel_01 = new DirModel();

     QCOMPARE(m_dirModel_01->fileSize(0),      QString("0 Bytes"));
     QCOMPARE(m_dirModel_01->fileSize(1023),   QString("1023 Bytes"));
     QCOMPARE(m_dirModel_01->fileSize(1024),   QString("1.0 KB"));
     QCOMPARE(m_dirModel_01->fileSize(1000*1000),
              QString("1.0 MB"));
     QCOMPARE(m_dirModel_01->fileSize(1000*1000*1000),
              QString("1.0 GB"));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestFolderModel tc;
    int ret = QTest::qExec(&tc, argc, argv);

    return ret;
}


#include "tst_folderlistmodel.moc"

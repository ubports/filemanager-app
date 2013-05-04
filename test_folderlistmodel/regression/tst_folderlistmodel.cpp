#include "filesystemaction.h"
#include "dirmodel.h"
#include "tempfiles.h"
#include <stdio.h>

#include <QApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QFileInfo>
#include <QMetaType>
#include <QDirIterator>

#define TIME_TO_PROCESS       2300
#define TIME_TO_REFRESH_DIR   80

class TestDirModel : public QObject
{
   Q_OBJECT

public:
       TestDirModel();
      ~TestDirModel();

protected slots:
    void slotFileAdded(const QString& s)     {m_filesAdded.append(s); }
    void slotFileRemoved(const QString& s)   {m_filesRemoved.append(s); }
    void slotFileAdded(const QFileInfo& f)   {m_filesAdded.append(f.absoluteFilePath()); }
    void slotFileRemoved(const QFileInfo& f) {m_filesRemoved.append(f.absoluteFilePath()); }
    void progress(int, int, int);
    void cancel(int index, int, int percent);

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
    void  modelRemoveDirWithHiddenFilesAndLinks();
    void  modelCancelCopyAction();
    void  modelCopyFileAndDirectoryLinks();
    void  modelCopyAndPaste3Times();
    void  modelCutAndPaste3Times();


private:
    void initDeepDirs();
    void cleanDeepDirs();
    void initModels();
    void cleanModels();
    bool compareDirectories(const QString& d1,
                            const QString& d2);

    bool createLink(const QString& fullSouce,
                    const QString& link,
                    bool  fullLink = false);

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

TestDirModel::TestDirModel() : m_deepDir_01(0)
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

void TestDirModel::progress(int cur, int total, int percent)
{
    m_progressCounter++; 
    m_progressCurrentItem = cur;
    m_progressTotalItems  = total;
    m_progressPercentDone = percent;
 //  qDebug() << "progress()" << cur << total << percent;
}


bool TestDirModel::createLink(const QString &fullSouce, const QString &link, bool fullLink)
{
    bool ret = false;

    QFileInfo source(fullSouce);
    if (source.exists())
    {
        if (fullLink)
        {
            QFileInfo lnk(link);
            ret = QFile::link(source.absoluteFilePath(), lnk.absoluteFilePath());
        }
        else
        {
            QString curDir = QDir::currentPath();
            if (QDir::setCurrent(source.absolutePath()))
            {
                QFileInfo lnk(link);             
                if (source.absolutePath() != lnk.absolutePath())
                {
                    if (lnk.isAbsolute())
                    {
                        ret = QFile::link(source.absoluteFilePath(), link);
                    }
                    else
                    {
                        QDir relative(lnk.absolutePath());
                        if (relative.exists() || relative.mkpath(lnk.absolutePath()))
                        {
                           int diff=0;
                           QStringList sourceDirs = source.absolutePath().
                                                    split(QDir::separator(), QString::SkipEmptyParts);
                           QStringList targetDirs = lnk.absolutePath().
                                                    split(QDir::separator(), QString::SkipEmptyParts);
                           while (diff < sourceDirs.count())
                           {
                               if (sourceDirs.at(diff) !=
                                       targetDirs.at(diff))
                               {
                                   break;
                               }
                               diff++;
                           }
                           QString relativePath = sourceDirs.at(diff);
                           QString gap(QLatin1String("..") + QDir::separator());
                           while (diff++ < targetDirs.count())
                           {
                               relativePath.prepend(gap);
                           }
                           ret = QFile::link(relativePath
                                                 + QDir::separator()
                                                 + source.fileName(),
                                             link);
                        }
                    }
                }
                else
                {
                    ret = QFile::link(source.fileName(), link);
                }
                QDir::setCurrent(curDir);
            }
        }
    }  
    return ret;
}

bool TestDirModel::compareDirectories(const QString &d1, const QString &d2)
{
    QDirIterator d1Info(d1,
                    QDir::Files | QDir::Hidden | QDir::System,
                    QDirIterator::Subdirectories);

    QDirIterator d2Info(d2,
                    QDir::Files | QDir::Hidden | QDir::System,
                    QDirIterator::Subdirectories);

    while (d1Info.hasNext() &&  !d1Info.next().isEmpty())
    {
        if (!d2Info.hasNext())
        {
            return false;
        }
        d2Info.next();
        if (d1Info.fileName() != d2Info.fileName())
        {
            return false;
        }
        if (d1Info.fileInfo().size() != d2Info.fileInfo().size())
        {
            return false;
        }
        if (d1Info.fileInfo().permissions() != d2Info.fileInfo().permissions())
        {
            return false;
        }
    }
    if (d2Info.hasNext())
    {
        return false;
    }
    return true;
}

void TestDirModel::cancel(int index, int, int percent)
{
    DirModel * model = static_cast<DirModel*> (sender());
    if (index > 1 ||  percent > 1)
    {
         model->cancelAction();
    }
}

TestDirModel::~TestDirModel()
{

}

void TestDirModel::initDeepDirs()
{
    cleanDeepDirs();
}


void TestDirModel::cleanDeepDirs()
{
    if (m_deepDir_01) delete m_deepDir_01;
    if (m_deepDir_02) delete m_deepDir_02;
    if (m_deepDir_03) delete m_deepDir_03;
    m_deepDir_01 = 0;
    m_deepDir_02 = 0;
    m_deepDir_03 = 0;
}


void TestDirModel::initModels()
{
    cleanModels();
}


void TestDirModel::cleanModels()
{
    if (m_dirModel_01) delete m_dirModel_01;
    if (m_dirModel_02) delete m_dirModel_02;
    m_dirModel_01 = 0;
    m_dirModel_02 = 0;
}

void TestDirModel::initTestCase()
{
   qRegisterMetaType<QVector<QFileInfo> >();
}


void TestDirModel::cleanupTestCase()
{
    cleanDeepDirs();
    cleanModels();
}


void TestDirModel::init()
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



void TestDirModel::cleanup()
{   
    cleanDeepDirs();
    cleanModels();    
    m_filesAdded.clear();
    m_filesRemoved.clear();
    m_progressCounter = 0;
    m_progressTotalItems = 0;
    m_progressCurrentItem = 0;
    m_progressPercentDone = 0;
}


void TestDirModel::fsActionRemoveSingleFile()
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



void TestDirModel::fsActionRemoveSingleDir()
{
    const int level = 10;
    m_deepDir_01 = new DeepDir("removeSingleDir", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    QStringList myDeepDir(m_deepDir_01->path());
    fsAction.remove(myDeepDir);
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  false);
    QVERIFY(m_progressCounter > 2);
}


void TestDirModel::fsActionRemoveOneFileOneDir()
{
    const int level = 10;
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



void TestDirModel::fsActionRemoveTwoFilesTwoDirs()
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

void TestDirModel::modelRemoveRecursiveDirByIndex()
{
    const int level = 5;
    m_deepDir_01 = new DeepDir("modelRemoveRecursiveDirByIndex", level);
    QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE(m_dirModel_01->rowCount(), 1);
    m_dirModel_01->removeIndex(0);
    QTest::qWait(500);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE(m_dirModel_01->rowCount(), 0);
}

void TestDirModel::modelRemoveMultiItemsByFullPathname()
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


void TestDirModel::modelRemoveMultiItemsByName()
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
     m_dirModel_01->removePaths(items);
     QTest::qWait(500);

     QCOMPARE(m_filesRemoved.count() , filesToCreate);
     QCOMPARE(m_dirModel_01->rowCount(),    0);
     QCOMPARE(files.howManyExist(), 0);

     QDir().rmdir(m_dirModel_01->path());
}


void TestDirModel::modelCopyDirPasteIntoAnotherModel()
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
    connect(m_dirModel_02, SIGNAL(progress(int,int,int)),
            this,          SLOT(progress(int,int,int)));
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);

    m_dirModel_01->copyIndex(0);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  1);
    QCOMPARE(m_progressPercentDone, 100);

    QCOMPARE(compareDirectories(m_deepDir_01->path(), m_deepDir_02->path()), true);
}


void TestDirModel::modelCopyManyItemsPasteIntoAnotherModel()
{
    QString orig("modelCopyManyItemstoAnotherModel_orig");

    m_deepDir_01  = new DeepDir(orig, 5);
    m_dirModel_01 = new DirModel();
    const int  filesCreated = 10;
    int  itemsCreated = filesCreated + 1;

    //create a big file to test copy loop
    QByteArray buf(4096, 't');
    QFile big(m_deepDir_01->path() + QDir::separator() + "big.txt");
    QCOMPARE(big.open(QFile::WriteOnly),  true);
    for(int i=0; i < 106; i++)
    {
        int wrote = (int) big.write(buf);
        QCOMPARE(wrote, buf.size());
        buf += "sdfsdsedccw121222";
    }
    big.close();
    itemsCreated++;

    // create more temporary files
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
    connect(m_dirModel_02, SIGNAL(progress(int,int,int)),
            this,          SLOT(progress(int,int,int)));
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);

    QStringList allFiles(m_deepDir_01->firstLevel());
    allFiles.append(tempFiles.createdList());
    allFiles.append(big.fileName());

    m_dirModel_01->copyPaths(allFiles);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated);
    QCOMPARE(m_dirModel_01->rowCount(),  itemsCreated);
    QCOMPARE(m_progressPercentDone, 100);
    QCOMPARE(compareDirectories(m_deepDir_01->path(), m_deepDir_02->path()), true);
}

void TestDirModel::modelCutManyItemsPasteIntoAnotherModel()
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

    m_dirModel_01->cutPaths(allFiles);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated); //pasted into
    QCOMPARE(m_dirModel_01->rowCount(),  0);  //cut from
}

void  TestDirModel::fsActionMoveItemsForcingCopyAndThenRemove()
{
     QString orig("fsActionMoveItemsForcingCopyAndThenRemove_orig");

     m_deepDir_01 = new DeepDir(orig, 1);
     m_dirModel_01 = new DirModel();
     const int  filesCreated = 4;
     const int  itemsCreated = filesCreated +1;

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

     connect(m_dirModel_02->m_fsAction, SIGNAL(added(QString)),
             this, SLOT(slotFileAdded(QString)));
     connect(m_dirModel_02, SIGNAL(progress(int,int,int)),
             this,          SLOT(progress(int,int,int)));

     QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
     QCOMPARE(m_dirModel_02->rowCount(),  0);

     QStringList allFiles(m_deepDir_01->firstLevel());
     allFiles.append(tempFiles.createdList());

     m_dirModel_02->m_fsAction->createAndProcessAction(FileSystemAction::ActionHardMoveCopy,
                                                       allFiles);

     QTest::qWait(TIME_TO_PROCESS);

     QCOMPARE(m_dirModel_02->rowCount(),  itemsCreated); //pasted into
     QCOMPARE(m_dirModel_01->rowCount(),  0);  //cut from

     int totalCopied = filesCreated + m_deepDir_01->itemsCreated();
     QCOMPARE(itemsCreated, m_filesAdded.count());
     QCOMPARE(totalCopied, m_progressTotalItems);
}

void TestDirModel::modelCancelRemoveAction()
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

     m_dirModel_01->removeIndex(0);
     QTest::qWait(TIME_TO_PROCESS);
     QTest::qWait(5);

     QCOMPARE(m_filesRemoved.count() , 0);
     QCOMPARE(m_dirModel_01->rowCount(), 1);
     QVERIFY(m_progressCurrentItem > 0);     // some file were performed
     QVERIFY(m_progressPercentDone < 100);   //
}

void TestDirModel::modelTestFileSize()
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



void TestDirModel::modelRemoveDirWithHiddenFilesAndLinks()
{
     const int level = 3;
     m_deepDir_01 = new DeepDir("modelRemoveDirWithHiddenFilesAndLinks", level);
     QCOMPARE( QFileInfo(m_deepDir_01->path()).exists(),  true);

     m_dirModel_01 = new DirModel();
     m_dirModel_01->setShowHiddenFiles(true);
     m_dirModel_01->setPath(m_deepDir_01->path());
     QTest::qWait(TIME_TO_REFRESH_DIR);

     QCOMPARE(m_dirModel_01->rowCount(), 1);

     connect(m_dirModel_01, SIGNAL(progress(int,int,int)),
             this,          SLOT(progress(int,int,int)));

     QModelIndex filepathIdx = m_dirModel_01->index(0, DirModel::FilePathRole - DirModel::FileNameRole);
     QString firstItemFullPath(m_dirModel_01->data(filepathIdx).toString());

     // hidden files and links are created under first item
     QFile hiddenFile(firstItemFullPath
                      + QDir::separator()
                      + QLatin1String(".hidden.txt"));

     QCOMPARE(hiddenFile.open(QFile::WriteOnly)  ,true);
     hiddenFile.close();
     QString link_to_hiddenFile(firstItemFullPath
                                + QDir::separator()
                                + QLatin1String("link_to_hiddenFile"));
     QCOMPARE(hiddenFile.link(link_to_hiddenFile), true);

     QString hiddenFolder(firstItemFullPath
                          + QDir::separator()
                          + QLatin1String(".hiddenFolder"));

     QCOMPARE(QDir().mkdir(hiddenFolder)         , true);
     QString link_to_hidden_folder(firstItemFullPath
                                   + QDir::separator()
                                   + QLatin1String("link_to_hidden_folder"));
     QCOMPARE(QFile(hiddenFolder).link(link_to_hidden_folder),  true);

     m_dirModel_01->removeIndex(0);
     QTest::qWait(TIME_TO_PROCESS);

     QCOMPARE(m_dirModel_01->rowCount(), 0);
     QCOMPARE(m_progressPercentDone, 100);
}


void TestDirModel::modelCancelCopyAction()
{
    QString orig("modelCancelCopyAction_orig");
    m_deepDir_01  = new DeepDir(orig, 0);

    //create a big file to test copy loop
    QByteArray buf(4096, 't');
    QFile big(m_deepDir_01->path() + QDir::separator() + "big.txt");
    QCOMPARE(big.open(QFile::WriteOnly),  true);
    for(int i=0; i < 186; i++)
    {
        int wrote = (int) big.write(buf);
        QCOMPARE(wrote, buf.size());
        buf += "sdfsdsedccw121222";
    }
    big.close();

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(m_dirModel_01->rowCount(), 1);

    QString target("modelCancelCopyAction_target");
    m_deepDir_02 = new DeepDir(target, 0);
    m_dirModel_02 = new DirModel();
    connect(m_dirModel_02, SIGNAL(progress(int,int,int)),
            this,          SLOT(progress(int,int,int)));
    connect(m_dirModel_02, SIGNAL(progress(int,int,int)),
            this,          SLOT(cancel(int,int,int)));
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    m_dirModel_01->copyIndex(0);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE( QFileInfo(m_deepDir_02->path()).exists(),  true);
    QCOMPARE(m_dirModel_02->rowCount(),  0);
}

void TestDirModel::modelCopyFileAndDirectoryLinks()
{
    QString orig("modelCopyFileAndDirectoryLinks_orig");
    m_deepDir_01  = new DeepDir(orig, 1);

    //create a link in the same directory to the directory created at first level
    QString firstDir         = m_deepDir_01->lastLevel();
    QString link_to_firstDir = m_deepDir_01->path() + QDir::separator()
                               + QLatin1String("link_to_firstDir");
    QCOMPARE(createLink(firstDir, link_to_firstDir),  true);

    QDir d(firstDir);
    QFileInfoList files = d.entryInfoList(QDir::Files);
    QVERIFY(files.count() > 0);
    QCOMPARE(createLink(files.at(0).absoluteFilePath(), QLatin1String("link_to_file")),  true);

    m_dirModel_01 = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(m_dirModel_01->rowCount(), 2);


    QString target("modelCopyFileAndDirectoryLinks_target");
    m_deepDir_02 = new DeepDir(target, 0);
    m_dirModel_02 = new DirModel();
    m_dirModel_02->setPath(m_deepDir_02->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);

    QStringList  items(firstDir);
    items.append(link_to_firstDir);

    m_dirModel_01->copyPaths(items);
    QTest::qWait(10);
    m_dirModel_02->paste();
    QTest::qWait(TIME_TO_PROCESS);

    QCOMPARE(m_dirModel_01->rowCount(), 2);
    QCOMPARE(m_dirModel_02->rowCount(), 2);
    QCOMPARE(compareDirectories(m_deepDir_01->path(), m_deepDir_02->path()), true);
}


void TestDirModel::modelCopyAndPaste3Times()
{
    QString orig("modelCopyAndPaste3Times_orig");
    m_deepDir_01  = new DeepDir(orig, 5);

    TempFiles moreFiles;
    QCOMPARE(moreFiles.addSubDirLevel(orig),   true);
    QCOMPARE(moreFiles.create(10),             true);

    QStringList items(m_deepDir_01->firstLevel());
    items.append(moreFiles.createdList());

    QString target1("modelCopyAndPaste3Times_target1");
    QString target2("modelCopyAndPaste3Times_target2");
    QString target3("modelCopyAndPaste3Times_target3");

    DeepDir t1(target1,0);
    DeepDir t2(target2,0);
    DeepDir t3(target3,0);

    m_dirModel_01  = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(items.count(), m_dirModel_01->rowCount());
    m_dirModel_01->copyPaths(items);

    DirModel  model1;
    model1.setPath(t1.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model1.rowCount() , 0);
    model1.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model1.rowCount(),  items.count());
    QCOMPARE(compareDirectories(m_deepDir_01->path(), t1.path()),   true);

    DirModel  model2;
    model2.setPath(t2.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model2.rowCount() , 0);
    model2.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model2.rowCount(),  items.count());
    QCOMPARE(compareDirectories(m_deepDir_01->path(), t2.path()),   true);

    DirModel  model3;
    model3.setPath(t3.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model3.rowCount() , 0);
    model3.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model3.rowCount(),  items.count());
    QCOMPARE(compareDirectories(m_deepDir_01->path(), t3.path()),   true);
}


void TestDirModel::modelCutAndPaste3Times()
{
    QString orig("modelCutAndPaste3Times_orig");
    m_deepDir_01  = new DeepDir(orig, 5);

    TempFiles moreFiles;
    QCOMPARE(moreFiles.addSubDirLevel(orig),   true);
    QCOMPARE(moreFiles.create(10),             true);

    QStringList items(m_deepDir_01->firstLevel());
    items.append(moreFiles.createdList());

    QString target1("modelCutAndPaste3Times_target1");
    QString target2("modelCutAndPaste3Times_target2");
    QString target3("modelCutAndPaste3Times_target3");

    DeepDir t1(target1,0);
    DeepDir t2(target2,0);
    DeepDir t3(target3,0);

    m_dirModel_01  = new DirModel();
    m_dirModel_01->setPath(m_deepDir_01->path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(items.count(), m_dirModel_01->rowCount());
    m_dirModel_01->cutPaths(items);

    DirModel  model1;
    model1.setPath(t1.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model1.rowCount() , 0);
    model1.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model1.rowCount(),  items.count());
    QCOMPARE(m_dirModel_01->rowCount()  , 0 );

    DirModel  model2;
    model2.setPath(t2.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model2.rowCount() , 0);
    model2.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model2.rowCount(),  items.count());

    DirModel  model3;
    model3.setPath(t3.path());
    QTest::qWait(TIME_TO_REFRESH_DIR);
    QCOMPARE(model3.rowCount() , 0);
    model3.paste();
    QTest::qWait(TIME_TO_PROCESS);
    QCOMPARE(model3.rowCount(),  items.count());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestDirModel tc;
    int ret = QTest::qExec(&tc, argc, argv);

    return ret;
}


#include "tst_folderlistmodel.moc"

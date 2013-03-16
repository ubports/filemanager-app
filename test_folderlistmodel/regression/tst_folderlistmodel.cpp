#include "filesystemaction.h"

#include <QApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QFileInfo>

#include <stdio.h>



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

DeepDir::DeepDir(const QString &rootDir, int level) :
    root(QDir::tempPath() + QDir::separator() + rootDir)
{
    QByteArray content(1024, 'a');
    if (!rootDir.isEmpty())
    {
        remove(); // clear
        QDir().mkpath(root);
        QString subdir(root);
        QString levelStr;
        for(int counter=1 ; counter <= level; counter++)
        {
            levelStr.sprintf("level_%02d", counter);
            subdir += QDir::separator() + levelStr;
            QDir().mkpath(subdir);
            levelStr.sprintf("file_%02d", counter);
            QFile f(subdir +  QDir::separator() + levelStr);
            f.open(QFile::WriteOnly);
            f.write(content);
            content += "1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq";
            f.close();
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
    void slotFileAdded(const QString& s)    {m_filesAdded.append(s); }
    void slotFileRemoved(const QString& s)  {m_filesRemoved.append(s); }
    void progress(int, int, int);

private Q_SLOTS:
    void initTestCase();       //before all tests
    void cleanupTestCase();    //after all tests
    void init();               //before every test
    void cleanup();            //after every test

    void  removeSingleFile();
    void  removeSingleDir();
    void  removeOneFileOneDir();
    void  removeTwoFilesTwoDir();


private:
    FileSystemAction  fsAction;

    QStringList m_filesAdded;
    QStringList m_filesRemoved;

    DeepDir   *    m_deepDir;

    int            m_progressCounter;
    int            m_progressTotalItems;
    int            m_progressCurrentItem;
    int            m_progressPercentDone;
    
};

TestFolderModel::TestFolderModel() : m_deepDir(0)
{
    connect(&fsAction, SIGNAL(added(QString)),
            this,      SLOT(slotFileAdded(QString)) );
    connect(&fsAction, SIGNAL(removed(QString)),
            this,      SLOT(slotFileRemoved(QString)) );
    connect(&fsAction, SIGNAL(progress(int,int,int)),
            this,      SLOT(progress(int,int,int)));
}

void TestFolderModel::progress(int cur, int total, int percent)
{
    m_progressCounter++;
    qDebug() << "TestFolderModel::progress"  << total << cur << percent;
}


TestFolderModel::~TestFolderModel()
{

}


void TestFolderModel::initTestCase()
{
   
}


void TestFolderModel::cleanupTestCase()
{
    if (m_deepDir)
    {
        delete m_deepDir;
        m_deepDir = 0;
    }
}


void TestFolderModel::init()
{  
   m_filesAdded.clear();
   m_filesRemoved.clear();
   if (m_deepDir)
   {
       delete m_deepDir;
       m_deepDir = 0;
   }
   m_progressCounter = 0;
   m_progressTotalItems = 0;
   m_progressCurrentItem = 0;
   m_progressPercentDone = 0;
}



void TestFolderModel::cleanup()
{   
    if (m_deepDir)
    {
        delete m_deepDir;
        m_deepDir = 0;
    }
}


void TestFolderModel::removeSingleFile()
{
    QFile tmp(QDir::tempPath() + QDir::separator() + "fsAtion_removeSingleFile");
    QCOMPARE(tmp.open(QFile::WriteOnly), true);
    tmp.write(QByteArray("1234567890\n"));
    tmp.close();
    QFileInfo i(tmp);
    QCOMPARE(i.exists(), true);
    QVERIFY(i.size() > 0);
    QStringList myFile(i.absoluteFilePath());
    fsAction.remove(myFile);
    QTest::qWait(700);
    QCOMPARE(m_filesRemoved.count() , 1);

    QCOMPARE(m_filesRemoved.at(0), i.absoluteFilePath());
    QFileInfo now(i.absoluteFilePath());
    QCOMPARE(now.exists(),  false);
}



void TestFolderModel::removeSingleDir()
{
    const int level = 50;
    m_deepDir = new DeepDir("removeSingleDir", level);
    QCOMPARE( QFileInfo(m_deepDir->path()).exists(),  true);

    QStringList myDeepDir(m_deepDir->path());
    fsAction.remove(myDeepDir);
    QTest::qWait(2000);

    QCOMPARE(m_filesRemoved.count() , 1);
    QCOMPARE( QFileInfo(m_deepDir->path()).exists(),  false); // does not exist anymore
    QVERIFY(m_progressCounter > 2);
}


void TestFolderModel::removeOneFileOneDir()
{
    QCOMPARE(false, true);
}



void TestFolderModel::removeTwoFilesTwoDir()
{
    QCOMPARE(false, true);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestFolderModel tc;
    int ret = QTest::qExec(&tc, argc, argv);

    return ret;
}


#include "tst_folderlistmodel.moc"

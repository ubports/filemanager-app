/**************************************************************************
 *
 * Copyright 2013 Canonical Ltd.
 * Copyright 2013 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: simplelist.cpp
 * Date: 3/9/2013
 */

#include "simplelist.h"
#include "ui_simplelist.h"
#include "dirmodel.h"

#include <QDir>
#include <QMetaType>
#include <QHeaderView>
#include <QDebug>
#include <QProgressBar>
#include <QMessageBox>
#include <QTimer>

SimpleList::SimpleList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleList),
    m_curRow(-1),
    m_pbar( new QProgressBar() )
{
    ui->setupUi(this);

    m_model = new DirModel(this);

    qRegisterMetaType< QVector<QFileInfo> > ("QVector<QFileInfo>");
    qRegisterMetaType<QFileInfo>("QFileInfo");

    ui->tableView->setModel(m_model);   

    connect(ui->tableView, SIGNAL(clicked(QModelIndex)),
            this,          SLOT(onRowClicked(QModelIndex)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)),
            this,          SLOT(onOpenItem(QModelIndex)));

    connect(ui->tableView->verticalHeader(), SIGNAL(sectionClicked(int)),
            this,                            SLOT(onVerticalHeaderClicked(int)));

    connect(m_model, SIGNAL(pathChanged(QString)),
            this,    SLOT(pathChanged(QString)));

    connect(ui->pushButtonCdUp,   SIGNAL(clicked()),  this, SLOT(onCdUP()));
    connect(ui->pushButtonCopy,   SIGNAL(clicked()),  this, SLOT(onCopy()));
    connect(ui->pushButtonCut,    SIGNAL(clicked()),  this, SLOT(onCut()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()),  this, SLOT(onRemove()));
    connect(ui->pushButtonGoHome, SIGNAL(clicked()),  this, SLOT(onGoHome()));
    connect(ui->pushButtonIntoDirs,SIGNAL(clicked()), this, SLOT(onCdInto()));
    connect(ui->pushButtonNewDir, SIGNAL(clicked()),  this, SLOT(onNewDir()));
    connect(ui->pushButtonPaste,  SIGNAL(clicked()),  this, SLOT(onPaste()));
    connect(ui->pushButtonRename, SIGNAL(clicked()),  this, SLOT(onRename()));

    connect(ui->checkBoxShowDirs,    SIGNAL(clicked(bool)), this, SLOT(onShowDirs(bool)));
    connect(ui->checkBoxShowHidden,  SIGNAL(clicked(bool)), this, SLOT(onShowHidden(bool)));
    connect(ui->checkBoxExtFsWatcher, SIGNAL(toggled(bool)),this, SLOT(onExtFsWatcherEnabled(bool)));

    connect(ui->pushButtonOpen,   SIGNAL(clicked()),
            this,                 SLOT(onOpen()));

    connect(ui->lineEditOpen,   SIGNAL(returnPressed()),
            this,                 SLOT(onOpen()));

    ui->checkBoxShowDirs->setChecked( m_model->showDirectories() );

    resize(800,600);

    connect(m_model, SIGNAL(insertedRow(int)),
            this,    SLOT(resizeColumnForName(int)));

    connect(ui->tableView->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),
            this,                              SLOT(setSort(int,Qt::SortOrder)));

    connect(m_model, SIGNAL(progress(int,int,int)),
            this,    SLOT(progress(int,int,int)));

    connect(m_model, SIGNAL(clipboardChanged()),
            this,    SLOT(clipboardChanged()));

    connect(m_model, SIGNAL(error(QString,QString)),
            this,    SLOT(error(QString,QString)));

    ui->tableView->horizontalHeader()->setSortIndicator(0,Qt::AscendingOrder);

    m_pbar->setMaximum(100);
    m_pbar->setMinimum(0);

    ui->checkBoxExtFsWatcher->click();
    m_model->goHome();

    clipboardChanged();
}

SimpleList::~SimpleList()
{
    delete ui;
}

void SimpleList::onRowClicked(QModelIndex index)
{
    if (index.isValid())
    {
        m_curRow = index.row();
    }
    else
    {
        m_curRow = -1;
    }
}


void SimpleList::onCdInto()
{
    m_model->cdIntoIndex(m_curRow);
}

void SimpleList::onGoHome()
{
     m_model->goHome();
}

void SimpleList::onCdUP()
{
    m_model->cdUp();
}

void SimpleList::onRemove()
{
     m_model->removeIndex(m_curRow);
}

void SimpleList::onCopy()
{
    m_model->copyIndex(m_curRow);
}

void SimpleList::onCut()
{
    m_model->cutIndex(m_curRow);
}


void SimpleList::onPaste()
{
    m_model->paste();
}

void SimpleList::onNewDir()
{
   m_model->mkdir(ui->lineEditNewDir->text());
}

void SimpleList::onRename()
{
   m_model->rename(m_curRow, ui->lineEditRename->text());
}

void SimpleList::onShowDirs(bool show)
{
    m_model->setShowDirectories(show);
}

void SimpleList::onShowHidden(bool s)
{
    m_model->setShowHiddenFiles(s);
}

void SimpleList::onVerticalHeaderClicked(int row)
{
    m_curRow = row;
}


void SimpleList::setSort(int col, Qt::SortOrder order)
{
    if (col == 0 || col == 2)
    {
        if (col == 0)
        {
            m_model->setSortBy(DirModel::SortByName);
        }
        else
        {
            m_model->setSortBy(DirModel::SortByDate);
        }
        DirModel::SortOrder o = (DirModel::SortOrder)order;
        m_model->setSortOrder(o);
    }
}

void SimpleList::clipboardChanged()
{   
    ui->clipboardNumber->setText( QString::number(m_model->getClipboardUrlsCounter()));
}

void SimpleList::progress(int cur, int total, int percent)
{
    QString p;
    m_pbar->setValue(percent);
    if (cur == 0 && percent == 0)
    {
        m_pbar->reset();
        m_pbar->show();
    }
    else
        if (percent == 100)
        {
            QTimer::singleShot(200, m_pbar, SLOT(hide()));
        }
    p.sprintf("progress(cur=%d, total=%d, percent=%d)", cur,total,percent);
    qDebug() << p;
}


void SimpleList::error(QString title, QString message)
{
    if (m_pbar)
    {
        m_pbar->hide();
    }
    QMessageBox::critical(this, title, message);
}

void SimpleList::onOpenItem(QModelIndex index)
{
    if (index.isValid())
    {
        m_curRow = index.row();
        if (!m_model->openIndex(m_curRow))
        {
            QModelIndex idx = m_model->index(m_curRow, 0);
            QString item = m_model->data(idx).toString();
            error("Could not open item index", item);
        }
    }
    else
    {
        m_curRow = -1;
    }
}


void SimpleList::pathChanged(QString path)
{
    this->setWindowTitle(path);
}

void SimpleList::resizeColumnForName(int)
{
    ui->tableView->resizeColumnToContents(0);
}


void SimpleList::onOpen()
{
    if ( ! m_model->openPath(ui->lineEditOpen->text()) )
    {
          QMessageBox::critical(this, "DirModel::openIndex() failed to open" , ui->lineEditOpen->text());
    }
}


void SimpleList::onExtFsWatcherEnabled(bool enable)
{
    m_model->setEnabledExternalFSWatcher(enable);
}

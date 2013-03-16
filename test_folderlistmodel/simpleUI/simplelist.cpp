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

SimpleList::SimpleList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleList),
    m_curRow(-1)
{
    ui->setupUi(this);

    m_model = new DirModel(this);

    qRegisterMetaType< QVector<QFileInfo> > ();

    ui->tableView->setModel(m_model);
    m_model->goHome();

    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onRowClicked(QModelIndex)));
    connect(ui->tableView->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onVerticalHeaderClicked(int)));

    connect(ui->pushButtonCdUp,   SIGNAL(clicked()),  this, SLOT(onCdUP()));
    connect(ui->pushButtonCopy,   SIGNAL(clicked()),  this, SLOT(onCopy()));
    connect(ui->pushButtonCut,    SIGNAL(clicked()),  this, SLOT(onCut()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()),  this, SLOT(onRemove()));
    connect(ui->pushButtonGoHome, SIGNAL(clicked()),  this, SLOT(onGoHome()));
    connect(ui->pushButtonIntoDirs,SIGNAL(clicked()), this, SLOT(onCdInto()));
    connect(ui->pushButtonNewDir, SIGNAL(clicked()),  this, SLOT(onNewDir()));
    connect(ui->pushButtonPaste,  SIGNAL(clicked()),  this, SLOT(onPaste()));
    connect(ui->pushButtonRename, SIGNAL(clicked()),  this, SLOT(onRename()));

    connect(ui->checkBoxShowDirs, SIGNAL(clicked(bool)), this, SLOT(onShowDirs(bool)));

    ui->checkBoxShowDirs->setChecked( m_model->showDirectories() );
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
    m_model->cdInto(m_curRow);
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
     m_model->remove(m_curRow);
}

void SimpleList::onCopy()
{

}

void SimpleList::onCut()
{
    m_model->cut(m_curRow);
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

void SimpleList::onVerticalHeaderClicked(int row)
{
    m_curRow = row;
}

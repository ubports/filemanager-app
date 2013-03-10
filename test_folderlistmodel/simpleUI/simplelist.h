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
 * File: simplelist.h
 * Date: 3/9/2013
 */

#ifndef SIMPLELIST_H
#define SIMPLELIST_H

#include <QWidget>
#include <QModelIndex>

class DirModel;

namespace Ui {
class SimpleList;
}

class SimpleList : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimpleList(QWidget *parent = 0);
    ~SimpleList();
    
private:
    Ui::SimpleList *ui;
    DirModel       *m_model;
    int            m_curRow;

private slots:
    void   onCdInto();
    void   onGoHome();
    void   onCdUP();
    void   onRemove();
    void   onCopy();
    void   onCut();
    void   onPaste();
    void   onNewDir();
    void   onRename();
    void   onShowDirs(bool);
    void   onRowClicked(QModelIndex);
    void   onVerticalHeaderClicked(int);
};

#endif // SIMPLELIST_H

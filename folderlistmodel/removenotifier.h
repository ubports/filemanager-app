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
 * File: removenotifier.h
 * Date: 3/16/2013
 */

#ifndef REMOVENOTIFIER_H
#define REMOVENOTIFIER_H

#include <QObject>
#include <QFileInfo>

class FileSystemAction;

/*!
 * \brief The RemoveNotifier is a utility class for \ref FileSystemAction to send
 *         notifications about removed files/dir
 *
 *  This class must have a unique instance to notify all instances of \ref FileSystemAction and \ref DirModel
 */
class RemoveNotifier : public QObject
{
    Q_OBJECT

    friend class FileSystemAction;

private:
    explicit RemoveNotifier(QObject *parent = 0);
    void notifyRemoved(const QString& item);
    void notifyRemoved(const QFileInfo& fi);

signals:   
    void     removed(const QString& item);
    void     removed(const QFileInfo&);
};

#endif // REMOVENOTIFIER_H

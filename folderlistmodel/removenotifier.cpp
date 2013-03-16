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
 * File: removenotifier.cpp
 * Date: 3/16/2013
 */

#include "removenotifier.h"

RemoveNotifier::RemoveNotifier(QObject *parent) :
    QObject(parent)
{
}

void RemoveNotifier::notifyRemoved(const QFileInfo &fi)
{
    emit removed(fi);
}


void RemoveNotifier::notifyRemoved(const QString &item)
{
    emit removed(item);
}

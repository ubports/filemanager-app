/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Author : Arto Jalkanen <ajalkane@gmail.com>
 */

#ifndef PAMAUTHENTICATION_H
#define PAMAUTHENTICATION_H

#include <QObject>
#include <QAbstractListModel>
#include <QStandardPaths>
#include <QSettings>

class PamAuthentication : public QObject
{
    Q_OBJECT

public:
    explicit PamAuthentication(QObject *parent = 0);
    ~PamAuthentication();

    Q_INVOKABLE inline bool requireAuthentication() {
        return true;
    }

    Q_INVOKABLE inline bool validatePasswordToken(const QString &token) const {
        return true;
    }

public slots:
private:
};

#endif // PAMAUTHENTICATION_H




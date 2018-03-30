/*
  Copyright (C) 2015 Canonical, Ltd.
  Copyright (C) 2015, 2017 Stefano Verzegnassi

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License 3 as published by
  the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef FMUTILS_H
#define FMUTILS_H

#include <QObject>
#include <QMimeType>

#include <libintl.h>
#define _(x) gettext(x)

class FMUtils : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static bool exists(const QString &path);
    Q_INVOKABLE static bool copy(const QString &source, const QString &destination);

    Q_INVOKABLE static QString getFileBaseNameFromPath(const QString &filePath);
    Q_INVOKABLE static QString getFileNameFromPath(const QString &filePath);

    Q_INVOKABLE static QString basename(const QString &path);
    Q_INVOKABLE static bool pathExists(const QString &path);

    Q_INVOKABLE static QString pathName(const QString &path);
    Q_INVOKABLE static QString pathTitle(const QString &path);

    Q_INVOKABLE static QString newFileUniqueName(const QString &dirPath, const QString &fileName);

    Q_INVOKABLE static QString capitalize(const QString &string);
};

#endif // FMUTILS_H

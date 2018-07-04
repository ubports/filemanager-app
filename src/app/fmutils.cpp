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
#include "fmutils.h"

#include <QFileInfo>
#include <QDir>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QDirIterator>
#include <QDateTime>
#include <QtGui/QGuiApplication>

#include <QDebug>

bool FMUtils::exists(const QString &path)
{
    QFileInfo fi(path);

    if (fi.isFile())
        return fi.exists();

    // else
    return QDir(path).exists();
}

bool FMUtils::copy(const QString &source, const QString &destination)
{
    qDebug() << Q_FUNC_INFO << "Copying" << source << "to" << destination;

    return QFile::copy(source, destination);
}

QString FMUtils::getFileBaseNameFromPath(const QString &filePath)
{
    return QFileInfo(filePath).completeBaseName();
}

QString FMUtils::getFileNameFromPath(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

QString FMUtils::basename(const QString &path)
{
    return QDir(path).dirName();
}

bool FMUtils::pathExists(const QString &path)
{
    QString normalizedPath = path;
    normalizedPath = normalizedPath.replace("~", QDir::homePath());

    if (normalizedPath.indexOf("/") == 0)
        return QFileInfo(normalizedPath).exists();
    else
        return false;
}

QString FMUtils::pathName(const QString &path)
{
    if (path == "/")
        return path;
    else
        return basename(path);
}

QString FMUtils::pathTitle(const QString &path)
{
    if (path == QDir::homePath())
        return QString(_("Home"));

    if (path == QLatin1Literal("/"))
        return QString(_("My Device"));

    if (path == QLatin1Literal("smb://"))
        return QString(_("Network"));

    return basename(path);
}

QString FMUtils::newFileUniqueName(const QString &dirPath, const QString &fileName)
{
    QDir dir(dirPath);
    QFileInfo fi(dir, fileName);

    // FIXME: TODO: Support files like .tar.gz - JS code from old File Manager had
    // the same issue. We can say it's okay unless we'll have a problem.

    QString suffix = fi.suffix();
    QString filenameWithoutSuffix = fi.completeBaseName();

    QMimeDatabase mdb;
    QMimeType mt = mdb.mimeTypeForFile(fi.absoluteFilePath());

    // If the filename doesn't have an extension add one from the
    // detected mimetype
    if (suffix.isEmpty())
        suffix = mt.preferredSuffix();

    QString destination = QString("%1.%2").arg(dir.absolutePath() + filenameWithoutSuffix, suffix);

    // If there's already a file of this name, reformat it to
    // "filename (copy x).png" where x is a number, incremented until we find an
    // available filename.
    if (QFile::exists(destination)) {
        /*
          TRANSLATORS: This string is used for renaming a copied file,
          when a file with the same name already exists in user's
          Documents folder.

          e.g. "Manual_Aquaris_E4.5_ubuntu_EN.pdf" will become
            "Manual_Aquaris_E4.5_ubuntu_EN (copy 2).pdf"
             where "2" is given by the argument "%1"
        */
        QString reformattedSuffix = QString(_("copy %1"));

        // Check if the file has already a "copy" suffix
        // If so, remove it since we will update it later.
        QRegExp rx(" \\(" + reformattedSuffix.arg(QString("\\d+")) + "\\)");
        int reformattedSuffixPos = filenameWithoutSuffix.lastIndexOf(rx);

        if (reformattedSuffixPos != -1)
            filenameWithoutSuffix.truncate(reformattedSuffixPos);

        // Add the right "copy" suffix.
        int append = 1;
        while (QFile::exists(destination)) {
            destination = QString("%1 (%2).%3").arg(
                        dir.absolutePath() + QDir::separator() + filenameWithoutSuffix,
                        reformattedSuffix.arg(QString::number(append)),
                        suffix);
            append++;
        }
    }

    return destination;
}

QString FMUtils::capitalize(const QString &string)
{
    QString tmp = string;
    tmp[0] = tmp[0].toUpper();

    return tmp;
}


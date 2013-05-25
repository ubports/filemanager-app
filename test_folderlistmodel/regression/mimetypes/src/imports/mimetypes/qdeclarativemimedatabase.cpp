/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMimeTypes addon of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativemimedatabase_p.h"   // Basis

#include "qdeclarativemimetype_p.h"

#include <QtCore/QDebug>

// ------------------------------------------------------------------------------------------------

extern bool qt_isQMimeDatabaseDebuggingActivated;

#ifndef QT_NO_DEBUG_OUTPUT
#define DBG() if (qt_isQMimeDatabaseDebuggingActivated) qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO
#else
#define DBG() if (0) qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO
#endif

// ------------------------------------------------------------------------------------------------

/*!
    \qmlclass MimeDatabase QDeclarativeMimeDatabase
    \brief The QML MimeDatabase element maintains a database of MIME types.

    The MIME type database is provided by the freedesktop.org shared-mime-info
    project. If the MIME type database cannot be found on the system, Qt
    will use its own copy of it.

    Applications which want to define custom MIME types need to install an
    XML file into the locations searched for MIME definitions.
    These locations can be queried with
    QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QLatin1String("mime/packages"),
    QStandardPaths::LocateDirectory);
    On a typical Unix system, this will be /usr/share/mime/packages/, but it is also
    possible to extend the list of directories by setting the environment variable
    XDG_DATA_DIRS. For instance adding /opt/myapp/share to XDG_DATA_DIRS will result
    in /opt/myapp/share/mime/packages/ being searched for MIME definitions.

    Here is an example of MIME XML:
    \code
    <?xml version="1.0" encoding="UTF-8"?>
    <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
      <mime-type type="application/vnd.nokia.qt.qmakeprofile">
        <comment xml:lang="en">Qt qmake Profile</comment>
        <glob pattern="*.pro" weight="50"/>
      </mime-type>
    </mime-info>
    \endcode

    For more details about the syntax of XML MIME definitions, including defining
    "magic" in order to detect MIME types based on data as well, read the
    Shared Mime Info specification at
    http://standards.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-latest.html

    On Unix systems, a binary cache is used for more performance. This cache is generated
    by the command "update-mime-database path", where path would be /opt/myapp/share/mime
    in the above example. Make sure to run this command when installing the MIME type
    definition file.

    The class is protected by a QMutex and can therefore be accessed by threads.

    \sa QMimeType
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \class QDeclarativeMimeDatabase
    \brief The QDeclarativeMimeDatabase class is the QML wrapper for the class QMimeDatabase which maintains a database of MIME types.
    \inherits QObject

    The MIME type database is provided by the freedesktop.org shared-mime-info
    project. If the MIME type database cannot be found on the system, Qt
    will use its own copy of it.

    Applications which want to define custom MIME types need to install an
    XML file into the locations searched for MIME definitions.
    These locations can be queried with
    QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QLatin1String("mime/packages"),
    QStandardPaths::LocateDirectory);
    On a typical Unix system, this will be /usr/share/mime/packages/, but it is also
    possible to extend the list of directories by setting the environment variable
    XDG_DATA_DIRS. For instance adding /opt/myapp/share to XDG_DATA_DIRS will result
    in /opt/myapp/share/mime/packages/ being searched for MIME definitions.

    Here is an example of MIME XML:
    \code
    <?xml version="1.0" encoding="UTF-8"?>
    <mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
      <mime-type type="application/vnd.nokia.qt.qmakeprofile">
        <comment xml:lang="en">Qt qmake Profile</comment>
        <glob pattern="*.pro" weight="50"/>
      </mime-type>
    </mime-info>
    \endcode

    For more details about the syntax of XML MIME definitions, including defining
    "magic" in order to detect MIME types based on data as well, read the
    Shared Mime Info specification at
    http://standards.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-latest.html

    On Unix systems, a binary cache is used for more performance. This cache is generated
    by the command "update-mime-database path", where path would be /opt/myapp/share/mime
    in the above example. Make sure to run this command when installing the MIME type
    definition file.

    The class is protected by a QMutex and can therefore be accessed by threads.

    \sa QMimeType
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QDeclarativeMimeDatabase::QDeclarativeMimeDatabase(QObject *theParent);
    \brief Performs default initialization of the contained MimeDatabase, and attaches the object to the specified \a theParent for destruction.
 */
QDeclarativeMimeDatabase::QDeclarativeMimeDatabase(QObject *theParent) :
        QObject(theParent),
        m_MimeDatabase()
{
    DBG() << "mimeTypeNames():" << mimeTypeNames();
}

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QDeclarativeMimeDatabase::~QDeclarativeMimeDatabase();
    \brief Destroys the contained MimeDatabase.
 */
QDeclarativeMimeDatabase::~QDeclarativeMimeDatabase()
{
    DBG() << "mimeTypeNames():" << mimeTypeNames();
}

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QMimeDatabase &QDeclarativeMimeDatabase::mimeDatabase()
    \brief Returns the contained MimeDatabase.
 */
QMimeDatabase &QDeclarativeMimeDatabase::mimeDatabase()
{
    return m_MimeDatabase;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty bool MimeDatabase::isDebuggingActivated
    Holds the indication if debugging for the class is activated.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeDatabase::isDebuggingActivated
    Holds the indication if debugging for the class is activated.
 */

// ------------------------------------------------------------------------------------------------

bool QDeclarativeMimeDatabase::isDebuggingActivated() const
{
    return qt_isQMimeDatabaseDebuggingActivated;
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeDatabase::setIsDebuggingActivated(const bool newIsDebuggingActivated)
{
    qt_isQMimeDatabaseDebuggingActivated = newIsDebuggingActivated;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty list<string> MimeDatabase::mimeTypeNames
    Holds the list of registered MIME types.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \property QDeclarativeMimeDatabase::mimeTypeNames
    Holds the list of registered MIME types.
 */

// ------------------------------------------------------------------------------------------------

QVariantList QDeclarativeMimeDatabase::mimeTypeNames() const
{
    QVariantList result;

    foreach (const QMimeType &mimeType, m_MimeDatabase.allMimeTypes()) {
        result << mimeType.name();
    }

    return result;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod MimeType MimeDatabase::mimeTypeForName(string nameOrAlias)
    \brief Returns a MIME type for \a nameOrAlias or an invalid one if none found.
 */

// ------------------------------------------------------------------------------------------------

QDeclarativeMimeType *QDeclarativeMimeDatabase::mimeTypeForName (
                                                    const QString &nameOrAlias
                                                )
{
    return new QDeclarativeMimeType (
                   m_MimeDatabase.mimeTypeForName(nameOrAlias),
                   this   // <- The new object will be released later
                          //    when this registry is released.
               );
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod MimeType MimeDatabase::mimeTypeForFileName(string fileName)
    \brief Returns a MIME type for the file \a fileName.

    A valid MIME type is always returned. If the file name doesn't match any
    known pattern, the default MIME type (application/octet-stream)
    is returned.

    This function does not try to open the file. To also use the content
    when determining the MIME type, use mimeTypeForFile().
*/
QDeclarativeMimeType *QDeclarativeMimeDatabase::mimeTypeForFileName (
                                                    const QString &fileName
                                                )
{
    return new QDeclarativeMimeType (
                   m_MimeDatabase.mimeTypeForFile(fileName, QMimeDatabase::MatchExtension),
                   this   // <- The new object will be released later
                          //    when this registry is released.
               );
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod MimeType MimeDatabase::mimeTypeForFile(string fileName)
    \brief Returns a MIME type for \a fileName.

    This method looks at both the file name and the file contents,
    if necessary. The file extension has priority over the contents,
    but the contents will be used if the file extension is unknown, or
    matches multiple MIME types.

    A valid MIME type is always returned. If the file doesn't match any
    known pattern or data, the default MIME type (application/octet-stream)
    is returned.

    The \a fileName can also include an absolute or relative path.
*/
QDeclarativeMimeType *QDeclarativeMimeDatabase::mimeTypeForFile(
                                                    const QString &fileName
                                                )
{
    return new QDeclarativeMimeType (
                   m_MimeDatabase.mimeTypeForFile(fileName),
                   this   // <- The new object will be released later
                          //    when this registry is released.
               );
}

// ------------------------------------------------------------------------------------------------

#undef DBG

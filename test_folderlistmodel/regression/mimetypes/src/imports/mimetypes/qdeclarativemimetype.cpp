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

#include "qdeclarativemimetype_p.h"   // Basis

#include <QtCore/QDebug>
#include <qmimetype_p.h>

// ------------------------------------------------------------------------------------------------

extern bool qt_isQMimeTypeDebuggingActivated;

#ifndef QT_NO_DEBUG_OUTPUT
#define DBG() if (qt_isQMimeTypeDebuggingActivated) qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO
#else
#define DBG() if (0) qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO
#endif

// ------------------------------------------------------------------------------------------------

/*!
    \qmlclass MimeType QDeclarativeMimeType
    \brief The QML MimeType element describes types of file or data, represented by a MIME type string.

    For instance a file named "readme.txt" has the MIME type "text/plain".
    The MIME type can be determined from the file name, or from the file
    contents, or from both. MIME type determination can also be done on
    buffers of data not coming from files.

    Determining the MIME type of a file can be useful to make sure your
    application supports it. It is also useful in file-manager-like applications
    or widgets, in order to display an appropriate icon() for the file, or even
    the descriptive comment() in detailed views.

    To check if a file has the expected MIME type, you should use inherits()
    rather than a simple string comparison based on the name(). This is because
    MIME types can inherit from each other: for instance a C source file is
    a specific type of plain text file, so text/x-csrc inherits text/plain.

    \sa QMimeDatabase
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \class QDeclarativeMimeType
    \brief The QDeclarativeMimeType class is the QML wrapper for the class QMimeType which describes types of file or data, represented by a MIME type string.
    \inherits QObject

    For instance a file named "readme.txt" has the MIME type "text/plain".
    The MIME type can be determined from the file name, or from the file
    contents, or from both. MIME type determination can also be done on
    buffers of data not coming from files.

    Determining the MIME type of a file can be useful to make sure your
    application supports it. It is also useful in file-manager-like applications
    or widgets, in order to display an appropriate icon() for the file, or even
    the descriptive comment() in detailed views.

    To check if a file has the expected MIME type, you should use inherits()
    rather than a simple string comparison based on the name(). This is because
    MIME types can inherit from each other: for instance a C source file is
    a specific type of plain text file, so text/x-csrc inherits text/plain.

    \sa QMimeDatabase
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QDeclarativeMimeType::QDeclarativeMimeType(QObject *theParent);
    \brief Performs default initialization of the contained MimeType, and attaches the object to the specified \a theParent for destruction.
 */
QDeclarativeMimeType::QDeclarativeMimeType(QObject *theParent) :
        QObject(theParent),
        m_MimeType()
{
    DBG() << "name():" << name();
    //DBG() << "aliases():" << aliases();
    //DBG() << "comment():" << comment();
    DBG() << "genericIconName():" << genericIconName();
    DBG() << "iconName():" << iconName();
    DBG() << "globPatterns():" << globPatterns();
    DBG() << "suffixes():" << suffixes();
    DBG() << "preferredSuffix():" << preferredSuffix();
}

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QDeclarativeMimeType::QDeclarativeMimeType(const QMimeType &other, QObject *theParent);
    \brief Copies the specified MimeType \a other, and attaches the object to the specified \a theParent for destruction.
 */
QDeclarativeMimeType::QDeclarativeMimeType(const QMimeType &other, QObject *theParent) :
        QObject(theParent),
        m_MimeType(other)
{
    DBG() << "name():" << name();
    //DBG() << "aliases():" << aliases();
    //DBG() << "comment():" << comment();
    DBG() << "genericIconName():" << genericIconName();
    DBG() << "iconName():" << iconName();
    DBG() << "globPatterns():" << globPatterns();
    DBG() << "suffixes():" << suffixes();
    DBG() << "preferredSuffix():" << preferredSuffix();
}

// ------------------------------------------------------------------------------------------------

#ifdef Q_COMPILER_RVALUE_REFS
/*!
    \internal
    \fn QDeclarativeMimeType::QDeclarativeMimeType(QMimeType &&other, QObject *theParent);
    \brief Constructs this QDeclarativeMimeType object by moving the data of the QMimeType rvalue reference \a other, and attaches the object to the specified \a theParent for destruction.
 */
QDeclarativeMimeType::QDeclarativeMimeType(QMimeType &&other, QObject *theParent) :
        QObject(theParent),
        m_MimeType(other)
{
    DBG() << "name():" << name();
    //DBG() << "aliases():" << aliases();
    //DBG() << "comment():" << comment();
    DBG() << "genericIconName():" << genericIconName();
    DBG() << "iconName():" << iconName();
    DBG() << "globPatterns():" << globPatterns();
    DBG() << "suffixes():" << suffixes();
    DBG() << "preferredSuffix():" << preferredSuffix();
}
#endif

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QDeclarativeMimeType::~QDeclarativeMimeType();
    \brief Destroys the contained MimeType.
 */
QDeclarativeMimeType::~QDeclarativeMimeType()
{
    DBG() << "name():" << name();
    //DBG() << "aliases():" << aliases();
    //DBG() << "comment():" << comment();
    DBG() << "genericIconName():" << genericIconName();
    DBG() << "iconName():" << iconName();
    DBG() << "globPatterns():" << globPatterns();
    DBG() << "suffixes():" << suffixes();
    DBG() << "preferredSuffix():" << preferredSuffix();
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod void MimeType::assign(MimeType other)
    Assigns the data of \a other to this MimeType object.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \fn void QDeclarativeMimeType::assign(QDeclarativeMimeType *other);
    \brief Assigns the data of \a other to this QDeclarativeMimeType object.
 */
void QDeclarativeMimeType::assign(QDeclarativeMimeType *other)
{
    if (other == 0) {
        qWarning() << Q_FUNC_INFO << "other is 0!";
        m_MimeType = QMimeType();
        return;
    }

    m_MimeType = other->m_MimeType;
}

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \qmlmethod bool MimeType::equals(MimeType other)
    Returns true if \a other equals this MimeType object, otherwise returns false.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn bool QDeclarativeMimeType::equals(QDeclarativeMimeType *other) const;
    \brief Returns true if \a other equals this QDeclarativeMimeType object, otherwise returns false.
 */
bool QDeclarativeMimeType::equals(QDeclarativeMimeType *other) const
{
    if (other == 0) {
        qWarning() << Q_FUNC_INFO << "other is 0!";
        return false;
    }

    return this == other || m_MimeType == other->m_MimeType;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod object MimeType::properties()
    Returns the properties as a JavaScript object.
 */

// ------------------------------------------------------------------------------------------------

#define ASSIGN_TO_PROPERTY(name) \
    result[#name] = name();

/*!
    \internal
    \fn QVariantMap QDeclarativeMimeType::properties() const;
    \brief Returns the properties as a JavaScript object.
 */
QVariantMap QDeclarativeMimeType::properties() const
{
    QVariantMap result;
    ASSIGN_TO_PROPERTY(name)
    //ASSIGN_TO_PROPERTY(aliases)
    //ASSIGN_TO_PROPERTY(comment)
    ASSIGN_TO_PROPERTY(genericIconName)
    ASSIGN_TO_PROPERTY(iconName)
    ASSIGN_TO_PROPERTY(globPatterns)
    return result;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod void MimeType::assignProperties(object other)
    Assigns the data of the JavaScript object \a other to this MimeType object.
 */

// ------------------------------------------------------------------------------------------------

#define ASSIGN_FROM_PROPERTY(name, setter, variantType, converter) \
    if (!other.contains(#name)) { \
        if (false) { \
            qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO << "Variant does not contain" << #name; \
        } \
    } \
    else if (other[#name].type() != QVariant::variantType) { \
        if (false) { \
            qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO << "Variant for" << #name << "has wrong type" << other[#name].type() << "with" << other[#name]; \
        } \
    } \
    else { \
        setter(other[#name].converter()); \
    }

/*!
    \internal
    \fn void QDeclarativeMimeType::assignProperties(const QVariantMap &other);
    \brief Assigns the data of the QVariantMap \a other to this QDeclarativeMimeType object.
 */
void QDeclarativeMimeType::assignProperties(const QVariantMap &other)
{
    m_MimeType = QMimeType();
    ASSIGN_FROM_PROPERTY(name, setName, String, toString)
    //ASSIGN_FROM_PROPERTY(aliases, setAliases, List, toList)
    //ASSIGN_FROM_PROPERTY(comment, setComment, String, toString)
    ASSIGN_FROM_PROPERTY(genericIconName, setGenericIconName, String, toString)
    ASSIGN_FROM_PROPERTY(iconName, setIconName, String, toString)
    ASSIGN_FROM_PROPERTY(globPatterns, setGlobPatterns, List, toList)
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlmethod bool MimeType::equalsProperties(object other)
    Returns true if the properties of the JavaScript object \a other equal the properties of this MimeType object, otherwise returns false.
 */

// ------------------------------------------------------------------------------------------------

#define EQUALS_PROPERTY(name, variantType, converter) \
    if (!other.contains(#name)) { \
        if (false) { \
            qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO << "Variant does not contain" << #name << name(); \
        } \
        return false; \
    } \
    \
    if (other[#name].type() != QVariant::variantType) { \
        if (false) { \
            qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO << "Variant for" << #name << "has wrong type" << other[#name].type() << "for" << name() << "with" << other[#name]; \
        } \
        return false; \
    } \
    if (name() != other[#name].converter()) { \
        if (false) { \
            qDebug() << static_cast<const void *>(this) << Q_FUNC_INFO << "Values for" << #name << "differ:" << name() << other[#name]; \
        } \
        return false; \
    }

/*!
    \internal
    \fn bool QDeclarativeMimeType::equalsProperties(const QVariantMap &other) const;
    \brief Returns true if the properties of the JavaScript object \a other equal the properties of this QDeclarativeMimeType object, otherwise returns false.
 */
bool QDeclarativeMimeType::equalsProperties(const QVariantMap &other) const
{
    EQUALS_PROPERTY(name, String, toString)
    //EQUALS_PROPERTY(aliases, List, toList)
    //EQUALS_PROPERTY(comment, String, toString)
    EQUALS_PROPERTY(genericIconName, String, toString)
    EQUALS_PROPERTY(iconName, String, toString)
    EQUALS_PROPERTY(globPatterns, List, toList)

    return true;
}

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \fn QMimeType QDeclarativeMimeType::mimeType() const;
    \brief Returns the contained MimeType.
 */
QMimeType QDeclarativeMimeType::mimeType() const
{
    return m_MimeType;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty bool MimeType::isValid
    Holds the indication if the object contains valid data.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::isValid
    Holds the indication if the object contains valid data.
 */

// ------------------------------------------------------------------------------------------------

bool QDeclarativeMimeType::isValid() const
{
    return m_MimeType.isValid();
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty bool MimeType::isDebuggingActivated
    Holds the indication if debugging for the class is activated.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::isDebuggingActivated
    Holds the indication if debugging for the class is activated.
 */

// ------------------------------------------------------------------------------------------------

bool QDeclarativeMimeType::isDebuggingActivated() const
{
    return qt_isQMimeTypeDebuggingActivated;
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setIsDebuggingActivated(const bool newIsDebuggingActivated)
{
    qt_isQMimeTypeDebuggingActivated = newIsDebuggingActivated;
}

// ------------------------------------------------------------------------------------------------

#ifndef Q_COMPILER_RVALUE_REFS
QMIMETYPE_BUILDER
#else
QMIMETYPE_BUILDER_FROM_RVALUE_REFS
#endif

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty string MimeType::name
    Holds the name of the MIME type.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::name
    Holds the name of the MIME type.
 */

// ------------------------------------------------------------------------------------------------

QString QDeclarativeMimeType::name() const
{
    return m_MimeType.name();
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setName(const QString &newName)
{
#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(newName, m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#else
    m_MimeType = buildQMimeType(QString(newName), m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#endif
}

// ------------------------------------------------------------------------------------------------

#if 0
QVariantList QDeclarativeMimeType::aliases() const
{
    QVariantList result;

    foreach (const QString &str, m_MimeType.aliases()) {
        result << str;
    }

    return result;
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setAliases(const QVariantList &newAliases)
{
    QList<QString> newAliasesStringList;

    foreach (const QVariant &variant, newAliases) {
        if (variant.type() != QVariant::String) {
            qWarning() << Q_FUNC_INFO << "variant" << variant << "is not a string!";
            continue;
        }

        newAliasesStringList << variant.toString();
    }

#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(m_MimeType.name(), newAliasesStringList, m_MimeType.comment(), m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#else
    m_MimeType = buildQMimeType(m_MimeType.name(), QVariantList(newAliasesStringList), m_MimeType.comment(), m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#endif
}

// ------------------------------------------------------------------------------------------------

QString QDeclarativeMimeType::comment() const
{
    return m_MimeType.comment();
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setComment(const QString &newComment)
{
#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(m_MimeType.name(), /*m_MimeType.aliases(),*/ newComment, m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#else
    m_MimeType = buildQMimeType(m_MimeType.name(), /*m_MimeType.aliases(),*/ QString(newComment), m_MimeType.genericIconName(), m_MimeType.iconName(), m_MimeType.globPatterns());
#endif
}

// ------------------------------------------------------------------------------------------------
#endif

/*!
    \qmlproperty string MimeType::genericIconName
    Holds the file name of an icon image that represents the MIME type.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::genericIconName
    Holds the file name of an icon image that represents the MIME type.
 */

// ------------------------------------------------------------------------------------------------

QString QDeclarativeMimeType::genericIconName() const
{
    return m_MimeType.genericIconName();
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setGenericIconName(const QString &newGenericIconName)
{
#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(m_MimeType.name(), newGenericIconName, m_MimeType.iconName(), m_MimeType.globPatterns());
#else
    m_MimeType = buildQMimeType(m_MimeType.name(), QString(newGenericIconName), m_MimeType.iconName(), m_MimeType.globPatterns());
#endif
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty string MimeType::iconName
    Holds the file name of an icon image that represents the MIME type.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::iconName
    Holds the file name of an icon image that represents the MIME type.
 */

// ------------------------------------------------------------------------------------------------

QString QDeclarativeMimeType::iconName() const
{
    return m_MimeType.iconName();
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setIconName(const QString &newIconName)
{
#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(m_MimeType.name(), m_MimeType.genericIconName(), newIconName, m_MimeType.globPatterns());
#else
    m_MimeType = buildQMimeType(m_MimeType.name(), m_MimeType.genericIconName(), QString(newIconName), m_MimeType.globPatterns());
#endif
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty list<string> MimeType::globPatterns
    Holds the list of glob matching patterns.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::globPatterns
    Holds the list of glob matching patterns.
 */

// ------------------------------------------------------------------------------------------------

QVariantList QDeclarativeMimeType::globPatterns() const
{
    QVariantList result;

    foreach (const QString &str, m_MimeType.globPatterns()) {
        result << str;
    }

    return result;
}

// ------------------------------------------------------------------------------------------------

void QDeclarativeMimeType::setGlobPatterns(const QVariantList &newGlobPatterns)
{
    QList<QString> newGlobPatternsStringList;

    foreach (const QVariant &variant, newGlobPatterns) {
        if (variant.type() != QVariant::String) {
            qWarning() << Q_FUNC_INFO << "variant" << variant << "is not a string!";
            continue;
        }

        newGlobPatternsStringList << variant.toString();
    }

#ifndef Q_COMPILER_RVALUE_REFS
    m_MimeType = buildQMimeType(m_MimeType.name(), m_MimeType.genericIconName(), m_MimeType.iconName(), newGlobPatternsStringList);
#else
    m_MimeType = buildQMimeType(m_MimeType.name(), m_MimeType.genericIconName(), m_MimeType.iconName(), QStringList(newGlobPatternsStringList));
#endif
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty list<string> MimeType::suffixes
    Holds the known suffixes for the MIME type.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::suffixes
    Holds the known suffixes for the MIME type.
 */

// ------------------------------------------------------------------------------------------------

QVariantList QDeclarativeMimeType::suffixes() const
{
    QVariantList result;

    foreach (const QString &str, m_MimeType.suffixes()) {
        result << str;
    }

    return result;
}

// ------------------------------------------------------------------------------------------------

/*!
    \qmlproperty string MimeType::preferredSuffix
    Holds the preferred suffix for the MIME type.
 */

// ------------------------------------------------------------------------------------------------

/*!
    \internal
    \property QDeclarativeMimeType::preferredSuffix
    Holds the preferred suffix for the MIME type.
 */

// ------------------------------------------------------------------------------------------------

QString QDeclarativeMimeType::preferredSuffix() const
{
    return m_MimeType.preferredSuffix();
}

// ------------------------------------------------------------------------------------------------

#undef DBG

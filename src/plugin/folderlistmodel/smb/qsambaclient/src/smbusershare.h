/**************************************************************************
 *
 * Copyright 2014 Canonical Ltd.
 * Copyright 2014 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: smbusershare.h
 * Date: 02/12/2014
 */

#ifndef SMBUSERSHARE_H
#define SMBUSERSHARE_H


#include <QString>
#include <QObject>

/*!
 * \brief The SmbUserShare class  provides simple mechanisms to create/remove Samba shares on the localhost
 *
 * <p> User shares configuration files are under \b /var/lib/samba/usershares/
 *
 * Tests:
 * \li   Works: For a user who already has shares, just creating a new share file under /var/lib/samba/usershares/
 * \li   Works: For a new created user, just creating a new share file under /var/lib/samba/usershares/
 * \li   Works: using commnad line: net usershare add created_net_usershare `pwd`/created_net_usershare "created ny SmbUserShare"  everyone:F guest_ok=y
 * \li   Works: using command line: net usershare add created_net_usershare `pwd`/created_net_usershare "created ny SmbUserShare"  everyone:R guest_ok=n
 * \li   Works: using command line: net usershare delete created_net_usershare
 * \li   Works: if the usershare exits prints the name: net usershare list created_net_usershare
 *
 * User share File example:
 * \code
 *            path=/home/carlos/samba
 *            comment=
 *            usershare_acl=S-1-1-0:d,S-1-22-1-1000:r
 *            guest_ok=y
 *            sharename=samba
 * \endcode
 *
 * Once the user is suitable set a new share is created by just by creating a new file under  /var/lib/samba/usershares/
 *
 * Where \a \b usershare_acl is: (\sa \link http://devarthur.blogspot.de/2008/05/integrating-ubuntu-hardy-heron-804-with.html)
 * \code
 *       usershare_acl=Group_SID:access_modifier
 *
 *       SID "S-1-1-0" is Everyone
 *
 *       The access modifiers after the group SID are as follows:
 *       R - read-only
 *       F - full access
 *       D - deny access
 * \endcode
 */

class SmbUserShare : public QObject
{
public:

    /*!
     * \brief The Access enum
     *
     *  For simplification purposes Deny users are not available
     */
    Q_ENUMS(Access)
    /*!
     * \brief The Access enum specifies how a user share is created by this class
     *
     *  \TODO  create some deny mechanism
     */
    enum Access
    {
        None,         //!<  Error, perhaps the share does not exist
        Readonly,
        ReadWrite
        //TODO allow deny users
    };


    /*!
     * \brief The UserShareFile struct keeps information from usershare config files
     */
    struct UserShareFile
    {
        QString path;
        QChar   everyoneFlag;
        QChar   guest_ok;
        QString name;
        bool    isGuestAllowed() const  { return guest_ok == QChar('y'); }
        Access  getAccess() const
        {
             Access ret = None;
             if (everyoneFlag == QChar('f'))
             {
                 ret = ReadWrite;
             }
             else
             if (everyoneFlag == QChar('r'))
             {
                 ret = Readonly;
             }
             return ret;
        }
        bool exists()  const;
    };



    explicit SmbUserShare(QObject *parent = 0);
    ~SmbUserShare();

    /*!
     * \brief SmbUserShare::canCreateShares()
     *
     *  Finds for "net" in the PATH as it will be used to create/remove shares
     *
     * \return true if the executable "net" exists in the PATH
     */
    Q_INVOKABLE static bool      canCreateShares();

    /*!
     * \brief SmbUserShare::createShareForFolder() attempts to create a user share using "net" command
     * \param fulldirpath  the full path of the directory
     * \param access       [optional] defaults to ReadOnly
     * \param allowGuests  [optional] defaults to true
     * \param name         [optional] defaults to \ref proposedName()
     * \return true when the "net" returns zero
     */
    Q_INVOKABLE static bool      createShareForFolder(const QString& fulldirpath,   Access access = Readonly, bool allowGuests = true, const QString& name = QLatin1String(0));

    /*!
     * \brief SmbUserShare::removeShare() attempts to remove a share related to
     * \param name_OR_fulldirpath  it can be a share name or a dirpathname which has a share in it
     * \return
     */
    Q_INVOKABLE static bool      removeShare(const QString& name_OR_fulldirpath);

    /*!
     * \brief QSmbShareCreation::proposedName() returns a suitable share name for a such directory
     * \param fulldirpath full path of the directory where a share is being to created/removed
     * \return name with no spaces
     */
    Q_INVOKABLE static QString   proposedName(const QString& fulldirpath);

    /*!
     * \brief QSmbShareCreation::getAccess() returns the main Access of a share, \ref None when it does not exist
     */
    Q_INVOKABLE static Access    getEveryoneAccess(const QString& name_OR_fulldirpath);

    /*!
     * \brief QSmbShareCreation::isGuestAllowed()
     * \return  true if guest can browse the share
     */
    Q_INVOKABLE static bool      isGuestAllowed(const QString& name_OR_fulldirpath);

    static   UserShareFile       search(const QString& name_OR_fulldirpath);
    static   UserShareFile       readConfigFile(const QString& pathname);

signals:
    static QString error();


private:
    static   QString             m_error;

#if defined(REGRESSION_TEST_QSAMBACLIENT)
    friend  class TestQSambaSuite;
#endif
};


#endif // SMBUSERSHARE_H

/**************************************************************************
 *
 * Copyright 2015 Canonical Ltd.
 * Copyright 2015 Carlos J Mazieri <carlos.mazieri@gmail.com>
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
 * File: networklocation.h
 * Date: 08/12/2015
 */

#ifndef NETWORKLOCATION_H
#define NETWORKLOCATION_H

#include "location.h"

/*!
 * \brief The NetworkLocation class is an Abstract class suitable for Network protocols easy browsing implementation
 *
 * Support to new protocols can have its browsing provided by this class and for the generic class \ref NetworkListWorker.
 *
 * To get the browsing working on a network protocol just create a new \ref Location class inherited from NetworkLocation,
 *  then provide both suitable classes \ref DirItemInfo and  \ref descendant classes for this new \ref Location.
 *
 * The browsing itself will be performed inside a secondary thread using the \ref NetworkListWorker class.
 *
 *\note  For this new \ref Location class it is also necessary:
 *    \li an enumerator item (the type) needs be added into Location::Locations
 *    \li the corresponding protocol URL need be registered in \ref LocationUrl
 *    \li an instance of this class needs to be created in the \ref LocationsFactory creator
 *
 * Minimal example of adding a new Location class the adds support to a new protocol in the File Manager:
 *\code
 *
 *  class NewLocation : public NetworkLocation
 *  {
 *  public:
 *      explicit NewLocation(int type, QObject *parent=0) : NetworkLocation(type, parent) {}
 *      ~NewLocation() {}
 *
 *  public:
 *      virtual DirItemInfo * newItemInfo(const QString& urlPath)
 *      {
 *          //provide the suitable DirItemInfo inherited class here
 *      }
 *
 *      virtual LocationItemDirIterator * newDirIterator(const QString & path,
 *                                                        QDir::Filters filters,
 *                                                        QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags,
 *                                                        LocationItemDirIterator::LoadMode loadmode = LocationItemDirIterator::LoadOnConstructor)
 *      {
 *        //provide the suitable LocationItemDirIterator object for the new protocol here
 *      }
 *
 *      virtual LocationItemFile   * newFile(const QString & path) { return 0; }                  //used only in Actions, browsing does not use it
 *      virtual LocationItemDir    * newDir(const QString & dir = QLatin1String(0)) { return 0; } //used only in Actions, browsing does not use it
 *
 *      virtual QString     urlBelongsToLocation(const QString& urlPath, int indexOfColonAndSlash)
 *      {
 *          // provide some kind of URL parsing for the new protocol, see other implementations
 *      }
 *  };
 *\endcode
 *
 *  \sa \ref SmbLocation, \ref SmbItemInfo, \ref SmbLocationDirIterator, \ref SmbLocationItemFile, \ref SmbLocationItemDir,
 *      \ref SmbLocationAuthentication , \ref NetAuthenticationData and \ref NetAuthenticationDataList
 */

class NetworkLocation: public Location
{
protected:
    explicit NetworkLocation(int type, QObject *parent = 0);
public:
    virtual DirListWorker *newListWorker(const QString &urlPath,
                                         QDir::Filters filter,
                                         const bool isRecursive);
};

#endif // NETWORKLOCATION_H

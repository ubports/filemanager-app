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
 * File: main_qsamba.cpp
 * Date: 03/12/2014
 */

#include "testqsambasuite.h"

#include <QCoreApplication>
#include <QStringList>
#include <QTest>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TestQSambaSuite tc;
    QStringList args = QCoreApplication::arguments();
    QString permanentShareToCreate;
    QString permanentShareToOpen;
    bool    listAll = false;
    bool    listLocal = false;
    int count = args.count();
    while (count--)
    {
       //Ubuntu Touch parameter
       if (args.at(count).startsWith(QLatin1String("--desktop_file_hint")))
       {
          args.removeAt(count);        
       }
       else
       if (args.at(count).startsWith(QLatin1String("--create")) && count > 0)
       {
           permanentShareToCreate = args.at(count+1);
           args.removeAt(count+1);
           args.removeAt(count);
       }
       else
       if (args.at(count).startsWith(QLatin1String("--open")) && count > 0)
       {
           permanentShareToOpen = args.at(count+1);
           args.removeAt(count+1);
           args.removeAt(count);
       }
       else
       if (args.at(count).startsWith(QLatin1String("--lall")) && count > 0)
       {
           listAll = true;
           args.removeAt(count);
       }
       else
       if (args.at(count).startsWith(QLatin1String("--llocal")) && count > 0)
       {
           listLocal = true;
           args.removeAt(count);
       }
    }

    if (!permanentShareToOpen.isEmpty())
    {
        return tc.openPermanenteShare(permanentShareToOpen) ? 0 : 1;
    }
    if (!permanentShareToCreate.isEmpty())
    {
        return  tc.createPermanentShare(permanentShareToCreate) ? 0 : 1;
    }
    if (listAll)
    {
       return tc.listRecursive();
    }
    if (listLocal)
    {
        return tc.listLocalhost();
    }

    return  QTest::qExec(&tc, args);
}




/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "tupapplication.h"
#include "tapplicationproperties.h"

/**
 * Support Class for main.cpp
 * This class contains some of the basic methods required when Tupi is launched
 * @author David Cuadrado
*/

TupApplication::TupApplication(int &argc, char **argv) : TApplication(argc, argv)
{
    setApplicationName("tupi");
}

TupApplication::~TupApplication()
{
    #ifdef K_DEBUG
        QString msg = "[Destroying ~TupApplication]";
        #ifdef Q_OS_WIN
           qDebug() << msg;
        #else
           tDebug() << msg;
        #endif
    #endif
}

void TupApplication::createCache(const QString &cacheDir)
{
    QDir cache(cacheDir);
    if (!cache.exists()) {
        #ifdef K_DEBUG
            QString msg = "Initializing repository: " + cacheDir;
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

       if (!cache.mkdir(cacheDir)) {
           #ifdef K_DEBUG
               QString msg = "TupApplication::createCache() - Fatal Error: Can't create project repository";
               #ifdef Q_OS_WIN
                   qDebug() << msg;
               #else
                   tError() << msg;
               #endif
           #endif
       }
    }

    qDebug() << "TupApplication::createCache() - cacheDir: " << cacheDir;

    kAppProp->setCacheDir(cacheDir);
}

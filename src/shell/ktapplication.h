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
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#ifndef KTAPPLICATION_H
#define KTAPPLICATION_H

#include "kapplication.h"

/**
 * Support Class for main.cpp
 * This class contains some of the basic methods required when Tupi is launched
 * @author David Cuadrado <krawek@toonka.com>
*/
class KTApplication : public KApplication
{
    Q_OBJECT;
    public:
        KTApplication(int &argc, char **argv);
        ~KTApplication();

    public slots:
        /**
         * @if english
         * Opens a settings wizard the first time Tupi is launched
         * @endif
         * @if spanish
         * Lanza un wizard de configuracion la primera vez que se inicia la aplicacion
         * @endif
         * @return true/false if the application has the settings file created
         */
         virtual bool firstRun();

        /**
         * @if english
         * Creates a cache directory with the path defined at cacheDir variable
         * @endif
         * @if spanish
         * Crea el cache en la ruta especificada por la variable cacheDir
         * @endif
         * @param cacheDir the path for the repository dir
         */
         void createCache(const QString &cacheDir);
};

#endif

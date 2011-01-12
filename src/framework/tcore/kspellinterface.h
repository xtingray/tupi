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

#ifndef KSPELLINTERFACE_H
#define KSPELLINTERFACE_H

#include <QString>
#include <QStringList>
#include "kglobal.h"

/**
 * @if english
 * Abstract class that define a interface for spell programs, such as aspell, ispell, pspell
 * @elseif spanish
 * Clase abstracta que define una interfaz para programas de ortografia
 * @endif
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_CORE_EXPORT KSpellInterface
{
    public:
        virtual ~KSpellInterface() {}
        /**
         * @if english
         * This function checks out if a word is well spelled 
         * @endif
         * @param word 
         * @return 
         */
        virtual bool checkWord(const QString &word) = 0;
        
        /**
         * @if english
         * This function returns a list of suggested words
         * @endif
         * @param word 
         * @return 
         */
        virtual QStringList suggestions(const QString &word) = 0;
};

#endif

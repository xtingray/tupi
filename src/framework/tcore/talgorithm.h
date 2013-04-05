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

#ifndef TALGORITHM_H
#define TALGORITHM_H

#include <QString>
#include <QColor>

#include "tglobal.h"

/**
 * Class containing generic useful algorithms about random values
 * @author David Cuadrado
*/

class T_CORE_EXPORT TAlgorithm
{
    public:
        /**
         * @if english
         * Returns a random integer
         * @endif
         * @return 
         */
        static int random();

        /**
         * @if english
         * Returns a random string, this may be useful for temporary file names
         * @endif
         * @param length 
         * @return 
         */
        static QString randomString(int length);

        /**
         * @if english
         * Returns a random color
         * @endif
         * 
         * @param withAlpha 
         * @return 
         */
        static QColor randomColor(bool withAlpha = false);

        static QStringList header(const QString &input);
};

#endif

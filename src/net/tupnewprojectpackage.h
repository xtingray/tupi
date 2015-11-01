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

#ifndef TUPNEWPROJECTPACKAGE_H
#define TUPNEWPROJECTPACKAGE_H

#include "tglobal.h"

#include <QDomDocument>

/**
 * @author Jorge Cuadrado
*/

class TUPI_EXPORT TupNewProjectPackage : public QDomDocument
{
    public:
        TupNewProjectPackage(const QString & name, const QString & author, const QString & description, 
                            const QString & bgcolor, const QString & dimension, const QString & fps);

        ~TupNewProjectPackage();
        
        void setName(const QString & name);
        void setAuthor(const QString & author);
        void setDescription(const QString & description);
        void setBgColor(const QString & bgcolor);
        void setDimension(const QString & dimension);
        void setFps(const QString & fps);
        
    private:
        struct Private;
        Private *const k;
};

#endif

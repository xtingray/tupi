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

#ifndef KTPALETTEDOCUMENT_H
#define KTPALETTEDOCUMENT_H

#include <QDomDocument>
#include <QBrush>
#include <QList>
#include "ktglobal_store.h"

/**
 * Esta clase se utiliza para guardar las paletas
 * @brief Esta clase representa un documento de paletta
 * @author David Cuadrado
*/

class STORE_EXPORT KTPaletteDocument : public QDomDocument
{
    public:
        /**
         * Construye un documento de paleta con un nombre
         */
        KTPaletteDocument(const QString &name, bool isEditable);
        
        /**
         * Destructor
         */
        ~KTPaletteDocument();
        
        /**
         * Añade un gradiente al documento
         */
        void addGradient(const QGradient &gradient);
        
        /**
         * Añade un color al documento
         */
        void addColor(const QColor &color);
        
        /**
         * Pone una lista de colores o gradientes en el documento
         */
        void setElements(const QList<QBrush > &colors);
};

#endif

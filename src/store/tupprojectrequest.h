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

#ifndef TUPPROJECTREQUEST_H
#define TUPPROJECTREQUEST_H

#include "tglobal.h"

#include <QObject>
#include <QString>
#include <QVariant>

class TupProjectRequest;
class TupProjectResponse;

class TUPI_EXPORT TupProjectRequestArgument
{
    public:
        TupProjectRequestArgument();
        TupProjectRequestArgument(const QString &v);
        ~TupProjectRequestArgument();
        
        void operator = (const QString &value);
        void setValue(const QString &value);
        
        bool toBool();
        int toInt();
        double toReal();
        QString toString();
        
    private:
        QString m_value;
};

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupProjectRequest
{
    public:
        enum Action
        {
            None = 0,
            /**
             * Anade una parte, el tipo de data es nulo
             */
            Add = 1,
            /**
             * Remueve una parte, el tipo de data es nulo
             */
            Remove = -Add,
            /**
             * Reinicia los valores de un objeto
             */
            Reset = 2,
            /**
             * Intercambia del valor entre dos posiciones 
             */
            Exchange = 3,
            /**
             * Mueve una parte, el tipo de data es un entero que significa la nueva posicion
             */
            Move = 4,
            /**
             * Bloquea una parte, el tipo de data es booleano.
             */
            Lock = 5,
            /**
             * Renombra una parte, el tipo de data es una cadena de texto.
             */
            Rename = 6,
            /**
             * Selecciona una parte, el tipo de data es nulo
             */
            Select = 7,
            /**
             * Cambia la visibilidad
             */
            View = 8,
            
            Group = 9,
            Ungroup = -Group,
            
            // Items
            Transform = 10,
            Convert = 11,
            EditNodes = 12,
            InsertSymbolIntoFrame = 13,
            RemoveSymbolFromFrame = 14,

            SetTween = 15,
            UpdateTween = 16,
            RemoveTween = 17,
            
            // frames
            Update = 18,
            Expand = 19,
            Copy = 20,
            Paste = 21,

            // scenes
            GetInfo = 22, 
            BgColor = 23,

            // layer
            AddLipSync = 24,
            UpdateLipSync = 25,
            RemoveLipSync = 26,
            UpdateOpacity = 27
        };
        
        enum Part
        {
            Project = 1000,
            Scene,
            Layer,
            Frame,
            Item,
            Library
        };
        
        TupProjectRequest(const QString &xml = 0);
        virtual ~TupProjectRequest();
        
        void setId(int id);
        virtual int id() const;
        virtual bool isValid() const;
        
        QString xml() const;
        
        void setExternal(bool b);
        bool isExternal() const;
        
        TupProjectRequest &operator = (const TupProjectRequest &other);

    private:
        struct Private;
        Private *const k;
};

#endif

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

#ifndef KTLAYER_H
#define KTLAYER_H

#include "ktabstractserializable.h"
#include "ktframe.h"

#include <QDomDocument>
#include <QDomElement>

#include "ktinthash.h"

#include "ktglobal_store.h"

typedef KTIntHash<KTFrame *> Frames;

class KTScene;
class KTProject;

/**
 * @brief This class represents a layer. Layers are defined by the KTDocument class and they contain KTFrames
 * @author David Cuadrado 
*/

class STORE_EXPORT KTLayer : public QObject, public KTAbstractSerializable
{
    Q_OBJECT
    public:
        /**
         * Default Constructor
         */
        KTLayer(KTScene *parent);
        
        /**
         * Destructor
         */
        ~KTLayer();
        
        /**
         * Retorna los frames del layer
         */
        Frames frames();
        
        /**
         * Pone la lista de frames, esta funcion reemplaza los frames anteriores
         */
        void setFrames(const Frames &frames);
        
        /**
         * Pone el nombre del layer
         */
        void setLayerName(const QString &name);
        
        /**
         * Bloquea el layer
         */
        void setLocked(bool isLocked);
        
        /**
         * Pone la visibilidad del layer
         */
        void setVisible(bool isVisible);
        
        /**
         * Retorna el nombre del layer
         */
        QString layerName() const;
        
        /**
         * Returna verdadero cuando el layer esta bloqueado
        */
        bool isLocked() const;
        
        /**
         * Retorna verdadero si el layer es visible
         */
        bool isVisible() const;
        
        KTFrame *createFrame(int position, bool loaded = false);

        bool removeFrame(int position);

        bool resetFrame(int position);
        
        bool moveFrame(int from, int to);

        bool exchangeFrame(int from, int to);
        
        bool expandFrame(int position, int size);
        
        KTFrame *frame(int position) const;
        
        KTScene *scene() const;
        KTProject *project() const;
        
        //int logicalIndexOf(KTFrame *frame) const;
        int visualIndexOf(KTFrame *frame) const;
        
        //int logicalIndex() const;
        int objectIndex() const;

        int framesNumber() const;

        //void setZLevel(int level);
        //int getZLevel();
        
    public:
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;
        
    private:
        struct Private;
        Private *const k;
};

#endif

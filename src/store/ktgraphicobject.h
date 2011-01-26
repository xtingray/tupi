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

#ifndef KTGRAPHICOBJECT_H
#define KTGRAPHICOBJECT_H

#include <QObject>
#include "ktabstractserializable.h"
#include "ktframe.h"
#include "ktglobal_store.h"

class QGraphicsItem;
class KTItemTweener;

/**
 * @author David Cuadrado
*/
class STORE_EXPORT KTGraphicObject : public QObject, public KTAbstractSerializable
{
    public:
        enum Transformations { ScaleX = 1, ScaleY, Rotate, TranslateX, TranslateY };
        
        KTGraphicObject(QGraphicsItem *item, KTFrame *parent);
        ~KTGraphicObject();
        
        void setItem(QGraphicsItem *item);
        QGraphicsItem *item() const;
        
        void setObjectName(const QString &name);
        QString objectName() const;
        
        void setTween(KTItemTweener *tween);
        bool hasTween() const;
        void removeTween();
        KTItemTweener *tween() const;
        
        KTFrame *frame() const;
        int objectIndex() const;
        void setFrame(KTFrame *frame);

        void setLastTweenPos(QPointF point);
        QPointF lastTweenPos();
        
    public:
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc)  const;
        
    private:
        void initItemData();
        
        struct Private;
        Private *const k;
};

#endif

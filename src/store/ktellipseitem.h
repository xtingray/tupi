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

#ifndef KTELLIPSEITEM_H 
#define KTELLIPSEITEM_H

#include "ktabstractserializable.h"
#include <QGraphicsEllipseItem>
#include "ktglobal_store.h"

/**
 * @author Jorge Cuadrado
*/

class STORE_EXPORT KTEllipseItem: public KTAbstractSerializable, public QGraphicsEllipseItem
{
    public:
        KTEllipseItem(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
        KTEllipseItem(const QRectF & rect, QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
        ~KTEllipseItem();
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;
        bool contains(const QPointF & point) const;
          
    protected:
        virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
        virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
        virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
          
    private:
        bool m_dragOver;
};

#endif

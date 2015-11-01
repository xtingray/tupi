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

#include "tupitemconverter.h"
#include "tuprectitem.h"
#include "tuppathitem.h"
#include "tuplineitem.h"
#include "tupellipseitem.h"
#include "tupproxyitem.h"
#include "tupitemgroup.h"

TupItemConverter::TupItemConverter()
{
}

TupItemConverter::~TupItemConverter()
{
}

void TupItemConverter::copyProperties(QGraphicsItem *src, QGraphicsItem *dest)
{
    dest->setMatrix(src->matrix());
    dest->setPos(src->scenePos());
    dest->setFlags(src->flags() );
    dest->setSelected(src->isSelected());
    
    // Shapes
    QAbstractGraphicsShapeItem *shape =  dynamic_cast<QAbstractGraphicsShapeItem*>(src);
    QAbstractGraphicsShapeItem *shapeDst = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(dest);
    
    if (shape && dest) {
        QBrush shapeBrush = shape->brush();
        
        if (shapeBrush.color().isValid() || shapeBrush.gradient() || 
        !shapeBrush.texture().isNull())
            shapeDst->setBrush( shape->brush());
        
        shapeDst->setPen(shape->pen());
    }
}

TupPathItem *TupItemConverter::convertToPath(QGraphicsItem *item)
{
    if (!item) 
        return 0;
    
    TupPathItem *path = new TupPathItem(item->parentItem());
    
    QPainterPath ppath;
    
    switch (item->type()) {
        case TupPathItem::Type:
        {
            ppath = qgraphicsitem_cast<TupPathItem *>(item)->path();
        }
        break;

        case TupRectItem::Type:
        {
            ppath.addRect(qgraphicsitem_cast<TupRectItem *>(item)->rect());
        }
        break;

        case TupEllipseItem::Type:
        {
            ppath.addEllipse(qgraphicsitem_cast<TupEllipseItem *>(item)->rect());
        }
        break;

        case TupProxyItem::Type:
        {
            QGraphicsItem * data = qgraphicsitem_cast<TupProxyItem*>(item)->item();
            data->setPos(item->scenePos());
            return convertToPath(data);
        }
        break;

        case TupLineItem::Type:
        {
            QLineF line = qgraphicsitem_cast<TupLineItem *>(item)->line();
            ppath.moveTo(line.p1());
            ppath.lineTo(line.p2());
        }
        break;

        case TupItemGroup::Type:
        {
            #ifdef K_DEBUG
                QString msg = "TupItemConverter::convertToPath - Error: Group items are not supported";
                #ifdef Q_OS_WIN32
                    qWarning() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif	
            delete path;
            return 0;
        }
        break;

        default:
        {
            #ifdef K_DEBUG
                QString msg = "TupItemConverter::convertToPath - Using default converter...";
                #ifdef Q_OS_WIN32
                    qWarning() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif			
            ppath = item->shape(); // TODO
        }
        break;
    }
    
    path->setPath(ppath);
    TupItemConverter::copyProperties(item, path);
    
    return path;
}

TupEllipseItem *TupItemConverter::convertToEllipse(QGraphicsItem *item)
{
    TupEllipseItem *ellipse = new TupEllipseItem(item->parentItem());
    
    switch (item->type()) {

        case TupPathItem::Type:
        {
            ellipse->setRect(qgraphicsitem_cast<QGraphicsPathItem *>(item)->path().boundingRect());
        }
        break;

        case TupEllipseItem::Type:
        {
            ellipse->setRect(qgraphicsitem_cast<QGraphicsEllipseItem *>(item)->rect());
        }
        break;
        // TODO: default case
    }
    
    TupItemConverter::copyProperties(item, ellipse);
    
    return ellipse;
}

TupRectItem *TupItemConverter::convertToRect(QGraphicsItem *item)
{
    TupRectItem *rect = new TupRectItem(item->parentItem());
    
    switch (item->type()) {

        case TupPathItem::Type:
        {
            rect->setRect(qgraphicsitem_cast<QGraphicsPathItem *>(item)->path().boundingRect());
        }
        break;

        case TupEllipseItem::Type:
        {
            rect->setRect(qgraphicsitem_cast<QGraphicsEllipseItem *>(item)->rect());
        }
        break;
        // TODO: include other kind of objects
    }
    
    TupItemConverter::copyProperties(item, rect);
    
    return rect;
}

TupLineItem *TupItemConverter::convertToLine(QGraphicsItem *item)
{
    TupLineItem *line = new TupLineItem(item->parentItem());

    switch (item->type()) {

        case TupPathItem::Type:
        {
            QRectF rect = qgraphicsitem_cast<QGraphicsPathItem *>(item)->path().boundingRect();
            line->setLine(QLineF(rect.topLeft(), rect.bottomRight()));
        }
        break;

        case TupEllipseItem::Type:
        {
            QRectF rect = qgraphicsitem_cast<QGraphicsEllipseItem *>(item)->rect();
            line->setLine(QLineF(rect.topLeft(), rect.bottomRight()));
        }
        break;
        // TODO: include other kind of objects
    }

    TupItemConverter::copyProperties(item, line);
    
    return line;
}

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

#include "tuprectitem.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBrush>
#include <QGraphicsScene>

TupRectItem::TupRectItem(QGraphicsItem * parent, QGraphicsScene * scene) : QGraphicsRectItem(parent, scene)
{
    setAcceptDrops(true);
}

TupRectItem::TupRectItem(const QRectF& rect, QGraphicsItem * parent , QGraphicsScene * scene) : QGraphicsRectItem(rect, parent, scene)
{
}

TupRectItem::~TupRectItem()
{
}

void TupRectItem::fromXml(const QString &xml)
{
}

QDomElement TupRectItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("rect");
    
    root.setAttribute("x", rect().x());
    root.setAttribute("y", rect().y());
    root.setAttribute("width", rect().width());
    root.setAttribute("height", rect().height());
    
    root.appendChild(TupSerializer::properties(this, doc));
    
    QBrush brush = this->brush();
    root.appendChild(TupSerializer::brush(&brush, doc));
    
    QPen pen = this->pen();
    root.appendChild(TupSerializer::pen(&pen, doc));

    return root;
}

void TupRectItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        event->setAccepted(true);
        m_dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void TupRectItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    m_dragOver = false;
    update();
}

void TupRectItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    m_dragOver = false;
    if (event->mimeData()->hasColor()) {
        setBrush(QBrush(qVariantValue<QColor>(event->mimeData()->colorData())));
    } else if (event->mimeData()->hasImage()) {
               setBrush(QBrush(qVariantValue<QPixmap>(event->mimeData()->imageData())));
    }
    update();
}

bool TupRectItem::contains(const QPointF & point) const
{
/*
    double thickness = pen().widthF()+2;
    QRectF rectS(point-QPointF(thickness/2,thickness/2) , QSizeF(thickness,thickness));
    
    QPolygonF pol = shape().toFillPolygon();

    foreach (QPointF point, pol) {
        if (rectS.contains( point))
            return true;
    }
    
    QPolygonF::iterator it1 = pol.begin();
    QPolygonF::iterator it2 = pol.begin()+1;
    
    while (it2 != pol.end()) {
           QRectF rect((*it1).x(), (*it1).y(), (*it2).x()-(*it1).x(), (*it2).y()-(*it1).y());
        
           if (rect.intersects(rectS))
               return true;
           ++it1;
           ++it2;
    }
    
    return false;
*/
    
    return QGraphicsRectItem::contains(point);
}

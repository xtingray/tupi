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

#include "tupellipseitem.h"
#include "tupsvg2qt.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QBrush>
#include "tdebug.h"
#include <QPainter>
#include <QPainterPath>

TupEllipseItem::TupEllipseItem(QGraphicsItem * parent, QGraphicsScene * scene): QGraphicsEllipseItem(parent, scene), m_dragOver(false)
{
    setAcceptDrops(true);
}

TupEllipseItem::TupEllipseItem(const QRectF & rect, QGraphicsItem * parent, QGraphicsScene * scene): QGraphicsEllipseItem(rect, parent, scene), m_dragOver(false)
{
    setAcceptDrops(true);
}

TupEllipseItem::~TupEllipseItem()
{
    
}

void TupEllipseItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement TupEllipseItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("ellipse");
    
    root.setAttribute("cx", rect().center().x());
    root.setAttribute("cy", rect().center().y());
    root.setAttribute("rx", rect().width()/2);
    root.setAttribute("ry", rect().height()/2);
    
    root.appendChild(TupSerializer::properties(this, doc));
    
    QBrush brush = this->brush();
    root.appendChild(TupSerializer::brush(&brush, doc));
    
    QPen pen = this->pen();
    root.appendChild(TupSerializer::pen(&pen, doc));
    
    return root;
}

bool TupEllipseItem::contains(const QPointF & point) const
{
    // SQA: Check if this code is really doing something
    /*
    int thickness = pen().width();
    QRectF rectS(point-QPointF(thickness/2,thickness/2) , QSizeF(thickness,thickness));
    
    QPolygonF pol = shape().toFillPolygon ();
    foreach (QPointF point, pol) {
             if (rectS.contains( point))
                 return true;
    }
    QPolygonF::iterator it1 = pol.begin() ;
    QPolygonF::iterator it2 = pol.begin()+1;
    
    while (it2 != pol.end()) {
           if (TupGraphicalAlgorithm::intersectLine( (*it1), (*it2), rectS  ))
               return true;
           ++it1;
           ++it2;
    }
    return false;
    */

    return QGraphicsEllipseItem::contains(point);
}

void TupEllipseItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        event->setAccepted(true);
        m_dragOver = true;
        update();
    } else {
        event->setAccepted(false);
    }
}

void TupEllipseItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    m_dragOver = false;
    update();
}

void TupEllipseItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    m_dragOver = false;
    if (event->mimeData()->hasColor()) {
        setBrush(QBrush(qVariantValue<QColor>(event->mimeData()->colorData())));
    } else if (event->mimeData()->hasImage()) {
        setBrush(QBrush(qVariantValue<QPixmap>(event->mimeData()->imageData())));
    }
    update();
}

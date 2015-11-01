/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#include "mouthtarget.h"

/**
 * This class defines the data structure for a node, and all the methods required to manipulate it.
 * 
 * @author Gustav Gonzalez 
*/

MouthTarget::MouthTarget(const QPointF & pos, int zLevel) : QGraphicsItem(0)
{
    QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor));
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsFocusable, true);

    setZValue(zLevel);
    setPos(pos);
}

MouthTarget::~MouthTarget()
{
}

void MouthTarget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
    Q_UNUSED(w);
    Q_UNUSED(option);
    
    QColor color;
    color = QColor("green");
    color.setAlpha(200);

    QRectF square = boundingRect();
    painter->setBrush(color);
    painter->drawRoundRect(square);

    painter->save();
    color = QColor("white");
    color.setAlpha(220); 
    painter->setPen(color);
    QPointF point1 = QPointF(square.topLeft().x() + 3, square.topLeft().y() + 3); 
    QPointF point2 = QPointF(square.bottomRight().x() - 3, square.bottomRight().y() - 3);
    QPointF point3 = QPointF(square.bottomLeft().x() + 3, square.bottomLeft().y() - 3);
    QPointF point4 = QPointF(square.topRight().x() - 3, square.topRight().y() + 3);

    painter->drawLine(point1, point2);
    painter->drawLine(point3, point4);
    painter->restore();
}

QRectF MouthTarget::boundingRect() const
{
    QSizeF size(10, 10);
    QRectF rect(QPointF(-size.width()/2, -size.height()/2), size);

    return rect;
}

void MouthTarget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void MouthTarget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[MouthTarget::mouseReleaseEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    emit positionUpdated(pos());

    QGraphicsItem::mouseReleaseEvent(event);
}

void MouthTarget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

QPointF MouthTarget::currentPos()
{
    QPointF xy = pos();
    return xy;
}

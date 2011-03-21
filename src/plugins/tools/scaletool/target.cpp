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

#include "target.h"
#include "kdebug.h"
#include "ktgraphicalgorithm.h"
#include "ktgraphicobject.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QApplication>

#define DEBUG 0

/**
 * This class defines the data structure for a node, and all the methods required to manipulate it.
 * 
 * @author Gustav Gonzalez 
*/

struct Target::Private
{
    QGraphicsItem *parent;
    QPointF position;
};

Target::Target(const QPointF & pos, QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsItem(0, scene), k(new Private)
{
    k->parent = parent;

    QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor));
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsFocusable, true);

    setPos(pos);
    setZValue(parent->zValue() + 1);
}

Target::~Target()
{
    delete k;
}

void Target::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
    Q_UNUSED(w);
    Q_UNUSED(option);
    
    QColor color;
    color = QColor("green");
    color.setAlpha(180);

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

QRectF Target::boundingRect() const
{
    QSizeF size(10, 10);
    QRectF rect(QPointF(-size.width()/2, -size.height()/2), size);

    return rect;
}

void Target::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    K_FUNCINFO;
    QGraphicsItem::mousePressEvent(event);
}

void Target::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    K_FUNCINFO;

    kFatal() << "Target::mouseReleaseEvent() - New point: [" << event->scenePos().x() << ", " << event->scenePos().y() << "]";
    emit positionUpdated(event->scenePos()); 

    QGraphicsItem::mouseReleaseEvent(event);
}

void Target::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

QPointF Target::position()
{
    return k->position;
}

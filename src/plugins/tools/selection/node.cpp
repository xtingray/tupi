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

#include "node.h"
#include "nodemanager.h"
#include "tupgraphicalgorithm.h"
#include "tupgraphicobject.h"

/**
 * This class defines the data structure for a node, and all the methods required to manipulate it.
 * 
 * @author David Cuadrado
*/

struct Node::Private
{
    TypeNode typeNode;
    ActionNode action;
    bool notChange;
    ActionNode generalState; 
    QGraphicsItem *parent;
    NodeManager *manager;
    QSizeF size;
};

Node::Node(TypeNode node, ActionNode action, const QPointF &pos, NodeManager *manager, QGraphicsItem *parent, int zValue) : 
           QGraphicsItem(0), k(new Private)
{
    QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor));
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsFocusable, true);
    setPos(pos);

    k->typeNode = node;
    k->action = action;
    k->manager = manager;
    k->parent = parent;
    k->size = QSizeF(10, 10);
    k->generalState = Scale;

    setZValue(zValue);
}

Node::~Node()
{
    delete k;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
    Q_UNUSED(w);
    Q_UNUSED(option);
    
    QColor color;
   
    if (k->typeNode != Center) {
        if (k->action == Rotate) {
            color = QColor(255, 102, 0);
            color.setAlpha(180);
        } else {
            color = QColor("green");
            color.setAlpha(200);
        }
    } else {
        if (k->generalState == Scale) {
            color = QColor(150, 150, 150);
        } else {
           color = QColor(255, 0, 0);
        }
        color.setAlpha(150);
    }

    QRectF square = boundingRect();
    painter->setBrush(color);
    painter->drawRoundRect(square);

    /* SQA: Code for debugging purposes
    #ifdef K_DEBUG
        painter->setFont(QFont(painter->font().family(), 5));
        painter->drawText(square, QString::number(k->typeNode));
    #endif
    */

    if (k->typeNode == Center) {
        painter->save();
        color = QColor("white");
        color.setAlpha(220);
        painter->setPen(color);

        QPointF point1 = QPointF(square.topLeft().x() + 2, square.topLeft().y() + 2);
        QPointF point2 = QPointF(square.bottomRight().x() - 2, square.bottomRight().y() - 2);
        QPointF point3 = QPointF(square.bottomLeft().x() + 2, square.bottomLeft().y() - 2);
        QPointF point4 = QPointF(square.topRight().x() - 2, square.topRight().y() + 2);

        painter->drawLine(point1, point2);
        painter->drawLine(point3, point4);
        painter->restore();
    }
}

QRectF Node::boundingRect() const
{
    QRectF rect(QPointF(-k->size.width()/2, -k->size.height()/2), k->size);
    return rect;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Node::itemChange()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (change == ItemSelectedChange) {
        setVisible(true);
        if (value.toBool())
            k->parent->setSelected(true);
        k->manager->show();
    }

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Node::mousePressEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->manager->setPress(true);
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Node::mouseReleaseEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QGraphicsItem::mouseReleaseEvent(event);
    k->parent->setSelected(true);
    k->manager->setPress(false);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF newPos(event->scenePos());

    if (k->notChange) {
        k->notChange = false;
    } else {
        if (k->action == Scale) {
            QRectF rect = k->parent->sceneBoundingRect();
            QRectF parentRect  = k->parent->sceneBoundingRect();
            QRectF parentSquare  = k->parent->boundingRect();
            
            // SQA: Lines for debugging purposes
            /*
            scene()->addRect(rect, QPen(Qt::red));
            scene()->addRect(parentRect, QPen(Qt::green));
            */
            
            switch (k->typeNode) {
                case TopRight:
                {
                     k->manager->setAnchor(parentSquare.bottomLeft());
                     rect.setTopRight(newPos);
                     break;
                }
                case BottomRight:
                {
                     k->manager->setAnchor(parentSquare.topLeft());
                     rect.setBottomRight(newPos);
                     break;
                }
                case TopLeft:
                {
                     k->manager->setAnchor(parentSquare.bottomRight());
                     rect.setTopLeft(newPos);
                     break;
                }
                case BottomLeft:
                {
                     k->manager->setAnchor(parentSquare.topRight());
                     rect.setBottomLeft(newPos);
                     break;
                }
                case Center:
                {
                     break;
                }
            };
            
            float sx = 1, sy = 1;
            sx = static_cast<float>(rect.width()) / static_cast<float>(parentRect.width());
            sy = static_cast<float>(rect.height()) / static_cast<float>(parentRect.height());

            if (k->manager->proportionalScale()) {
                k->manager->scale(sx, sx);
            } else {
                if (sx > 0 && sy > 0) {
                    k->manager->scale(sx, sy);
                } else {
                    if (sx > 0)
                        k->manager->scale(sx, 1);

                    if (sy > 0)
                        k->manager->scale(1, sy);
                }
            }
        } else if (k->action == Rotate) {
                   QPointF pRef; 
                   bool isCenter = false;
                   switch (k->typeNode) {
                       case TopRight:
                       {
                           pRef = k->parent->sceneBoundingRect().topRight();
                           break;
                       }
                       case BottomRight:
                       {
                           pRef = k->parent->sceneBoundingRect().bottomRight();
                           break;
                       }
                       case TopLeft:
                       {
                           pRef = k->parent->sceneBoundingRect().topLeft();
                           break;
                       }
                       case BottomLeft:
                       {
                           pRef = k->parent->sceneBoundingRect().bottomLeft();
                           break;
                       }
                       case Center:
                       {
                           isCenter = true;
                           break;
                       }
                   };

                   if (!isCenter) {
                       QPointF p1 = newPos;
                       QPointF p2 = k->parent->sceneBoundingRect().center();
                       k->manager->setAnchor(k->parent->boundingRect().center());

                       QLineF line(p2, p1);
                       QLineF lineRef(p2, pRef);
                       double angle = line.angle() - lineRef.angle();
                       k->manager->rotate(angle);
                   }
        }
    }

    if (k->typeNode == Center) {
        k->parent->moveBy(event->scenePos().x() - scenePos().x() , event->scenePos().y() - scenePos().y());
        event->accept();
    }
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Node::mouseDoubleClickEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->manager->toggleAction();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

int Node::typeNode() const
{
    return k->typeNode;
}

void Node::setAction(ActionNode action)
{
    if (k->typeNode != Center)
        k->action = action;
    else
        k->action = Scale;

    if (k->generalState == Scale)
        k->generalState = Rotate;
    else
        k->generalState = Scale;

    update();
}

int Node::actionNode()
{
    return k->action;
}

void Node::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    k->manager->setProportion(false);
}

void Node::resize(qreal factor)
{
    setScale(factor);
}

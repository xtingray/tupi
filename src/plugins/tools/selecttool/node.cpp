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

#include "node.h"
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QApplication>

#include <QCursor>
#include "kdebug.h"
#include "nodemanager.h"

#include <cmath> //atan
#include "ktgraphicalgorithm.h"

#include "ktgraphicobject.h"

#define DEBUG 0

/**
 * This class defines the data structure for a node, and all the methods required to manipulate it.
 * 
 * @author David Cuadrado <krawek@toonka.com>
*/

struct Node::Private
{
    Private(TypeNode node, ActionNode action, NodeManager *manager, QGraphicsItem * parent) : typeNode(node),
                        action(action), notChange(false), parent(parent), manager(manager)
    {
    }
    
    TypeNode typeNode;
    ActionNode action;
    bool notChange;
    QGraphicsItem * parent;
    NodeManager *manager;
};

Node::Node(TypeNode node, ActionNode action, const QPointF & pos, NodeManager *manager, QGraphicsItem *parent,
        QGraphicsScene * scene) : QGraphicsItem(0, scene), k(new Private(node, action, manager, parent))
{
    QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor));
    // setParentItem(k->parent);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, true);
    
    setPos(pos);
    setZValue(parent->zValue() + 1);
}

Node::~Node()
{
    delete k;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w)
{
    Q_UNUSED(w);
    
    bool antialiasing =  painter->renderHints() & QPainter::Antialiasing;
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    QColor color;
    
    if ((option->state & QStyle::State_Sunken) && (k->action == Rotate)) {
        color = QColor("white");
        color.setAlpha(200);
    } else {
        color = QColor("green");
        color.setAlpha(200);
    }
    
    if (k->action == Rotate) {
        if (k->typeNode != Center) {
            color = QColor(31, 183, 180);
            color.setAlpha(150);
        } else {
            color = QColor(255, 0, 0);
            color.setAlpha(150);
        }
    }

    QRectF square = boundingRect();

    painter->setBrush(color);
    painter->drawRoundRect(square);

    //DEBUG
    #if DEBUG
        painter->setFont(QFont(painter->font().family(), 5));
        painter->drawText(square, QString::number(k->typeNode));
    #endif

    if (k->typeNode == Center) {
        painter->save();
        painter->setPen(Qt::gray);
        painter->drawLine(square.topLeft(), square.bottomRight());
        painter->drawLine(square.bottomLeft(), square.topRight());
        painter->restore();
    }
    
    painter->setRenderHint(QPainter::Antialiasing, antialiasing);
}

QRectF Node::boundingRect() const
{
    QSizeF size(8, 8);
    QRectF r(QPointF(-size.width()/2, -size.height()/2), size);

    return r;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change ==  ItemSelectedChange) {
        K_FUNCINFO;
        setVisible(true);
        
        if (value.toBool())
            k->parent->setSelected(true);

        k->manager->show();
    }

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    K_FUNCINFO;
    // update();
    k->manager->setPress(true);
    
    QGraphicsItem::mousePressEvent(event);
   
    /* 
    #if K_DEBUG
        QRectF r = k->parent->sceneMatrix().inverted().mapRect(k->parent->sceneBoundingRect());
        scene()->addRect(r, QPen(Qt::magenta), QBrush(QColor(100,100,200,50)));
        scene()->update(r);
    #endif
    */
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    K_FUNCINFO;
    // update();
    QGraphicsItem::mouseReleaseEvent(event);
    k->parent->setSelected(true);
    k->manager->setPress(false);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QPointF newPos(event->scenePos());

    if( k->notChange) {
        k->notChange = false;
    } else {
        if (k->action == Scale) {
            QRectF rect = k->parent->sceneBoundingRect();
            QRectF parentRect  = k->parent->sceneBoundingRect();
            QRectF parentSquare  = k->parent->boundingRect();
            
            //Debug
            /*
            scene()->addRect(rect, QPen(Qt::red));
            scene()->addRect(parentRect, QPen(Qt::green));
            */
            
            switch (k->typeNode) {
                    case TopLeft:
                    {
                         k->manager->setAnchor(parentSquare.bottomRight());
                         rect.setTopLeft(newPos);
                         break;
                    }
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
            
            if (sx > 0 && sy > 0) {
                k->manager->scale( sx,sy);
            } else {
                if (sx > 0)
                    k->manager->scale(sx, 1);

                if (sy > 0)
                    k->manager->scale(1, sy);
            }
        } else {
            
            if (k->typeNode != Center) {
                // k->manager->setVisible(false);
                QPointF p1 = newPos;
                QPointF p2 = k->parent->sceneBoundingRect().center();
                k->manager->setAnchor(k->parent->boundingRect().center());
                
                double a = (180 * KTGraphicalAlgorithm::angleForPos(p1, p2)) / M_PI;
                k->manager->rotate(a-45);
            }
        }
    }

    if (k->typeNode == Center) {
        k->parent->moveBy(event->scenePos().x() - scenePos().x() , event->scenePos().y() - scenePos().y());
        event->accept();
    }
}


void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * e)
{
    K_FUNCINFO;
    // update();

    //if (!m_parent->isSelected())
    //k->parent->setSelected(true);
    //k->parent->setFocus(Qt::MouseFocusReason);

    k->manager->toggleAction();

    //e->setAccepted(false);
    QGraphicsItem::mouseDoubleClickEvent(e);
}

int Node::typeNode() const
{
    return k->typeNode;
}

void Node::setAction(ActionNode action)
{
    k->action = action;
    update();
}

int Node::actionNode()
{
    return k->action;
}

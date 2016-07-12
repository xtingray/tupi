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

#include "nodemanager.h"
#include "tupgraphicobject.h"

/**
 * This class handles the methods required to manipulate a set of nodes.
 * 
 * @author Jorge Cuadrado
*/

struct NodeManager::Private
{
    QHash<Node::TypeNode, Node *> nodes;
    QGraphicsItem *parent;
    QGraphicsScene *scene;

    QTransform origTransform;
    QPointF origPos;
    QPointF itemCenter;

    bool isPressed;
    bool proportional;
    qreal rotation;
    qreal scaleX;
    qreal scaleY;
};

NodeManager::NodeManager(QGraphicsItem *parent, QGraphicsScene *scene, int zValue): k(new Private)
{
    k->parent = parent;
    k->scene = scene;
    k->itemCenter = QPointF(0, 0);
    k->isPressed = false;

    k->rotation = k->parent->data(TupGraphicObject::Rotate).toReal();
    k->scaleX = k->parent->data(TupGraphicObject::ScaleX).toReal();
    k->scaleY = k->parent->data(TupGraphicObject::ScaleY).toReal();

    QRectF rect = parent->sceneBoundingRect();
    Node *topLeft = new Node(Node::TopLeft, Node::Scale, rect.topLeft(), this, parent, zValue);
    Node *topRight = new Node(Node::TopRight, Node::Scale, rect.topRight(), this, parent, zValue);
    Node *bottomLeft = new Node(Node::BottomLeft, Node::Scale, rect.bottomLeft(), this, parent, zValue);
    Node *bottomRight = new Node(Node::BottomRight, Node::Scale, rect.bottomRight(), this, parent, zValue);
    Node *center = new Node(Node::Center, Node::Scale, rect.center(), this, parent, zValue);

    k->nodes.insert(Node::TopLeft, topLeft);
    k->nodes.insert(Node::TopRight, topRight);
    k->nodes.insert(Node::BottomLeft, bottomLeft);
    k->nodes.insert(Node::BottomRight, bottomRight);
    k->nodes.insert(Node::Center, center);

    k->proportional = false;
    
    beginToEdit();
}

NodeManager::~NodeManager()
{
}

void NodeManager::clear()
{
    foreach (Node *node, k->nodes) {
             if (node) {
                 QGraphicsScene *scene = node->scene();
                 if (scene)
                     scene->removeItem(node);
             }
             delete node;
             node = 0;
    }
    k->nodes.clear();
}

void NodeManager::syncNodes(const QRectF &rect)
{
    if (k->nodes.isEmpty())
        return;
    
    QHash<Node::TypeNode, Node *>::iterator it = k->nodes.begin();
    while (it != k->nodes.end()) {
           if ((*it)) {
               switch (it.key()) {
                       case Node::TopLeft:
                       {
                            if ((*it)->scenePos() != rect.topLeft())
                                (*it)->setPos(rect.topLeft());
                            break;
                       }
                       case Node::TopRight:
                       {
                            if ((*it)->scenePos() != rect.topRight())
                                (*it)->setPos(rect.topRight());
                            break;
                       }
                       case Node::BottomRight:
                       {
                            if ((*it)->scenePos() != rect.bottomRight())
                                (*it)->setPos(rect.bottomRight());
                            break;
                       }
                       case Node::BottomLeft:
                       {
                            if ((*it)->scenePos() != rect.bottomLeft())
                                (*it)->setPos(rect.bottomLeft() );
                            break;
                       }
                       case Node::Center:
                       {
                            if ((*it)->scenePos() != rect.center())
                                (*it)->setPos(rect.center());
                            break;
                       }
               }
           }
           ++it;
    }
}

void NodeManager::syncNodesFromParent()
{
    if (k->parent)
        syncNodes(k->parent->sceneBoundingRect());
}

QGraphicsItem *NodeManager::parentItem() const
{
    return k->parent;
}

bool NodeManager::isModified() const
{
    return !((k->parent->transform() == k->origTransform) && (k->parent->pos() == k->origPos));
}

void NodeManager::beginToEdit()
{
    k->origTransform = k->parent->transform();
    k->origPos = k->parent->pos();
}

void NodeManager::restoreItem()
{
    k->parent->setTransform(k->origTransform);
    k->parent->setPos(k->origPos);
}

void NodeManager::scale(float sx, float sy)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[NodeManager::scale()]";
        #else
            T_FUNCINFO;
            tWarning() << "Scale X: " << sx;
            tWarning() << "Scale Y: " << sy;
        #endif
    #endif

    QTransform transform;

    QPointF point = k->parent->boundingRect().center();
    transform.translate(point.x(), point.y());
    transform.rotate(k->rotation);
    transform.scale(sx, sy);
    transform.translate(-point.x(), -point.y());

    k->parent->setTransform(transform);

    syncNodesFromParent();
    k->scaleX = sx;
    k->scaleY = sy;
    k->parent->setData(TupGraphicObject::ScaleX, sx);
    k->parent->setData(TupGraphicObject::ScaleY, sy);
}

void NodeManager::rotate(double angle)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[NodeManager::rotate()]";
        #else
            T_FUNCINFO;
            tWarning() << "angle: " << angle;
        #endif
    #endif

    QTransform transform;

    QPointF point = k->parent->boundingRect().center();
    transform.translate(point.x(), point.y());
    transform.rotate(angle);
    transform.scale(k->scaleX, k->scaleY);
    transform.translate(-point.x(), -point.y());

    k->parent->setTransform(transform);

    syncNodesFromParent();
    k->rotation = angle;
    k->parent->setData(TupGraphicObject::Rotate, k->rotation);
}

void NodeManager::show()
{
    foreach (Node *node, k->nodes) {
             if (!node->scene())
                 k->scene->addItem(node);
    }
}

void NodeManager::setPressedStatus(bool isPressed)
{
    k->isPressed = isPressed;
}

bool NodeManager::isPressed()
{
    return k->isPressed;
}

void NodeManager::toggleAction()
{
    foreach (Node *node, k->nodes) {
             if (node->actionNode() == Node::Scale) {
                 node->setAction(Node::Rotate);
             } else if (node->actionNode() == Node::Rotate) {
                        node->setAction(Node::Scale);
             }
    }
}

void NodeManager::setActionNode(Node::ActionNode action)
{
    foreach (Node *node, k->nodes)
             node->setAction(action);
}

void NodeManager::resizeNodes(qreal factor)
{
    foreach (Node *node, k->nodes)
             node->resize(factor);
}

void NodeManager::setVisible(bool visible)
{
    foreach (Node *node, k->nodes)
             node->setVisible(visible);
}

double NodeManager::rotation()
{
    return k->rotation;
}

void NodeManager::setProportion(bool flag)
{
    k->proportional = flag;
}

bool NodeManager::proportionalScale()
{
    return k->proportional;
}

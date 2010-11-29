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

#include "knodegroup.h"
#include "kdebug.h"

#include <QGraphicsPathItem>
#include <QAbstractGraphicsShapeItem>

struct KNodeGroup::Private
{
    QList<KControlNode*> nodes;
    QGraphicsItem *parentItem;
    QPainterPath path;
    QPointF pos;
    QHash<int, QPointF> changedNodes;
    QGraphicsScene *scene;
};

KNodeGroup::KNodeGroup(QGraphicsItem * parent, QGraphicsScene *scene): k(new Private)
{
    KINIT;
    
    k->parentItem = parent;
    k->scene = scene;
    
    if (QGraphicsPathItem *pathItem = qgraphicsitem_cast<QGraphicsPathItem *>(parent))
        createNodes(pathItem);
}

QGraphicsItem * KNodeGroup::parentItem()
{
    return k->parentItem;
}

KNodeGroup::~KNodeGroup()
{
    KEND;
    qDeleteAll(k->nodes);
    k->nodes.clear();
    delete k;
}

void KNodeGroup::clean()
{
    qDeleteAll(k->nodes);
    k->nodes.clear();
}

void KNodeGroup::syncNodes(const QPainterPath & path)
{
    if (k->nodes.isEmpty())
        return;

    foreach (KControlNode *node, k->nodes) {
             if (node) {
                 node->hasChanged(true);
                 node->setPos(path.elementAt(node->index()));
             }
    }
}

void KNodeGroup::syncNodesFromParent()
{
    if (k->parentItem) {
        if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem))
            syncNodes(k->parentItem->sceneMatrix().map(qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path()));
    }
}

void KNodeGroup::setParentItem(QGraphicsItem *newParent)
{
    K_FUNCINFO;
    k->parentItem = newParent;
    foreach (KControlNode *node, k->nodes) {
             if (node)
                 node->setGraphicParent(newParent);
    }
}

void KNodeGroup::moveElementTo(int index, const QPointF& pos)
{
    QPainterPath path = qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path();
    path.setElementPositionAt(index, pos.x(), pos.y());
    QPainterPath::Element e = path.elementAt(0);
    qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->setPath(path);
    
    if (k->changedNodes.contains(index)) {
        (*k->changedNodes.find(index)) = pos;
    } else {
        k->changedNodes.insert(index, pos);
        emit itemChanged(k->parentItem);
    }
}

QHash<int, QPointF> KNodeGroup::changedNodes()
{
    return k->changedNodes;
}

void KNodeGroup::clearChangesNodes()
{
    k->changedNodes.clear();
}

void KNodeGroup::restoreItem()
{
    qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->setPath(k->path);
    k->parentItem->setPos(k->pos);
}

void KNodeGroup::show()
{
    foreach (KControlNode *node, k->nodes) {
             if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)) {
                 if (!node->scene())
                     k->scene->addItem(node);
             }
    }
}

void KNodeGroup::saveParentProperties()
{
    if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)) {
        k->path = qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path();
        k->pos = k->parentItem->scenePos();
    }
}

int KNodeGroup::removeSelectedNodes()
{
    int count = 0;
    foreach (KControlNode *node, k->nodes) {
             if (node->isSelected()) {
                 count++;
                 k->nodes.removeAll(node);
                 // FIXME: recreate the path
             }
    }
    
    return count;
}

void KNodeGroup::createNodes(QGraphicsPathItem *pathItem)
{
    if (pathItem) {

        qDeleteAll(k->nodes);
        k->nodes.clear();
        
        QPainterPath path = pathItem->sceneMatrix().map(pathItem->path());
        saveParentProperties();
        int index = 0;
        
        while (index < path.elementCount()) {
               QPainterPath::Element e = path.elementAt(index);
            
               if (e.type == QPainterPath::CurveToDataElement) {
                   if (index - 2 < 0) 
                       continue;
                   if (path.elementAt(index-2).type == QPainterPath::CurveToElement) {
                       KControlNode *node = new KControlNode(index, this, path.elementAt(index), pathItem, k->scene);
                       QPainterPath::Element e1 = path.elementAt(index-1);
                       node->setLeft(new KControlNode(index-1, this, e1, pathItem, k->scene));
                    
                       if (index+1 < path.elementCount()) {
                           QPainterPath::Element e2 = path.elementAt(index+1);
                           if (e2.type == QPainterPath::CurveToElement) {
                               node->setRight(new KControlNode(index+1, this, e2, pathItem, k->scene));
                               k->nodes << node->right();
                               index++;
                           }
                       }
                       k->nodes << node;
                       k->nodes << node->left();
                   }
               } else if ((e.type == QPainterPath::LineToElement || e.type == QPainterPath::MoveToElement)) {
                          KControlNode *node;
                          if (index+1 < path.elementCount()) {
                    
                              if (path.elementAt(index+1).type == QPainterPath::CurveToElement) {
                                  node = new KControlNode(index, this, path.elementAt(index), pathItem, k->scene);
                                  node->setRight(new KControlNode(index+1, this, path.elementAt(index+1), pathItem, k->scene));
                        
                                  index++;
                                  k->nodes << node;
                                  k->nodes << node->right();
                              } else {
                                  node = new KControlNode(index, this, path.elementAt(index), pathItem, k->scene);
                                  k->nodes << node;
                              }
               } else {
                    node = new KControlNode(index, this, path.elementAt(index), pathItem, k->scene);
                    k->nodes << node;
               }
            }
            index++;
        }
    } else {
        kDebug("selection") << "Item not item path";
    }
}

void KNodeGroup::addControlNode(KControlNode*)
{
    
}

void KNodeGroup::emitNodeClicked()
{
    emit nodeClicked();
}

void KNodeGroup::expandAllNodes()
{
    foreach (KControlNode *node, k->nodes)
             node->showChildNodes(true);
}

bool KNodeGroup::isSelected()
{
    foreach (KControlNode *node, k->nodes) {
             if (node->isSelected())
                 return true;
    }

    return false;
}

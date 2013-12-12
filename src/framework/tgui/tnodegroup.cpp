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

#include "tnodegroup.h"
#include "tdebug.h"

#include <QGraphicsPathItem>
#include <QAbstractGraphicsShapeItem>

struct TNodeGroup::Private
{
    QList<TControlNode*> nodes;
    QGraphicsItem *parentItem;
    QPainterPath path;
    QPointF pos;
    QHash<int, QPointF> changedNodes;
    QGraphicsScene *scene;
    GroupType type;
    int level;
};

TNodeGroup::TNodeGroup(QGraphicsItem *parent, QGraphicsScene *scene, GroupType type, int level): k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->parentItem = parent;
    k->scene = scene;
    k->type = type;
    k->level = level;
    
    if (QGraphicsPathItem *pathItem = qgraphicsitem_cast<QGraphicsPathItem *>(parent))
        createNodes(pathItem);
}

QGraphicsItem *TNodeGroup::parentItem()
{
    return k->parentItem;
}

TNodeGroup::~TNodeGroup()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    // clear();
    delete k;
}

void TNodeGroup::clear()
{
    // SQA: Check if exists a better way to clean nodes 
    qDeleteAll(k->nodes);

    k->parentItem->update();
}

void TNodeGroup::syncNodes(const QPainterPath &path)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->nodes.isEmpty())
        return;

    foreach (TControlNode *node, k->nodes) {
             if (node) {
                 node->hasChanged(true);
                 node->setPos(path.elementAt(node->index()));
             }
    }
}

void TNodeGroup::syncNodesFromParent()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->parentItem) {
        if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem))
            syncNodes(k->parentItem->sceneMatrix().map(qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path()));
    }
}

void TNodeGroup::setParentItem(QGraphicsItem *newParent)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->parentItem = newParent;
    foreach (TControlNode *node, k->nodes) {
             if (node)
                 node->setGraphicParent(newParent);
    }
}

void TNodeGroup::moveElementTo(int index, const QPointF& pos)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QPainterPath path = qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path();
    path.setElementPositionAt(index, pos.x(), pos.y());
    // QPainterPath::Element e = path.elementAt(0);
    qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->setPath(path);
    
    if (k->changedNodes.contains(index)) {
        (*k->changedNodes.find(index)) = pos;
    } else {
        k->changedNodes.insert(index, pos);
        emit itemChanged(k->parentItem);
    }
}

QHash<int, QPointF> TNodeGroup::changedNodes()
{
    return k->changedNodes;
}

void TNodeGroup::clearChangesNodes()
{
    if (!k->changedNodes.isEmpty())
        k->changedNodes.clear();
}

void TNodeGroup::restoreItem()
{
    qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->setPath(k->path);
    k->parentItem->setPos(k->pos);
}

void TNodeGroup::show()
{
    foreach (TControlNode *node, k->nodes) {
             if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)) {
                 if (!node->scene())
                     k->scene->addItem(node);
             }
    }
}

void TNodeGroup::saveParentProperties()
{
    if (qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)) {
        k->path = qgraphicsitem_cast<QGraphicsPathItem *>(k->parentItem)->path();
        k->pos = k->parentItem->scenePos();
    }
}

int TNodeGroup::removeSelectedNodes()
{
    int count = 0;
    foreach (TControlNode *node, k->nodes) {
             if (node->isSelected()) {
                 count++;
                 k->nodes.removeAll(node);
                 // FIXME: recreate the path
             }
    }
    
    return count;
}

void TNodeGroup::createNodes(QGraphicsPathItem *pathItem)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (pathItem) {
        qDeleteAll(k->nodes);
        k->nodes.clear();
        
        QPainterPath path = pathItem->sceneMatrix().map(pathItem->path());
        saveParentProperties();
        int index = 0;

        /*
        int level = k->scene->items().count();
        if (k->type != PositionTween && k->type != CompoundTween)
            level += pathItem->zValue();
        */

        // k->level = k->scene->items().count();
        
        while (index < path.elementCount()) {
               QPainterPath::Element e = path.elementAt(index);
            
               if (e.type == QPainterPath::CurveToDataElement) {
                   if (index - 2 < 0) 
                       continue;
                   if (path.elementAt(index-2).type == QPainterPath::CurveToElement) {
                       TControlNode *node = new TControlNode(index, this, path.elementAt(index), pathItem, k->scene, k->level);
                       QPainterPath::Element e1 = path.elementAt(index-1);
                       node->setLeft(new TControlNode(index-1, this, e1, pathItem, k->scene, k->level));
                    
                       if (index+1 < path.elementCount()) {
                           QPainterPath::Element e2 = path.elementAt(index+1);
                           if (e2.type == QPainterPath::CurveToElement) {
                               node->setRight(new TControlNode(index+1, this, e2, pathItem, k->scene, k->level));
                               k->nodes << node->right();
                               index++;
                           }
                       }
                       k->nodes << node;
                       k->nodes << node->left();
                   }
               } else if ((e.type == QPainterPath::LineToElement || e.type == QPainterPath::MoveToElement)) {
                          TControlNode *node;
                          if (index+1 < path.elementCount()) {
                    
                              if (path.elementAt(index+1).type == QPainterPath::CurveToElement) {
                                  node = new TControlNode(index, this, path.elementAt(index), pathItem, k->scene, k->level);
                                  node->setRight(new TControlNode(index+1, this, path.elementAt(index+1), pathItem, k->scene));
                        
                                  index++;
                                  k->nodes << node;
                                  k->nodes << node->right();
                              } else {
                                  node = new TControlNode(index, this, path.elementAt(index), pathItem, k->scene, k->level);
                                  k->nodes << node;
                              }
               } else {
                    node = new TControlNode(index, this, path.elementAt(index), pathItem, k->scene, k->level);
                    k->nodes << node;
               }
            }
            index++;
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TNodeGroup::createNodes() - Fatal Error: Item is NULL!";
        #endif
    }
}

void TNodeGroup::addControlNode(TControlNode*)
{
}

void TNodeGroup::emitNodeClicked(TControlNode::State state)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    /* SQA: Possible code for the future 
    if (state == TControlNode::Pressed) {
        tFatal() << "TNodeGroup::emitNodeClicked() - Click! -> PRESSED";
        emit nodePressed();
    }
    */

    if (state == TControlNode::Released)
        emit nodeReleased();
}

void TNodeGroup::expandAllNodes()
{
    foreach (TControlNode *node, k->nodes)
             node->showChildNodes(true);
}

bool TNodeGroup::isSelected()
{
    foreach (TControlNode *node, k->nodes) {
             if (node->isSelected())
                 return true;
    }

    return false;
}

int TNodeGroup::size()
{
    return k->nodes.count();
}

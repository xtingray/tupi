/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                          *
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

#ifndef NODEGROUP_H
#define NODEGROUP_H

#include "tglobal.h"
#include "tcontrolnode.h"

#include <QGraphicsScene>
#include <QObject>
#include <QHash>
#include <QGraphicsPathItem>
#include <QAbstractGraphicsShapeItem>

/**
 * @author Jorge Cuadrado
*/

class T_GUI_EXPORT TNodeGroup : public QObject
{
    Q_OBJECT

    public:
        enum GroupType { Polyline, LineSelection, PositionTween, ComposedTween };

        TNodeGroup(QGraphicsItem *parent, QGraphicsScene *scene, GroupType type, int level);
        ~TNodeGroup();

        void clear();
        
        QGraphicsItem *parentItem();
        void syncNodes(const QPainterPath &path);
        void syncNodesFromParent();
        
        void setParentItem(QGraphicsItem *);
        
        void moveElementTo(int index, const QPointF &pos);
        QHash<int, QPointF > changedNodes();
        bool hasChangedNodes();
        void clearChangedNodes();
        void restoreItem();
        void show();

        void saveParentProperties();
        
        int removeSelectedNodes();
        
        void addControlNode(TControlNode* node);
        
        void createNodes(QGraphicsPathItem *nodes);
        void emitNodeClicked(TControlNode::State state);

        void expandAllNodes();
        bool isSelected();
        int size();
        void resizeNodes(qreal scaleFactor);
        
    private:
        struct Private;
        Private *const k;
        
    signals:
        void itemChanged(QGraphicsItem *item);
        void nodePressed();
        void nodeReleased(); 
};

#endif

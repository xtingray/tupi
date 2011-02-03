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

#ifndef NODEGROUP_H
#define NODEGROUP_H

#include "kcontrolnode.h"

#include <QGraphicsScene>
#include <QObject>
#include <QHash>

/**
 * @author Jorge Cuadrado
*/

class KNodeGroup : public QObject
{
    Q_OBJECT

    public:
        KNodeGroup(QGraphicsItem * parent, QGraphicsScene *scene);
        ~KNodeGroup();

        void clean();
        
        QGraphicsItem * parentItem();
        void syncNodes(const QPainterPath & path);
        void syncNodesFromParent();
        
        void setParentItem(QGraphicsItem *);
        
        void moveElementTo(int index, const QPointF& pos);
        QHash<int, QPointF > changedNodes();
        void clearChangesNodes();
        void restoreItem();
        void show();
        void saveParentProperties();
        
        int removeSelectedNodes();
        
        void addControlNode(KControlNode* node);
        
        void createNodes(QGraphicsPathItem *nodes);
        void emitNodeClicked(KControlNode::State state);

        // void emitNodeMoved();
        
        void expandAllNodes();
        bool isSelected();
        
    private:
        struct Private;
        Private *const k;
        
    signals:
        void itemChanged(QGraphicsItem *item);
        void nodePressed();
        void nodeReleased(); 
};

#endif

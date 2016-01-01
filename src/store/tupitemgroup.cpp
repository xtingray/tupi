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

#include "tupitemgroup.h"
#include "tupserializer.h"

struct TupItemGroup::Private
{
    QList<QGraphicsItem *> children;
};

TupItemGroup::TupItemGroup(QGraphicsItem *parent) : QGraphicsItemGroup(parent), k(new Private)
{
}

TupItemGroup::~TupItemGroup()
{
    delete k;
}

void TupItemGroup::addToGroup(QGraphicsItem *item)
{
    k->children << item;
    QGraphicsItemGroup::addToGroup(item);
}

void TupItemGroup::recoverChilds()
{
    int total = k->children.count();
    for(int i=0; i<total; i++) {
        QGraphicsItem *item = k->children.at(i);
        item->setZValue(i);

        if (TupItemGroup *child = qgraphicsitem_cast<TupItemGroup *>(item))
            child->recoverChilds();
        
        if (item->parentItem() != this)
            item->setParentItem(this);
    }
}

QList<QGraphicsItem *> TupItemGroup::childItems()
{
    return k->children;
}

void TupItemGroup::fromXml(const QString &)
{
}

QDomElement TupItemGroup::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("group");
    int total = k->children.count();
    for(int i=0; i<total; i++) {
        QGraphicsItem *item = k->children.at(i);
        root.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(doc));
    }

    QPointF point = this->scenePos();
    QString pos = "(" + QString::number(point.x()) + ", " + QString::number(point.y()) + ")";
    root.setAttribute("pos", pos);

    root.appendChild(TupSerializer::properties(this, doc));
    
    return root;
}

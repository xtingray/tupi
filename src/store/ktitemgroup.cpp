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

#include "ktitemgroup.h"
#include "tdebug.h"

#include "ktserializer.h"

struct KTItemGroup::Private
{
    QList<QGraphicsItem *> childs;
};

KTItemGroup::KTItemGroup(QGraphicsItem *parent , QGraphicsScene *scene) : QGraphicsItemGroup(parent, scene), k(new Private)
{
}

KTItemGroup::~KTItemGroup()
{
    delete k;
}

QVariant KTItemGroup::itemChange ( GraphicsItemChange change, const QVariant & value )
{
    if (change == QGraphicsItem::ItemChildRemovedChange) {
        // k->childs.removeAll( qvariant_cast<QGraphicsItem *>(value) );
    } else if ( change == QGraphicsItem::ItemChildAddedChange ) {
        if (!k->childs.contains(qvariant_cast<QGraphicsItem *>(value))) {
            k->childs << qvariant_cast<QGraphicsItem *>(value);
        }
    }
    
    return QGraphicsItemGroup::itemChange(change, value);
}

void KTItemGroup::recoverChilds()
{
    foreach (QGraphicsItem *item, k->childs) {
        if (KTItemGroup *child = qgraphicsitem_cast<KTItemGroup *>(item)) {
            child->recoverChilds();
        }
        
        if (item->parentItem() != this) {
            item->setParentItem(this);
        }
    }
}

QList<QGraphicsItem *> KTItemGroup::childs()
{
    return k->childs;
}

void KTItemGroup::fromXml(const QString &)
{
}

QDomElement KTItemGroup::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("group");
    
    root.appendChild( KTSerializer::properties( this, doc));
    
    foreach (QGraphicsItem *item, children()) {
             root.appendChild(dynamic_cast<KTAbstractSerializable *>(item)->toXml( doc ));
    }
    
    return root;
}

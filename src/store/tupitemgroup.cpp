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

TupItemGroup::TupItemGroup(QGraphicsItem *parent) : QGraphicsItemGroup(parent)
{
}

TupItemGroup::~TupItemGroup()
{
}

void TupItemGroup::fromXml(const QString &)
{
}

QDomElement TupItemGroup::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("group");
    foreach (QGraphicsItem *item, childItems())
             root.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(doc));

    QPointF point = this->scenePos();
    QString pos = "(" + QString::number(point.x()) + ", " + QString::number(point.y()) + ")";
    root.setAttribute("pos", pos);
    root.appendChild(TupSerializer::properties(this, doc));
    
    return root;
}

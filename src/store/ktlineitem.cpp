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

#include "ktlineitem.h"
#include "ktserializer.h"
#include "tdebug.h"

KTLineItem::KTLineItem(QGraphicsItem * parent, QGraphicsScene * scene) : QGraphicsLineItem(parent, scene)
{
}

KTLineItem::~KTLineItem()
{
}

void KTLineItem::fromXml(const QString &xml)
{
}

QDomElement KTLineItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("line");
    
    root.setAttribute("x1", line().x1());
    root.setAttribute("y1", line().y1());
    root.setAttribute("x2", line().x2());
    root.setAttribute("y2", line().y2());
    
    root.appendChild( KTSerializer::properties( this, doc));
    
    QPen pen = this->pen();
    root.appendChild(KTSerializer::pen(&pen, doc));

    return root;
}

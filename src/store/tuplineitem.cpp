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

#include "tuplineitem.h"
#include "tupserializer.h"
#include "tdebug.h"

TupLineItem::TupLineItem(QGraphicsItem * parent, QGraphicsScene * scene) : QGraphicsLineItem(parent, scene)
{
}

TupLineItem::~TupLineItem()
{
}

void TupLineItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement TupLineItem::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("line");
    
    root.setAttribute("x1", line().x1());
    root.setAttribute("y1", line().y1());
    root.setAttribute("x2", line().x2());
    root.setAttribute("y2", line().y2());
    
    root.appendChild(TupSerializer::properties(this, doc));
    
    QPen pen = this->pen();
    root.appendChild(TupSerializer::pen(&pen, doc));

    return root;
}

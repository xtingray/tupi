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

#include "ktsvgitem.h"
#include "ktserializer.h"

#include <QSvgRenderer>
#include <QFileInfo>

#include "kdebug.h"

KTSvgItem::KTSvgItem(QGraphicsItem * parent)
    : QGraphicsSvgItem(parent)
{
}

KTSvgItem::KTSvgItem(QString &file)
    : QGraphicsSvgItem(file)
{
    path = file;
}

KTSvgItem::~KTSvgItem()
{
}

void KTSvgItem::setSymbolName(const QString &symbolName)
{
    name = symbolName;
}

QString KTSvgItem::symbolName() const
{
    return name;
}

QString KTSvgItem::itemPath() const
{
    return path;
}

void KTSvgItem::rendering()
{
    QByteArray stream = data.toLocal8Bit(); 
    renderer()->load(stream);
}

void KTSvgItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement KTSvgItem::toXml(QDomDocument &doc) const
{
    //QFileInfo svgName(path);

    kFatal() << "KTSvgItem::toXml() - Name: " << name;

    QDomElement root = doc.createElement("svg");
    //root.setAttribute("itemPath", svgName.fileName()); 
    root.setAttribute("id", name);
    root.appendChild(KTSerializer::properties(this, doc));
 
    return root;
}

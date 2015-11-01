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

#include "tupgraphiclibraryitem.h"
#include "tupserializer.h"
#include "tupitemgroup.h"

struct TupGraphicLibraryItem::Private
{
    QString symbolName;
    QString svgContent;
    // QList<QGraphicsItem *> items;
    TupLibraryObject::Type itemType;
};

TupGraphicLibraryItem::TupGraphicLibraryItem() : TupProxyItem(), k(new Private)
{
}

TupGraphicLibraryItem::TupGraphicLibraryItem(TupLibraryObject *object) : TupProxyItem(), k(new Private)
{
    setObject(object);
    k->itemType = object->type();
}

TupGraphicLibraryItem::~TupGraphicLibraryItem()
{
    // qDeleteAll(k->items);
    delete k;
}

TupLibraryObject::Type TupGraphicLibraryItem::itemType()
{
    return k->itemType;
}

QDomElement TupGraphicLibraryItem::toXml(QDomDocument &doc) const
{
    QDomElement library = doc.createElement("symbol");
    library.setAttribute("id", k->symbolName);
    library.appendChild(TupSerializer::properties(this, doc));
    
    return library;
}

void TupGraphicLibraryItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

void TupGraphicLibraryItem::setObject(TupLibraryObject *object)
{
    if (!object) {
        #ifdef K_DEBUG
            QString msg = "TupGraphicLibraryItem::setObject() - Setting null library object";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return;
    }
    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qWarning() << "TupGraphicLibraryItem::setObject() - object->symbolName(): " << object->symbolName();
        #else
            T_FUNCINFOX("library") << object->symbolName();
        #endif
    #endif

    k->symbolName = object->symbolName();
    switch(object->type()) {
        case TupLibraryObject::Item:
        case TupLibraryObject::Text:
        case TupLibraryObject::Image:
        {
             setItem(qvariant_cast<QGraphicsItem *>(object->data()));
        }
        break;
        case TupLibraryObject::Svg:
        {
             setSvgContent(object->dataPath());
        }
        break;
        default: 
        break;
    }
}

void TupGraphicLibraryItem::setSymbolName(const QString &name)
{
    k->symbolName = name;
}

QString TupGraphicLibraryItem::symbolName() const
{
    return k->symbolName;
}

void TupGraphicLibraryItem::setSvgContent(const QString &path)
{
    k->svgContent = path;
}

QString TupGraphicLibraryItem::svgContent()
{
   return k->svgContent;
}

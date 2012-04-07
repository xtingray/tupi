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

#include "tupsvgitem.h"
#include "tupserializer.h"
#include "tdebug.h"

#include <QSvgRenderer>
#include <QFileInfo>

struct TupSvgItem::Private
{
    QString name;
    QString path;
    QString data;
    TupFrame *frame;
    TupItemTweener *tween;
    bool hasTween;
    QPointF lastTweenPos;
};

TupSvgItem::TupSvgItem(QGraphicsItem * parent)
    : QGraphicsSvgItem(parent), k(new Private)
{
}

TupSvgItem::TupSvgItem(QString &file, TupFrame *frame)
    : QGraphicsSvgItem(file), k(new Private)
{
    k->path = file;
    k->frame = frame;
    k->hasTween = false;
}

TupSvgItem::~TupSvgItem()
{
}

void TupSvgItem::setSymbolName(const QString &symbolName)
{
    k->name = symbolName;
}

QString TupSvgItem::symbolName() const
{
    return k->name;
}

QString TupSvgItem::itemPath() const
{
    return k->path;
}

TupFrame *TupSvgItem::frame() const
{
    return k->frame;
}

TupItemTweener *TupSvgItem::tween() const
{
    return k->tween;
}

void TupSvgItem::rendering()
{
    QByteArray stream = k->data.toLocal8Bit(); 
    renderer()->load(stream);
}

void TupSvgItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement TupSvgItem::toXml(QDomDocument &doc) const
{
    if (k->name.length() == 0) {
        #ifdef K_DEBUG
               tError() << "TupFrame::fromXml() - ERROR: Object id is null!";
        #endif
    }

    QDomElement root = doc.createElement("svg");
    root.setAttribute("id", k->name);
    root.appendChild(TupSerializer::properties(this, doc));

    if (k->hasTween)
        root.appendChild(k->tween->toXml(doc));

    return root;
}

void TupSvgItem::setTween(TupItemTweener *tween)
{
    k->tween = tween;
    k->hasTween = true;

    /*
    if (!update) {
        if (k->tween)
            k->frame->scene()->addTweenObject(this);
        else
            k->frame->scene()->removeTweenObject(this);
    }
    */
}

bool TupSvgItem::hasTween()
{
    return k->hasTween;
}

void TupSvgItem::removeTween()
{
    k->tween = 0; 
    k->hasTween = false;
}

void TupSvgItem::setLastTweenPos(QPointF point)
{
    k->lastTweenPos = point;
}

QPointF TupSvgItem::lastTweenPos()
{
    return k->lastTweenPos;
}


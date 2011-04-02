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
#include "kdebug.h"

#include <QSvgRenderer>
#include <QFileInfo>

struct KTSvgItem::Private
{
    QString name;
    QString path;
    QString data;
    KTFrame *frame;
    KTItemTweener *tween;
    bool hasTween;
    QPointF lastTweenPos;
};

KTSvgItem::KTSvgItem(QGraphicsItem * parent)
    : QGraphicsSvgItem(parent), k(new Private)
{
}

KTSvgItem::KTSvgItem(QString &file, KTFrame *frame)
    : QGraphicsSvgItem(file), k(new Private)
{
    k->path = file;
    k->frame = frame;
    k->hasTween = false;
}

KTSvgItem::~KTSvgItem()
{
}

void KTSvgItem::setSymbolName(const QString &symbolName)
{
    k->name = symbolName;
}

QString KTSvgItem::symbolName() const
{
    return k->name;
}

QString KTSvgItem::itemPath() const
{
    return k->path;
}

KTFrame *KTSvgItem::frame() const
{
    return k->frame;
}

KTItemTweener *KTSvgItem::tween() const
{
    return k->tween;
}

void KTSvgItem::rendering()
{
    QByteArray stream = k->data.toLocal8Bit(); 
    renderer()->load(stream);
}

void KTSvgItem::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement KTSvgItem::toXml(QDomDocument &doc) const
{
    kFatal() << "KTSvgItem::toXml() - Name: " << k->name;

    if (k->name.length() == 0) {
        #ifdef K_DEBUG
               kError() << "KTFrame::fromXml() - ERROR: Object id is null!";
        #endif
    }

    QDomElement root = doc.createElement("svg");
    root.setAttribute("id", k->name);
    root.appendChild(KTSerializer::properties(this, doc));

    if (k->tween)
        root.appendChild(k->tween->toXml(doc));
 
    return root;
}

void KTSvgItem::setTween(KTItemTweener *tween)
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

bool KTSvgItem::hasTween()
{
    return k->hasTween;
}

void KTSvgItem::removeTween()
{
    k->tween = 0; 
    k->hasTween = false;
}

void KTSvgItem::setLastTweenPos(QPointF point)
{
    k->lastTweenPos = point;
}

QPointF KTSvgItem::lastTweenPos()
{
    return k->lastTweenPos;
}


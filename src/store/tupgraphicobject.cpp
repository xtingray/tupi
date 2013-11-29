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

#include "tupgraphicobject.h"
#include "tupframe.h"
#include "tupscene.h"
#include "tupitemtweener.h"

#include <QMatrix>
#include <QGraphicsItem>

#include "tdebug.h"

struct TupGraphicObject::Private
{
    QString name;
    QGraphicsItem *item;
    bool hasTween;
    TupItemTweener *tween;
    TupFrame *frame;
    QPointF lastTweenPos;
};

TupGraphicObject::TupGraphicObject(QGraphicsItem *item, TupFrame *parent) : QObject(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->item = item;
    k->tween = 0;
    k->frame = parent;
    k->hasTween = false;

    initItemData();
}

TupGraphicObject::~TupGraphicObject()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    if (k->item)
        delete k->item;

    /*
    if (k->tween && k->frame->scene()) {
        k->frame->scene()->removeTweenObject(this);
    }
    */

    delete k;
}

void TupGraphicObject::fromXml(const QString &xml)
{
}

QDomElement TupGraphicObject::toXml(QDomDocument &doc) const
{
    QDomElement object = doc.createElement("object");

    if (TupAbstractSerializable *is = dynamic_cast<TupAbstractSerializable *>(k->item))
        object.appendChild(is->toXml(doc));

    if (k->tween)
        object.appendChild(k->tween->toXml(doc));

    return object;
}

void TupGraphicObject::setItem(QGraphicsItem *item)
{
    if (item) {
        k->item = item;
        initItemData();
    } else {
        #ifdef K_DEBUG
               tError() << "TupGraphicObject::setItem() - Fatal Error: QGraphicsItem is null!";
        #endif
    } 
}

QGraphicsItem *TupGraphicObject::item() const
{
    return k->item;
}

void TupGraphicObject::setObjectName(const QString &name)
{
    k->name = name;
}

QString TupGraphicObject::objectName() const
{
    return k->name;
}

void TupGraphicObject::initItemData()
{
    if (! k->item->data(ScaleX).isValid())
        k->item->setData(ScaleX, 1.0);

    if (! k->item->data(ScaleY).isValid())
        k->item->setData(ScaleY, 1.0);

    if (! k->item->data(Rotate).isValid())
        k->item->setData(Rotate, 0.0);

    if (! k->item->data(TranslateX).isValid())
        k->item->setData(TranslateX, 0.0);

    if (! k->item->data(TranslateY).isValid())
        k->item->setData(TranslateY, 0.0);
}

void TupGraphicObject::setTween(TupItemTweener *tween)
{
    k->tween = tween;
    k->hasTween = true;
}

bool TupGraphicObject::hasTween() const
{
    return k->hasTween;
}

void TupGraphicObject::removeTween()
{
    k->tween = 0;
    k->hasTween = false;
}

TupItemTweener *TupGraphicObject::tween() const
{
    return k->tween;
}

TupFrame *TupGraphicObject::frame() const
{
    return k->frame;
}

void TupGraphicObject::setFrame(TupFrame *frame) 
{
    k->frame = frame;
}

int TupGraphicObject::objectIndex() const
{
    return k->frame->indexOf(const_cast<TupGraphicObject *>(this));
}

void TupGraphicObject::setLastTweenPos(QPointF point)
{
    k->lastTweenPos = point;
}

QPointF TupGraphicObject::lastTweenPos()
{   
    return k->lastTweenPos;
}

void TupGraphicObject::setItemZValue(int value)
{
    k->item->setZValue(value);
}

int TupGraphicObject::itemZValue()
{
    return k->item->zValue();
}

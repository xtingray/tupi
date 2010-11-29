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

#include "ktgraphicobject.h"
#include "ktframe.h"
#include "ktscene.h"
#include "ktitemtweener.h"

#include <QMatrix>
#include <QGraphicsItem>

#include "kdebug.h"

struct KTGraphicObject::Private
{
    QString name;
    QGraphicsItem *item;
    KTItemTweener *tweener;
    KTFrame *frame;
};

KTGraphicObject::KTGraphicObject(QGraphicsItem *item, KTFrame *parent)
	: QObject(parent), k(new Private)
{
    #ifdef K_DEBUG
           KINIT;
    #endif

    k->item = item;
    k->tweener = 0;
    k->frame = parent;

    initItemData();
}

KTGraphicObject::~KTGraphicObject()
{
    #ifdef K_DEBUG
           KEND;
    #endif

    if (k->item)
        delete k->item;

    /*
    if (k->tweener && k->frame->scene()) {
        k->frame->scene()->removeTweenObject(this);
    }
    */

    delete k;
}

void KTGraphicObject::fromXml(const QString &xml)
{
}

QDomElement KTGraphicObject::toXml(QDomDocument &doc) const
{
    QDomElement object = doc.createElement("object");
 
    if (KTAbstractSerializable *is = dynamic_cast<KTAbstractSerializable *>(k->item))
        object.appendChild(is->toXml(doc));

    if (k->tweener)
        object.appendChild(k->tweener->toXml(doc));

    return object;
}

void KTGraphicObject::setItem(QGraphicsItem *item)
{
    if (item) {
        k->item = item;
        initItemData();
    } 
}

QGraphicsItem *KTGraphicObject::item() const
{
    return k->item;
}

void KTGraphicObject::setObjectName(const QString &name)
{
    k->name = name;
}

QString KTGraphicObject::objectName() const
{
    return k->name;
}

void KTGraphicObject::initItemData()
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

void KTGraphicObject::setTweener(bool update, KTItemTweener *tweener)
{
    k->tweener = tweener;

    if (!update) {
        if (k->tweener)
            k->frame->scene()->addTweenObject(this);
        else
            k->frame->scene()->removeTweenObject(this);
    }
}

KTItemTweener *KTGraphicObject::tweener() const
{
    return k->tweener;
}

KTFrame *KTGraphicObject::frame() const
{
    return k->frame;
}

/*
int KTGraphicObject::logicalIndex() const
{
    return k->frame->logicalIndexOf(const_cast<KTGraphicObject *>(this));
}
*/

int KTGraphicObject::objectIndex() const
{
    return k->frame->indexOf(const_cast<KTGraphicObject *>(this));
}

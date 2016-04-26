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
#include "tuplayer.h"
#include "tupscene.h"
#include "tupitemtweener.h"
#include "tupserializer.h"

struct TupGraphicObject::Private
{
    QString name;
    QGraphicsItem *item;
    bool hasTween;
    TupItemTweener *tween;
    TupFrame *frame;
    QPointF lastTweenPos;

    QStringList transformDoList;
    QStringList transformUndoList;

    QStringList brushDoList;
    QStringList brushUndoList;

    QStringList penDoList;
    QStringList penUndoList;
};

TupGraphicObject::TupGraphicObject(QGraphicsItem *item, TupFrame *parent) : QObject(parent), k(new Private)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicObject()]";
        #else
            TINIT;
        #endif
    #endif	
    */

    k->item = item;
    k->tween = 0;
    k->frame = parent;
    k->hasTween = false;

    initItemData();
}

TupGraphicObject::~TupGraphicObject()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupGraphicObject()]";
        #else
            TEND;
        #endif
    #endif	
    */

    delete k;
}

void TupGraphicObject::fromXml(const QString &xml)
{
    Q_UNUSED(xml);
}

QDomElement TupGraphicObject::toXml(QDomDocument &doc) const
{
    QDomElement object = doc.createElement("object");

    if (TupAbstractSerializable *serialData = dynamic_cast<TupAbstractSerializable *>(k->item))
        object.appendChild(serialData->toXml(doc));

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
            QString msg = "TupGraphicObject::setItem() - Fatal Error: QGraphicsItem is null!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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

int TupGraphicObject::frameIndex()
{
    return k->frame->index();
}

bool TupGraphicObject::layerIsVisible()
{
    TupLayer *layer = k->frame->layer();
    if (layer->isVisible())
        return true;

    return false;
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

bool TupGraphicObject::transformationIsNotEdited()
{
    return k->transformDoList.isEmpty() && k->transformUndoList.isEmpty();
}

void TupGraphicObject::saveInitTransformation()
{
    QDomDocument doc;
    doc.appendChild(TupSerializer::properties(k->item, doc));
    k->transformDoList << doc.toString();
}

void TupGraphicObject::storeItemTransformation(const QString &properties)
{
    k->transformDoList << properties;
}

void TupGraphicObject::undoTransformation()
{
    if (k->transformDoList.count() > 1) {
        k->transformUndoList << k->transformDoList.takeLast();
        if (!k->transformDoList.isEmpty()) {
            QString properties = k->transformDoList.last();
            QDomDocument doc;
            doc.setContent(properties);
            TupSerializer::loadProperties(k->item, doc.documentElement());
        }
    }
}

void TupGraphicObject::redoTransformation()
{
    if (!k->transformUndoList.isEmpty()) {
        QString properties = k->transformUndoList.takeLast();
        k->transformDoList << properties;
        QDomDocument doc;
        doc.setContent(properties);
        TupSerializer::loadProperties(k->item, doc.documentElement());
    }
}

bool TupGraphicObject::brushIsNotEdited()
{
    return k->brushDoList.isEmpty() && k->brushUndoList.isEmpty();
}

void TupGraphicObject::saveInitBrush()
{
    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
        QBrush brush = shape->brush();
        QDomDocument doc; 
        doc.appendChild(TupSerializer::brush(&brush, doc));
        k->brushDoList << doc.toString();
    }
}

void TupGraphicObject::setBrush(const QString &xml)
{
    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
        k->brushDoList << xml;
        QBrush brush;
        QDomDocument doc;
        doc.setContent(xml);

        TupSerializer::loadBrush(brush, doc.documentElement());
        shape->setBrush(brush);
    }
}

void TupGraphicObject::redoBrushAction()
{
    if (!k->brushUndoList.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
            QString xml = k->brushUndoList.takeLast();
            k->brushDoList << xml;
            QBrush brush;
            QDomDocument doc;
            doc.setContent(xml);

            TupSerializer::loadBrush(brush, doc.documentElement());
            shape->setBrush(brush);
        }
    }
}

void TupGraphicObject::undoBrushAction()
{
    if (k->brushDoList.count() > 1) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
            k->brushUndoList << k->brushDoList.takeLast();
            if (!k->brushDoList.isEmpty()) {
                QString xml = k->brushDoList.last();
                QBrush brush;
                QDomDocument doc;
                doc.setContent(xml);

                TupSerializer::loadBrush(brush, doc.documentElement());
                shape->setBrush(brush);
            }
        }
    }
}

bool TupGraphicObject::penIsNotEdited()
{
    return k->penDoList.isEmpty() && k->penUndoList.isEmpty();
}

void TupGraphicObject::saveInitPen()
{
    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
        QPen pen = shape->pen();
        QDomDocument doc;
        doc.appendChild(TupSerializer::pen(&pen, doc));
        k->penDoList << doc.toString();
    }
}

void TupGraphicObject::setPen(const QString &xml)
{
    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
        QPen pen;
        QDomDocument doc;
        doc.setContent(xml);

        TupSerializer::loadPen(pen, doc.documentElement());
        shape->setPen(pen);
        k->penDoList << xml;
    }
}

void TupGraphicObject::redoPenAction()
{
    if (!k->penUndoList.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
            QString xml = k->penUndoList.takeLast();
            k->penDoList << xml;
            QPen pen;
            QDomDocument doc;
            doc.setContent(xml);

            TupSerializer::loadPen(pen, doc.documentElement());
            shape->setPen(pen);
        }
    }
}

void TupGraphicObject::undoPenAction()
{
    if (k->penDoList.count() > 1) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->item)) {
            k->penUndoList << k->penDoList.takeLast();
            if (!k->penDoList.isEmpty()) {
                QString xml = k->penDoList.last();
                QPen pen;
                QDomDocument doc;
                doc.setContent(xml);

                TupSerializer::loadPen(pen, doc.documentElement());
                shape->setPen(pen);
            }
        }
    }
}

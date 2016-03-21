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

#include "tupsvgitem.h"
#include "tupserializer.h"
#include "tuplayer.h"

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

TupSvgItem::TupSvgItem(QGraphicsItem * parent) : QGraphicsSvgItem(parent), k(new Private)
{
    setAcceptHoverEvents(true);
}

TupSvgItem::TupSvgItem(const QString &file, TupFrame *frame) : QGraphicsSvgItem(file), k(new Private)
{
    setAcceptHoverEvents(true);
    k->path = file;
    k->frame = frame;
    k->hasTween = false;
}

TupSvgItem::~TupSvgItem()
{
    setAcceptHoverEvents(false);
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

int TupSvgItem::frameIndex()
{
    return k->frame->index();
}

bool TupSvgItem::layerIsVisible()
{
    TupLayer *layer = k->frame->layer();
    if (layer->isVisible())
        return true;

    return false;
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
            QString msg = "TupFrame::fromXml() - Error: Object ID is null!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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

void TupSvgItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    /*
    QRectF rect = sceneBoundingRect();
    int x = event->pos().x();
    int y = event->pos().y();
    
    int xmin = 0; 
    int xmax = rect.width();
    int ymin = 0;
    int ymax = rect.height();

    if (rect.width() >= 100) {
        xmin = (rect.width() - 100)/2;
        xmax = xmin + 100;
        ymin = (rect.height() - 100)/2;
        ymax = ymin + 100;
    }

    if (x >= xmin && x <= xmax) {
        if (y >= ymin && y <= ymax) {
            emit enabledChanged();
        }
    }
    */

    emit enabledChanged();
    QGraphicsSvgItem::hoverEnterEvent(event);
}

void TupSvgItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsSvgItem::hoverLeaveEvent(event);
}

/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupruler.h" 
#include "tdebug.h"

#include <cmath>

struct TupRuler::Private
{
    Qt::Orientation rulerType;
    qreal origin;
    qreal oldPos;
    qreal rulerZoom;
    qreal currentZoomFactor;
    QPointF cursorPos;
    QPolygonF arrow;
    Transformation currentTransformation;
};

TupRuler::TupRuler(Qt::Orientation rulerType, QWidget *parent) : QWidget(parent), k(new Private)
{
    k->currentTransformation = None;
    k->rulerType = rulerType;
    k->origin = 0.0;
    k->oldPos = 0.0;
    k->rulerZoom = 1.0;
    k->currentZoomFactor = 1;

    k->arrow = QPolygonF(3);

    if (rulerType == Qt::Horizontal) {
        setMaximumHeight(20);
        setMinimumHeight(20);

        k->arrow << QPointF(0.0, 0.0);
        k->arrow << QPointF(5.0, 5.0);
        k->arrow << QPointF(10.0, 0.0);

        k->arrow.translate(0, 14);
    } else {
        setMaximumWidth(20);
        setMinimumWidth(20);

        k->arrow << QPointF(0.0, 0.0);
        k->arrow << QPointF(5.0, 5.0);
        k->arrow << QPointF(0.0, 10.0);

        k->arrow.translate(14, 0);
    }

    QFont ruleFont(font().family(), 7);
    setFont(ruleFont);
}

TupRuler::~TupRuler()
{
}

QSize TupRuler::minimumSizeHint() const
{
    return QSize(RULER_BREADTH, RULER_BREADTH);
}

Qt::Orientation TupRuler::rulerType() const
{
    return k->rulerType;
}

qreal TupRuler::origin() const
{
    return k->origin;
}

qreal TupRuler::rulerZoom() const
{
    return k->rulerZoom;
}

void TupRuler::setOrigin(const qreal origin)
{
    if (k->origin != origin) {
        k->origin = origin;
        update();
    }
}

void TupRuler::setRulerZoom(const qreal rulerZoom)
{
    k->currentZoomFactor *= rulerZoom;
    update();
}

void TupRuler::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (k->currentTransformation == None) {
        bool isHorizontal = Qt::Horizontal == k->rulerType;
        QPainter painter(this);
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, true);

        QRectF rulerRect = this->rect();
        QPointF starPt = isHorizontal ? rulerRect.bottomLeft() : rulerRect.topRight();
        QPointF endPt = rulerRect.bottomRight();

        qreal rulerStartMark = isHorizontal ? rulerRect.left() : rulerRect.top();
        qreal rulerEndMark = isHorizontal ? rulerRect.right() : rulerRect.bottom();
        if (k->origin < rulerStartMark || k->origin > rulerEndMark) {
            if (Qt::Horizontal == k->rulerType)
                endPt -= QPointF(18, 0);
            else
                endPt -= QPointF(0, 18);
        }

        painter.drawLine(starPt, endPt);
        drawAScaleMeter(&painter, rulerRect);
        painter.drawConvexPolygon(k->arrow);

        painter.restore();
    }
}

void TupRuler::drawAScaleMeter(QPainter *painter, QRectF rulerRect)
{
    qreal scaleMeter = 10 * k->currentZoomFactor;

    bool isHorizontal = Qt::Horizontal == k->rulerType;
    qreal rulerStartMark = isHorizontal ? rulerRect.left() : rulerRect.top();
    qreal rulerEndMark = isHorizontal ? rulerRect.right() : rulerRect.bottom();

    if (scaleMeter > 3) {
        if (k->origin >= rulerStartMark && k->origin <= rulerEndMark) {        
            drawFromOriginTo(painter, rulerRect, k->origin, rulerEndMark, scaleMeter);
            drawFromOriginTo(painter, rulerRect, k->origin, rulerStartMark, -scaleMeter);
        } else if (k->origin < rulerStartMark) {
                   drawMaximizedRuler(painter, rulerRect, rulerStartMark, rulerEndMark, scaleMeter);
        } else if (k->origin > rulerEndMark) {
                   drawMaximizedRuler(painter, rulerRect, rulerStartMark, rulerEndMark, -scaleMeter);
        }
    } else {
        drawSimpleRuler(painter, rulerRect, k->origin, rulerEndMark, scaleMeter*5);
        drawSimpleRuler(painter, rulerRect, k->origin, rulerStartMark, -scaleMeter*5); 
    }
}

void TupRuler::drawFromOriginTo(QPainter* painter, QRectF rulerRect, qreal startMark, qreal endMark, qreal step)
{
    bool isHorizontal = Qt::Horizontal == k->rulerType;
    int counter = 0;
    qreal startPosition = 16;

    for (qreal current = startMark + 5; (step < 0 ? current >= endMark : current <= endMark); current += step) {
         startPosition = 16;
         if (counter % 5 == 0)
             startPosition = 12;
         if (counter % 10 == 0) {
             startPosition = (isHorizontal ? 9 : 0);
             if (step < 0 && current == startMark + 5) {
                 counter++;
                 continue;
             }
         }

         qreal x1 = isHorizontal ? current : rulerRect.left() + startPosition;
         qreal y1 = isHorizontal ? rulerRect.top() + startPosition : current;
         qreal x2 = isHorizontal ? current : rulerRect.right();
         qreal y2 = isHorizontal ? rulerRect.bottom() : current;

         painter->drawLine(QLineF(x1, y1, x2, y2));

         if (counter % 10 == 0) {  
             int tag = counter*10;
             if (step < 0)
                 tag *= -1;
             painter->drawText(QPoint(x1 + (isHorizontal ? 3 : 0), y1 + (isHorizontal ? 3 : -2)), QString::number(tag));
         }

         counter++;
    }
}

void TupRuler::drawMaximizedRuler(QPainter* painter, QRectF rulerRect, qreal startMark, qreal endMark, qreal step)
{
    endMark -= 20;
    int delta = (int) fabs(k->origin) % (int) step;
    bool isHorizontal = Qt::Horizontal == k->rulerType;
    qreal startPosition = 16;

    int counter = fabs(k->origin)/step;

    for (qreal current = startMark + delta; current <= endMark; current += step) {
         startPosition = 16;
         if (counter % 5 == 0)
             startPosition = 12;
         if (counter % 10 == 0)
             startPosition = (isHorizontal ? 9 : 0);

         qreal x1 = isHorizontal ? current : rulerRect.left() + startPosition;
         qreal y1 = isHorizontal ? rulerRect.top() + startPosition : current;
         qreal x2 = isHorizontal ? current : rulerRect.right();
         qreal y2 = isHorizontal ? rulerRect.bottom() : current;

         painter->drawLine(QLineF(x1, y1, x2, y2));

         if (counter % 10 == 0) {
             int tag = counter*10;
             if (step < 0)
                 tag *= -1;
             painter->drawText(QPoint(x1 + (isHorizontal ? 3 : 0), y1 + (isHorizontal ? 3 : -2)), QString::number(tag));
         }

         counter++;
    }
}

void TupRuler::drawSimpleRuler(QPainter* painter, QRectF rulerRect, qreal startMark, qreal endMark, qreal step)
{
    bool isHorizontal = Qt::Horizontal == k->rulerType;
    int counter = 0;
    qreal startPosition = 16;

    for (qreal current = startMark + 5; (step < 0 ? current >= endMark : current <= endMark); current += step) {
         if (counter % 5 == 0)
             startPosition = 16;
         if (counter % 10 == 0) {
             startPosition = 12;
             if (step < 0 && current == startMark + 5) {
                 counter += 5;
                 continue;
             }
         }

         qreal x1 = isHorizontal ? current : rulerRect.left() + startPosition;
         qreal y1 = isHorizontal ? rulerRect.top() + startPosition : current;
         qreal x2 = isHorizontal ? current : rulerRect.right();
         qreal y2 = isHorizontal ? rulerRect.bottom() : current;

         painter->drawLine(QLineF(x1, y1, x2, y2));

         counter += 5;
    }
}

void TupRuler::translateArrow(double dx, double dy)
{
    k->arrow.translate(dx, dy);
}

void TupRuler::updateCurrentTransformation(Transformation current)
{
    k->currentTransformation = current;
}

void TupRuler::movePointers(const QPointF pos)
{
    if (k->currentTransformation == None) {
        if (k->rulerType == Qt::Horizontal) {
            qreal x = pos.x() * k->currentZoomFactor;
            translateArrow(-k->oldPos, 0);
            translateArrow(k->origin + x, 0);
            k->oldPos = k->origin + x;
        } else {
            qreal y = pos.y() * k->currentZoomFactor;
            translateArrow(0, -k->oldPos);
            translateArrow(0, k->origin + y);
            k->oldPos = k->origin + y;
        }
    }

    update();
}

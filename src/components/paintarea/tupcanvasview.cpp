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

#include "tupcanvasview.h"

struct TupCanvasView::Private
{
    QSize screenSize;
    QSize projectSize;
    QColor bg;
};

TupCanvasView::TupCanvasView(QWidget *parent, const QSize &screenSize, const QSize &projectSize, 
                             const QColor &bg) : QGraphicsView(parent), k(new Private)
{
    setAccessibleName("FULL_SCREEN");
    k->screenSize = screenSize;
    k->projectSize = projectSize;
    k->bg = bg;

    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setBackgroundBrush(QBrush(k->bg, Qt::SolidPattern));
}

TupCanvasView::~TupCanvasView()
{
}

void TupCanvasView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(QPen(Qt::NoPen));

    // QRectF drawingRect = QRectF(rect.topLeft(), k->screenSize);
    // painter->fillRect(drawingRect, k->bg);

    double w = (double) k->projectSize.width() / (double) 2;
    double h = (double) k->projectSize.height() / (double) 2;

    painter->setPen(QPen(QColor(230, 230, 230, 255), 6));
    QPointF topLeft = rect.center() - QPointF(w, h);
    QPointF bottomRight = rect.center() + QPointF(w, h);
    // QRectF workspace = QRectF(topLeft, k->projectSize);

    painter->drawLine(topLeft - QPoint(0, 30), topLeft + QPoint(0, 30));  
    painter->drawLine(topLeft - QPoint(30, 0), topLeft + QPoint(30, 0)); 

    painter->drawLine(bottomRight - QPoint(0, 30), bottomRight + QPoint(0, 30));          
    painter->drawLine(bottomRight - QPoint(30, 0), bottomRight + QPoint(30, 0));  

    // painter->drawRect(drawingRect);
    // painter->drawRect(workspace);
    painter->restore();
}

void TupCanvasView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        emit rightClick();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

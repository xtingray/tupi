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

#include "tupcanvasview.h"
#include "tdebug.h"

struct TupCanvasView::Private
{
    QSize screenSize;
    QSize projectSize;
    QColor bg;
};

TupCanvasView::TupCanvasView(QWidget *parent, const QSize &screenSize, const QSize &projectSize, 
                             const QColor &bg) : QGraphicsView(parent), k(new Private)
{
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
}

bool TupCanvasView::event(QEvent *event)
{
     if (event->type() == QEvent::ToolTip) {
         tError() << "Tracing 2!";
         return true;
     }

     return QWidget::event(event);
}


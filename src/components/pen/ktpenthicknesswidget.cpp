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

#include "ktpenthicknesswidget.h"
#include "tdebug.h"

struct KTPenThicknessWidget::Private
{
    int thickness;
    int brush;
    QColor color;
};

KTPenThicknessWidget::KTPenThicknessWidget(QWidget *parent) : QWidget(parent), k(new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

KTPenThicknessWidget::~KTPenThicknessWidget()
{
}

void KTPenThicknessWidget::render(int thickness)
{
    k->thickness = thickness;
    update();
}

void KTPenThicknessWidget::setColor(const QColor color)
{
    k->color = color;
}

void KTPenThicknessWidget::setBrush(int index)
{
    k->brush = index;
    update();
}

QSize KTPenThicknessWidget::minimumSizeHint() const
{
    return QSize(100, 106);
}

QSize KTPenThicknessWidget::sizeHint() const
{
    return QSize(100, 106);
}

void KTPenThicknessWidget::paintEvent(QPaintEvent *)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing, true);
     painter.fillRect(0, 0, width(), height(), QColor(255, 255, 255));

     QPen border(QColor(0, 0, 0));
     border.setWidth(0.5);
     painter.setPen(border);
     painter.drawRect(0, 0, width(), height());

     painter.translate(width() / 2, height() / 2);

     QBrush brush;
     Qt::BrushStyle style = Qt::BrushStyle(k->brush);
     
     if (style != Qt::TexturePattern) {  
         brush = QBrush(Qt::BrushStyle(k->brush));
         brush.setColor(k->color);
         QPen pen(Qt::NoPen);
         painter.setPen(pen);
         painter.setBrush(brush);
         painter.drawEllipse(-(k->thickness/2), -(k->thickness/2), k->thickness, k->thickness);
     } else {
         QPixmap pixmap(THEME_DIR + "icons/brush_15.png");
         painter.drawPixmap(-(pixmap.width()/2), -(pixmap.height()/2), pixmap);  
     }
}

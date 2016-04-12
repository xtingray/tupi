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

#include "tuppenthicknesswidget.h"

struct TupPenThicknessWidget::Private
{
    int thickness;
    double opacity;
    int brush;
    QColor color;
    QBrush currentBrush;
};

TupPenThicknessWidget::TupPenThicknessWidget(QWidget *parent) : QWidget(parent), k(new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    k->opacity = 1.0;
    k->thickness = 100;
}

TupPenThicknessWidget::~TupPenThicknessWidget()
{
}

void TupPenThicknessWidget::render(int thickness)
{
    k->thickness = thickness;
    update();
}

void TupPenThicknessWidget::render(double opacity)
{
    k->opacity = opacity;
    update();
}

void TupPenThicknessWidget::setColor(const QColor color)
{
    k->color = color;
    update();
}

void TupPenThicknessWidget::setBrush(int index)
{
    k->brush = index;
    update();
}

void TupPenThicknessWidget::setBrush(const QBrush brush)
{
    k->currentBrush = brush;
    k->brush = -1;
}

QSize TupPenThicknessWidget::minimumSizeHint() const
{
    return QSize(100, 106);
}

QSize TupPenThicknessWidget::sizeHint() const
{
    return QSize(100, 106);
}

void TupPenThicknessWidget::paintEvent(QPaintEvent *)
{
     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing, true);
     painter.fillRect(0, 0, width(), height(), QColor(255, 255, 255));

     QPen border(QColor(0, 0, 0));
     border.setWidth(1);
     painter.setPen(border);
     painter.drawRect(0, 0, width(), height());

     painter.translate(width() / 2, height() / 2);

     QBrush brush;
     Qt::BrushStyle style = Qt::BrushStyle(k->brush);
     
     if (style != Qt::TexturePattern) {  
         if (k->brush != -1) {
             // tFatal() << "TupPenThicknessWidget::paintEvent() - Setting pre-def brush";
             brush = QBrush(Qt::BrushStyle(k->brush));
             brush.setColor(k->color);
         } else {
             if (k->currentBrush.gradient()) {
                 // tFatal() << "TupPenThicknessWidget::paintEvent() - Setting gradient brush";
                 brush = k->currentBrush;
             } else {
                #ifdef K_DEBUG
                    QString msg = "TupPenThicknessWidget::paintEvent() - Warning! NO gradient!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
             }
         }
         QPen pen(Qt::NoPen);
         painter.setPen(pen);
         painter.setBrush(brush);
         painter.setOpacity(k->opacity);
         painter.drawEllipse(-(k->thickness/2), -(k->thickness/2), k->thickness, k->thickness);
     } else {
         QPixmap pixmap(THEME_DIR + "icons/brush_15.png");
         painter.drawPixmap(-(pixmap.width()/2), -(pixmap.height()/2), pixmap);  
     }
}

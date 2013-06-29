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

#include "tupcolorpicker.h"
#include "tdebug.h"

#include <QPoint>
#include <QColor>
#include <QImage>
#include <QSizePolicy>
#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QMouseEvent>

static int pWidth = 100;
static int pHeight = 80;

struct TupColorPicker::Private
{
    int hue;
    int saturation;
    QPixmap *pix;
};

TupColorPicker::TupColorPicker(QWidget* parent) : QFrame(parent), k( new Private)
{
    k->hue = 0;
    k->saturation = 0;
    setColor(150, 255);

    QImage img(pWidth, pHeight, QImage::Format_RGB32);
    int x,y;
    for (y = 0; y < pHeight; y++) {
         for (x = 0; x < pWidth; x++) {
              QPoint p(x, y);
              QColor c;
              c.setHsv(huePoint(p), saturationPoint(p), 200);
              img.setPixel(x, y, c.rgb());
         }
    }

    k->pix = new QPixmap(QPixmap::fromImage(img));
    setAttribute(Qt::WA_NoSystemBackground);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

TupColorPicker::~TupColorPicker()
{
    delete k;
    #ifdef K_DEBUG
           TEND;
    #endif
}

QPoint TupColorPicker::colorPoint()
{ 
    return QPoint((360-k->hue)*(pWidth-1)/360, (255-k->saturation)*(pHeight-1)/255); 
}

int TupColorPicker::huePoint(const QPoint &pt)
{ 
    return 360 - pt.x()*360/(pWidth-1); 
}

int TupColorPicker::saturationPoint(const QPoint &pt)
{ 
    return 255 - pt.y()*255/(pHeight-1); 
}

void TupColorPicker::setColor(const QPoint &pt)
{ 
    // tFatal() << "TupColorPicker::setColor() - Setting new color point!";
    setColor(huePoint(pt), saturationPoint(pt)); 
}

QSize TupColorPicker::sizeHint() const
{
    return QSize(pWidth + 2*frameWidth(), pHeight + 2*frameWidth());
}

void TupColorPicker::setColor(int hue, int saturation)
{
    int nhue = qMin(qMax(0, hue), 359);
    int nsat = qMin(qMax(0, saturation), 255);

    if (nhue == k->hue && nsat == k->saturation) {
        // tFatal() << "TupColorPicker::setColor() - Same values... exiting!!!";
        return;
    }

    QRect rect(colorPoint(), QSize(20,20));
    k->hue = nhue; 
    k->saturation = nsat;
    rect = rect.unite(QRect(colorPoint(), QSize(20,20)));
    rect.translate(contentsRect().x()-9, contentsRect().y()-9);

    repaint(rect);
}

void TupColorPicker::setHUE(int hue)
{
    int newHue = qMin(qMax(0, hue), 359);
    if (newHue == k->hue)
        return;

    QRect rect(colorPoint(), QSize(20, 20));
    k->hue = newHue;
    rect = rect.unite(QRect(colorPoint(), QSize(20, 20)));
    rect.translate(contentsRect().x()-9, contentsRect().y()-9);

    repaint(rect);
}

void TupColorPicker::setSaturation(int saturation)
{
    int newSat = qMin(qMax(0, saturation), 255);
    if (newSat == k->saturation)
        return;

    QRect rect(colorPoint(), QSize(20,20));
    k->saturation = newSat;
    rect = rect.unite(QRect(colorPoint(), QSize(20,20)));
    rect.translate(contentsRect().x()-9, contentsRect().y()-9);

    repaint(rect);
}

void TupColorPicker::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);
    emit newColor(k->hue, k->saturation);
}

void TupColorPicker::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos() - contentsRect().topLeft();
    setColor(point);
    emit newColor(k->hue, k->saturation);
}

void TupColorPicker::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    drawFrame(&painter);
    QRect rect = contentsRect();

    painter.drawPixmap(rect.topLeft(), *k->pix);
    QPoint point = colorPoint() + rect.topLeft();
    painter.setPen(Qt::black);

    painter.fillRect(point.x()-9, point.y(), 20, 2, Qt::black);
    painter.fillRect(point.x(), point.y()-9, 2, 20, Qt::black);
}

int TupColorPicker::hue()
{
    return k->hue;
}

int TupColorPicker::saturation()
{
    return k->saturation;
}

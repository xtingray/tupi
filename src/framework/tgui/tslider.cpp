/***************************************************************************
 *   Project TUPI: 2D Magic                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *                                                                         *
 *   Developers:                                                           *
 *   2012:                                                                 *
 *    Gustavo Gonzalez / @xtingray                                         *
 *    Andres Calderon / @andresfcalderon                                   *
 *    Antonio Vanegas / @hpsaturn                                          *
 *                                                                         *
 *   Tupi is a fork of the KTooN project                                   *
 *   KTooN's versions:                                                     *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2012 Mae Floresta - http://www.maefloresta.com          *
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

#include "tslider.h"
#include "tapplicationproperties.h" 

#include <QDesktopWidget>
#include <QDebug>

struct TSlider::Private
{
    int min;
    int max;
    QColor startColor;
    QColor endColor;
    QImage *image;
    int imageW;
    int imageH;

    int value;
    bool enabled;
    Qt::Orientation orientation;
    TSlider::Mode mode;
    Qt::BrushStyle style;
    double opacity;
    int currentBase;
};

TSlider::TSlider(Qt::Orientation orientation, TSlider::Mode mode, const QColor &start, const QColor &end, QWidget *parent) : QGraphicsView(parent), k(new Private)
{
    setStyleSheet("* { background-color: rgba(255,255,255,0); border: 1px solid rgb(170,170,170); }");

    k->orientation = orientation; 
    k->mode = mode;
    k->startColor = start;
    k->endColor = end;
    k->value = 0;
    k->enabled = true;

    if (k->orientation == Qt::Vertical) {
        k->image = new QImage(THEME_DIR + "icons/slider_vertical.png");
        k->imageW = k->image->width();
        k->imageH = k->image->height();
        setFixedWidth(k->imageW + 2);
    } else {
        k->image = new QImage(THEME_DIR + "icons/slider_horizontal.png");
        k->imageW = k->image->width();
        k->imageH = k->image->height();
        setFixedHeight(k->imageH + 2);
    }

    setUpdatesEnabled(true);

    if (k->orientation == Qt::Vertical)
        k->currentBase = viewport()->height();
    else
        k->currentBase = viewport()->width();
}

TSlider::~TSlider()
{
}

void TSlider::setBrushSettings(Qt::BrushStyle style, double opacity)
{
    k->style = style;
    k->opacity = opacity;
}

void TSlider::setRange(int min, int max)
{
    k->min = min;
    k->max = max;
}

void TSlider::setValue(int value)
{
    if (k->orientation == Qt::Vertical) {
        int height = viewport()->height();
        if (value == k->max) {
            k->value = height - k->image->height()/2;
        } else if (value == k->min) {
            k->value = 0;
        } else {
            k->value = height*value/k->max;
        }
    } else {
        int width = viewport()->width();
        if (value == k->max) {
            k->value = width - k->image->width()/2;
        } else if (value == k->min) {
            k->value = 0;
        } else {
            k->value = width*value/k->max;
        }
    }

    this->update();
}

void TSlider::setEnabled(bool flag)
{
    k->enabled = flag;
    this->update();
}

bool TSlider::isEnabled()
{
    return k->enabled;
}

void TSlider::setColors(const QColor &start, const QColor &end)
{
    k->startColor = start;
    k->endColor = end;

    this->update();
}

void TSlider::mousePressEvent(QMouseEvent *event)
{
    if (!k->enabled)
        return;

    int pos = -1;
    if (k->orientation == Qt::Vertical)
        pos = event->y();
    else
        pos = event->x();

    calculateNewPosition(pos);
}

void TSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (!k->enabled) 
        return;

    int pos = -1;
    if (k->orientation == Qt::Vertical) 
        pos = event->y();
    else
        pos = event->x();

    calculateNewPosition(pos);
}

void TSlider::calculateNewPosition(int pos)
{
    int length = -1;
    k->value = pos;

    if (k->orientation == Qt::Vertical) {
        length = viewport()->height();
        if (pos > (length - k->image->height())) {
            this->update();
            if (k->mode == Color)
                calculateColor(k->min);
            else
                emit valueChanged(k->min);
            return;
        }
    } else {
        length = viewport()->width();
        if (pos > (length - k->image->width())) {
            this->update();
            if (k->mode == Color) 
                calculateColor(k->max);
            else
                emit valueChanged(k->max);
            return;
        }
    }

    int value = -1;
    if (k->orientation == Qt::Vertical) 
        value = k->min + (k->max - k->min) * (1.0 - float(pos)/float(length));
    else 
        value = k->min + (k->max - k->min) * (float(pos)/float(length));

    if (k->value < 0)
        k->value = 0;

    if (value < k->min)
        value = k->min;

    this->update();

    if (k->mode == Color) 
        calculateColor(value);
    else
        emit valueChanged(value);
}

void TSlider::calculateColor(int value)
{
    int r = k->endColor.red();
    int g = k->endColor.green();
    int b = k->endColor.blue();

    r = (r*value)/100;
    g = (g*value)/100;
    b = (b*value)/100;

    QColor color = QColor(r, g, b);
    emit colorChanged(color);
}

void TSlider::paintScales()
{
    QPainter painter(viewport());

    if (!k->enabled) {
        QColor color(232, 232, 232);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawRect(0, 0, viewport()->width(), viewport()->height());
        return;
    }

    int width = viewport()->width();
    int height = viewport()->height();
    int length = viewport()->width();
    if (k->orientation == Qt::Vertical)
        length = viewport()->height();
    int segments = 32;
    int delta = length/(segments-1);

    if (k->mode == Color) {
        for (int section=0; section<=segments; section++) {
             QColor color;
             int r;
             int g;
             int b;
             r = section*(k->endColor.red() - k->startColor.red()) / segments + k->startColor.red();
             g = section*(k->endColor.green() - k->startColor.green()) / segments + k->startColor.green();
             b = section*(k->endColor.blue() - k->startColor.blue()) / segments + k->startColor.blue();

             if ((r > -1 && r < 256) && (g > -1 && g < 256) && (b > -1 && b < 256)) {
                 color.setRed(r);
                 color.setGreen(g);
                 color.setBlue(b);

                 painter.setPen(color);
                 painter.setBrush(color);

                 if (k->orientation == Qt::Vertical)
                     painter.drawRect((width - k->imageW)/2, section*delta, k->imageW, delta);
                 else 
                     painter.drawRect(section*delta, (height - k->imageH)/2, delta, k->imageH);
            }
        }
    } else if (k->mode == Size) {
               painter.setPen(QColor(232, 232, 232));
               painter.setBrush(QBrush(k->endColor, k->style));
               painter.setOpacity(k->opacity);
               painter.setRenderHint(QPainter::Antialiasing);

               QPainterPath path;

               if (k->orientation == Qt::Vertical) {
                   path = QPainterPath(QPointF(0, 0));
                   path.lineTo(QPointF(0, height));
                   path.lineTo(QPointF(width, height));
                   path.lineTo(QPointF(0, 0));
               } else {
                   path = QPainterPath(QPointF(0, height));
                   path.lineTo(QPointF(width, 0)); 
                   path.lineTo(QPointF(width, height));
                   path.lineTo(QPointF(0, height));
               }

               painter.drawPath(path);
               painter.setOpacity(1.0);
    } else if (k->mode == Opacity) {
               double opacityDelta = 1.0/32; 
               double opacity = 0;
               for (int section=0; section<=segments; section++) {
                    painter.setPen(QColor(232, 232, 232));
                    painter.setBrush(k->endColor);
                    painter.setOpacity(opacity);
                    opacity += opacityDelta;

                    if (k->orientation == Qt::Vertical) {
                        painter.drawRect((width - k->imageW)/2, section*delta, k->imageW, delta);
                     } else {
                        painter.drawRect(section*delta, (height - k->imageH)/2, delta, k->imageH);
                     }
               }
               painter.setOpacity(1.0);
    } else if (k->mode == FPS) {
        for (int section=0; section<=segments; section++) {
             painter.setPen(QColor(232, 232, 232));
             painter.setBrush(k->endColor);
             painter.setOpacity(1.0);
             if (k->orientation == Qt::Vertical) {
                 painter.drawRect((width - k->imageW)/2, section*delta, k->imageW, delta);
              } else {
                 painter.drawRect(section*delta, (height - k->imageH)/2, delta, k->imageH);
              }
        }
    }

    // Drawing selector image
    if (k->orientation == Qt::Vertical) {
        int h = viewport()->height();

        if (k->value >= h || k->currentBase != h) {
            k->value = (k->value * h)/k->currentBase;
            k->currentBase = h;
        }

        painter.drawImage((width/2)-(k->imageW/2), k->value, *k->image);

        int x = (width/2)-(k->imageW/2);
        int middle = k->imageH/2;
        if (k->value <= middle) {
            painter.drawImage(x, k->value, *k->image);
        } else if (k->value >= (h - middle)) {
            painter.drawImage(x, h - k->imageH, *k->image);
        } else {
            painter.drawImage(x, k->value - middle, *k->image);
        }
    } else {
        int w = viewport()->width();

        if (k->value >= w || k->currentBase != w) {
            k->value = (k->value * w)/k->currentBase;
            k->currentBase = w;
        }

        int y = (height/2)-(k->imageH/2);
        int middle = k->imageW/2; 
        if (k->value <= middle) {
            painter.drawImage(k->value, y, *k->image);
        } else if (k->value >= (w - middle)) {
            painter.drawImage(w - k->imageW, y, *k->image);
        } else {
            painter.drawImage(k->value - middle, y, *k->image);
        }
    }
}

void TSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    paintScales();
}

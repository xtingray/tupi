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

#include "ktluminancepicker.h"

#include <QPainter>
#include <qdrawutil.h>
#include "kdebug.h"

struct KTLuminancePicker::Private
{
    int val;
    int hue;
    int sat;
    QPixmap *pix;

    ~Private()
     {
       delete pix;
     };
};

int KTLuminancePicker::y2val(int y)
{
    int k = height() - 2*coff - 1;
    return 255 - (y - coff)*255/k;
}

int KTLuminancePicker::val2y(int v)
{
    int k = height() - 2*coff - 1;
    return coff + (255-v)*k/255;
}

KTLuminancePicker::KTLuminancePicker(QWidget* parent)
	:QWidget(parent), k( new Private)
{
    k->hue = 100; 
    k->val = 100; 
    k->sat = 100;
    k->pix = 0;
}

KTLuminancePicker::~KTLuminancePicker()
{
    delete k;
    #ifdef K_DEBUG
           KEND;
    #endif
}

void KTLuminancePicker::mouseMoveEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
	
}

void KTLuminancePicker::mousePressEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}

void KTLuminancePicker::setVal(int v)
{
    if (k->val == v)
        return;
    k->val = qMax(0, qMin(v,255));
    delete k->pix; k->pix=0;
    repaint();

    emit newHsv(k->hue, k->sat, k->val);
}

//receives from a k->hue,k->sat chooser and relays.
void KTLuminancePicker::setCol(int h, int s)
{
    setCol(h, s, k->val);
    emit newHsv(h, s, k->val);
}

void KTLuminancePicker::paintEvent(QPaintEvent *)
{
    int w = width() - 5;

    QRect r(0, foff, w, height() - 2*foff);
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if (!k->pix || k->pix->height() != hi || k->pix->width() != wi) {
        delete k->pix;
        QImage img(wi, hi, QImage::Format_RGB32);
        int y;
        for (y = 0; y < hi; y++) {
             QColor c;
             c.setHsv(k->hue, k->sat, y2val(y+coff));
             QRgb r = c.rgb();
             int x;
             for (x = 0; x < wi; x++)
                  img.setPixel(x, y, r);
        }
        k->pix = new QPixmap(QPixmap::fromImage(img));
    }

    QPainter p(this);
    p.drawPixmap(1, coff, *k->pix);
    const QPalette &g = palette();
    qDrawShadePanel(&p, r, g, true);
    p.setPen(g.foreground().color());
    p.setBrush(g.foreground());
    QPolygon a;
    int y = val2y(k->val);
    a.setPoints(3, w, y, w+5, y+5, w+5, y-5);
    p.eraseRect(w, 0, 5, height());
    p.drawPolygon(a);
}

void KTLuminancePicker::setCol(int h, int s , int v)
{
    k->val = v;
    k->hue = h;
    k->sat = s;
    delete k->pix; 
    k->pix = 0;
    repaint();
}

int  KTLuminancePicker::value()
{
    return k->val;
}


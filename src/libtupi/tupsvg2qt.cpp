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

#include "tupsvg2qt.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "tdebug.h"

TupSvg2Qt::TupSvg2Qt()
{
}

TupSvg2Qt::~TupSvg2Qt()
{
}

// SVG PATH 2 PAINTER PATH

static void pathArcSegment(QPainterPath &path,
               qreal xc, qreal yc,
               qreal th0, qreal th1,
               qreal rx, qreal ry, qreal xAxisRotation)
{
    qreal sinTh, cosTh;
    qreal a00, a01, a10, a11;
    qreal x1, y1, x2, y2, x3, y3;
    qreal t;
    qreal thHalf;

    sinTh = sin(xAxisRotation * (M_PI / 180.0));
    cosTh = cos(xAxisRotation * (M_PI / 180.0));

    a00 =  cosTh * rx;
    a01 = -sinTh * ry;
    a10 =  sinTh * rx;
    a11 =  cosTh * ry;

    thHalf = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * sin(thHalf * 0.5) * sin(thHalf * 0.5) / sin(thHalf);
    x1 = xc + cos(th0) - t * sin(th0);
    y1 = yc + sin(th0) + t * cos(th0);
    x3 = xc + cos(th1);
    y3 = yc + sin(th1);
    x2 = x3 + t * sin(th1);
    y2 = y3 - t * cos(th1);

    path.cubicTo(a00 * x1 + a01 * y1, a10 * x1 + a11 * y1,
             a00 * x2 + a01 * y2, a10 * x2 + a11 * y2,
             a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

// the arc handling code underneath is from XSVG (BSD license)
/*
 * Copyright  2002 USC/Information Sciences Institute
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of
 * Information Sciences Institute not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  Information Sciences Institute
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * INFORMATION SCIENCES INSTITUTE DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INFORMATION SCIENCES
 * INSTITUTE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

static void pathArc(QPainterPath &path,
            qreal        rx,
            qreal        ry,
            qreal        x_axis_rotation,
            int        large_arc_flag,
            int        sweep_flag,
            qreal        x,
            qreal        y,
            qreal curx, qreal cury)
{
    qreal sin_th, cos_th;
    qreal a00, a01, a10, a11;
    qreal x0, y0, x1, y1, xc, yc;
    qreal d, sfactor, sfactor_sq;
    qreal th0, th1, th_arc;
    int i, n_segs;
    qreal dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;

    rx = qAbs(rx);
    ry = qAbs(ry);

    sin_th = sin(x_axis_rotation * (M_PI / 180.0));
    cos_th = cos(x_axis_rotation * (M_PI / 180.0));

    dx = (curx - x) / 2.0;
    dy = (cury - y) / 2.0;
    dx1 =  cos_th * dx + sin_th * dy;
    dy1 = -sin_th * dx + cos_th * dy;
    Pr1 = rx * rx;
    Pr2 = ry * ry;
    Px = dx1 * dx1;
    Py = dy1 * dy1;
    /* Spec : check if radii are large enough */
    check = Px / Pr1 + Py / Pr2;

    if (check > 1) {
        rx = rx * sqrt(check);
        ry = ry * sqrt(check);
    }

    a00 =  cos_th / rx;
    a01 =  sin_th / rx;
    a10 = -sin_th / ry;
    a11 =  cos_th / ry;
    x0 = a00 * curx + a01 * cury;
    y0 = a10 * curx + a11 * cury;
    x1 = a00 * x + a01 * y;
    y1 = a10 * x + a11 * y;
    /* (x0, y0) is current point in transformed coordinate space.
    (x1, y1) is new point in transformed coordinate space.

    The arc fits a unit-radius circle in this space.
    */
    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    sfactor_sq = 1.0 / d - 0.25;
    if (sfactor_sq < 0) sfactor_sq = 0;
    sfactor = sqrt(sfactor_sq);
    if (sweep_flag == large_arc_flag) sfactor = -sfactor;
    xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
    yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
    /* (xc, yc) is center of the circle. */

    th0 = atan2(y0 - yc, x0 - xc);
    th1 = atan2(y1 - yc, x1 - xc);

    th_arc = th1 - th0;
    if (th_arc < 0 && sweep_flag)
        th_arc += 2 * M_PI;
    else if (th_arc > 0 && !sweep_flag)
        th_arc -= 2 * M_PI;

    n_segs = int(ceil(qAbs(th_arc / (M_PI * 0.5 + 0.001))));

    for (i = 0; i < n_segs; i++) {
        pathArcSegment(path, xc, yc,
                   th0 + i * th_arc / n_segs,
                   th0 + (i + 1) * th_arc / n_segs,
                   rx, ry, x_axis_rotation);
    }
}

QList<qreal> TupSvg2Qt::parseNumbersList(QString::const_iterator &itr)
{
    QList<qreal> points;
    QString temp;
    
    while ((*itr).isSpace()) {
        ++itr;
    }
    
    while ((*itr).isNumber() || (*itr) == QLatin1Char('-') 
           || (*itr) == QLatin1Char('+') || (*itr) == QLatin1Char('.')) {

        temp = QString();

        if ((*itr) == QLatin1Char('-'))
            temp += *itr++;
        else if ((*itr) == QLatin1Char('+'))
            temp += *itr++;

        while ((*itr).isDigit())
            temp += *itr++;

        if ((*itr) == QLatin1Char('.'))
            temp += *itr++;

        while ((*itr).isDigit())
            temp += *itr++;

        if (( *itr) == QLatin1Char('e')) {
            temp += *itr++;
            if ((*itr) == QLatin1Char('-') || (*itr) == QLatin1Char('+'))
                temp += *itr++;
        }

        while ((*itr).isDigit())
               temp += *itr++;
        
        while ((*itr).isSpace())
               ++itr;
        
        if ((*itr) == QLatin1Char(',')) {
            ++itr;
        }
        
        bool ok = false;
        qreal number = temp.toDouble(&ok);
        
        if (ok)
            points.append(number);
        else
            points.append(0.0f);
        
        // eat the rest of space
        while ((*itr).isSpace()) {
               ++itr;
        }
    }

    return points;
}

bool TupSvg2Qt::svgpath2qtpath(const QString &data, QPainterPath &path)
{
    QString::const_iterator itr = data.constBegin();
    qreal x0 = 0, y0 = 0;              // starting point
    qreal x = 0, y = 0;                // current point
    char lastMode = 0;
    QChar pathElem;
    QPointF ctrlPt;
    
    while (itr != data.constEnd()) {
        while ((*itr).isSpace())
            ++itr;
        
        pathElem = *itr;
        ++itr;
        QList<qreal> arg = parseNumbersList(itr);
        
        if (pathElem == QLatin1Char('z') || pathElem == QLatin1Char('Z'))
            arg.append(0);//dummy

        while (!arg.isEmpty()) {
               qreal offsetX = x;        // correction offsets
               qreal offsetY = y;        // for relative commands

               switch (pathElem.toAscii()) {
                       case 'm': 
                       {
                           x = x0 = arg[0] + offsetX;
                           y = y0 = arg[1] + offsetY;
                           path.moveTo(x0, y0);
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'M': 
                       {
                           x = x0 = arg[0];
                           y = y0 = arg[1];
                           path.moveTo(x0, y0);
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'z':
                       case 'Z': 
                       {
                           x = x0;
                           y = y0;
                           path.closeSubpath();
                           arg.pop_front();//pop dummy
                       }
                       break;
                       case 'l': {
                           x = arg.front() + offsetX;
                           arg.pop_front();
                           y = arg.front() + offsetY;
                           arg.pop_front();
                           path.lineTo(x, y);
                       }
                       break;
                       case 'L': {
                           x = arg.front(); arg.pop_front();
                           y = arg.front(); arg.pop_front();
                           path.lineTo(x, y);
                       }
                       break;
                       case 'h': {
                           x = arg.front() + offsetX; arg.pop_front();
                           path.lineTo(x, y);
                       }
                       break;
                       case 'H': {
                           x = arg[0];
                           path.lineTo(x, y);
                           arg.pop_front();
                       }
                       break;
                       case 'v': {
                           y = arg[0] + offsetY;
                           path.lineTo(x, y);
                           arg.pop_front();
                       }
                       break;
                       case 'V': {
                           y = arg[0];
                           path.lineTo(x, y);
                           arg.pop_front();
                       }
                       break;
                       case 'c': {
                           QPointF c1(arg[0]+offsetX, arg[1]+offsetY);
                           QPointF c2(arg[2]+offsetX, arg[3]+offsetY);
                           QPointF e(arg[4]+offsetX, arg[5]+offsetY);
                           path.cubicTo(c1, c2, e);
                           ctrlPt = c2;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'C': 
                       {
                           QPointF c1(arg[0], arg[1]);
                           QPointF c2(arg[2], arg[3]);
                           QPointF e(arg[4], arg[5]);
                           path.cubicTo(c1, c2, e);
                           ctrlPt = c2;
                           x = e.x();
                           y = e.y();
                    
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 's': {
                           QPointF c1;
                           if (lastMode == 'c' || lastMode == 'C' || lastMode == 's' || lastMode == 'S')
                               c1 = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                           else
                               c1 = QPointF(x, y);
                           QPointF c2(arg[0]+offsetX, arg[1]+offsetY);
                           QPointF e(arg[2]+offsetX, arg[3]+offsetY);
                           path.cubicTo(c1, c2, e);
                           ctrlPt = c2;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'S': {
                           QPointF c1;
                           if (lastMode == 'c' || lastMode == 'C' ||
                               lastMode == 's' || lastMode == 'S')
                               c1 = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                           else
                               c1 = QPointF(x, y);
                           QPointF c2(arg[0], arg[1]);
                           QPointF e(arg[2], arg[3]);
                           path.cubicTo(c1, c2, e);
                           ctrlPt = c2;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'q': {
                           QPointF c(arg[0]+offsetX, arg[1]+offsetY);
                           QPointF e(arg[2]+offsetX, arg[3]+offsetY);
                           path.quadTo(c, e);
                           ctrlPt = c;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'Q': {
                           QPointF c(arg[0], arg[1]);
                           QPointF e(arg[2], arg[3]);
                           path.quadTo(c, e);
                           ctrlPt = c;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 't': {
                           QPointF e(arg[0]+offsetX, arg[1]+offsetY);
                           QPointF c;
                           if (lastMode == 'q' || lastMode == 'Q' ||
                               lastMode == 't' || lastMode == 'T')
                               c = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                           else
                               c = QPointF(x, y);
                           path.quadTo(c, e);
                           ctrlPt = c;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'T': {
                           QPointF e(arg[0], arg[1]);
                           QPointF c;
                           if (lastMode == 'q' || lastMode == 'Q' ||
                               lastMode == 't' || lastMode == 'T')
                               c = QPointF(2*x-ctrlPt.x(), 2*y-ctrlPt.y());
                           else
                               c = QPointF(x, y);
                           path.quadTo(c, e);
                           ctrlPt = c;
                           x = e.x();
                           y = e.y();
                           arg.pop_front(); arg.pop_front();
                       }
                       break;
                       case 'a': {
                           qreal rx = arg[0];
                           qreal ry = arg[1];
                           qreal xAxisRotation = arg[2];
                           qreal largeArcFlag  = arg[3];
                           qreal sweepFlag = arg[4];
                           qreal ex = arg[5] + offsetX;
                           qreal ey = arg[6] + offsetY;
                           qreal curx = x;
                           qreal cury = y;
                           pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag), int(sweepFlag), ex, ey,
                                   curx, cury);

                           x = ex;
                           y = ey;

                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front();
                       }
                       break;
                       case 'A': {
                           qreal rx = arg[0];
                           qreal ry = arg[1];
                           qreal xAxisRotation = arg[2];
                           qreal largeArcFlag  = arg[3];
                           qreal sweepFlag = arg[4];
                           qreal ex = arg[5];
                           qreal ey = arg[6];
                           qreal curx = x;
                           qreal cury = y;
                           pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag), int(sweepFlag), ex, ey,
                                   curx, cury);
                           x = ex;
                           y = ey;
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front(); arg.pop_front();
                           arg.pop_front();
                       }
                       break;
                       default: 
                       break;
            }

            lastMode = pathElem.toAscii();
        }
    }

    return true;
}


// Pen

void TupSvg2Qt::parsePen(QPen &pen, const QXmlAttributes &attributes)
{
    QString value = attributes.value(QLatin1String("stroke"));
    QString dashArray = attributes.value(QLatin1String("stroke-dasharray"));
    QString dashOffset = attributes.value(QLatin1String("stroke-dashoffset"));
    QString linecap = attributes.value(QLatin1String("stroke-linecap"));
    QString linejoin = attributes.value(QLatin1String("stroke-linejoin"));
    QString miterlimit = attributes.value(QLatin1String("stroke-miterlimit"));
    QString opacity = attributes.value(QLatin1String("stroke-opacity"));
    QString width = attributes.value(QLatin1String("stroke-width"));
    QString myId = attributes.value(QLatin1String("id"));
    
    if (opacity.isEmpty())
        opacity = attributes.value(QLatin1String("opacity"));

    if (!value.isEmpty() || !width.isEmpty()) {
        if (value != QLatin1String("none")) {
            if (!value.isEmpty()) {
                QColor color(value);
                double alpha = opacity.toDouble();

                if (alpha < 1)
                    alpha *= 255;
                
                color.setAlpha(int(alpha));
                
                pen.setColor(color);
                pen.setStyle(Qt::SolidLine);
            }
            
            if (!width.isEmpty()) {
                double widthF = width.toDouble();
                
                if (!widthF) {
                    pen.setStyle(Qt::NoPen);
                    return;
                }
                pen.setWidthF(widthF);
            }
            qreal penw = pen.widthF();

            if (!linejoin.isEmpty()) {
                if (linejoin == QLatin1String("miter"))
                    pen.setJoinStyle(Qt::SvgMiterJoin);
                else if (linejoin == QLatin1String("round"))
                    pen.setJoinStyle(Qt::RoundJoin);
                else if (linejoin == QLatin1String("bevel"))
                    pen.setJoinStyle(Qt::BevelJoin);
            }
            
            if (!miterlimit.isEmpty()) {
                pen.setMiterLimit(miterlimit.toDouble());
            }

            if (!linecap.isEmpty()) {
                if (linecap == QLatin1String("butt"))
                    pen.setCapStyle(Qt::FlatCap);
                else if (linecap == QLatin1String("round"))
                    pen.setCapStyle(Qt::RoundCap);
                else if (linecap == QLatin1String("square"))
                    pen.setCapStyle(Qt::SquareCap);
            }

            if (!dashArray.isEmpty()) {
                QString::const_iterator itr = dashArray.constBegin();
                QList<qreal> dashes = parseNumbersList(itr);
                QVector<qreal> vec(dashes.size());

                int i = 0;
                foreach(qreal dash, dashes) {
                    vec[i++] = dash/penw;
                }
                pen.setDashPattern(vec);
            }

        } else {
            pen.setStyle(Qt::NoPen);
        }
    }
}

// Brush

bool TupSvg2Qt::parseBrush(QBrush &brush, const QXmlAttributes &attributes)
{
    QString value   = attributes.value(QLatin1String("fill"));
    QString opacity = attributes.value(QLatin1String("fill-opacity"));

    if (opacity.isEmpty())
        opacity = attributes.value(QLatin1String("opacity"));

    QColor color;
    if (!value.isEmpty() || !opacity.isEmpty()) {
        if (value.startsWith(QLatin1String("url"))) {
            value = value.remove(0, 3);
            // QString id = idFromUrl(value);
            // FIXME: parsing is missing 
        } else if (value != QLatin1String("none")) {
            bool ok = false;
            
            int vcolor = value.toInt(&ok);
            if (!ok)
                color = QColor(value);
            else
                color = QColor(vcolor);
            
            double alpha = opacity.toDouble();

            if (alpha < 1) 
                alpha *= 255;
            
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(color);
        } else {
            brush = QBrush(Qt::NoBrush);
        }

        return true;
    }

    return false;
}

bool TupSvg2Qt::svgmatrix2qtmatrix(const QString &data, QMatrix &matrix)
{
    if (data.isEmpty()) 
        return false;
    
    QString::const_iterator itr = data.constBegin();

    while (itr != data.constEnd()) {

           if ((*itr) == QLatin1Char('m')) {  //matrix
               QString temp(QLatin1String("m"));
               int remains = 6;
               while (remains--) {
                      temp += *itr++;
               }

               while ((*itr).isSpace()) {
                      ++itr;
               }
            
               ++itr;// '('
               QList<qreal> points = parseNumbersList(itr);
               ++itr; // ')'

               Q_ASSERT(points.count() == 6);
               matrix = matrix * QMatrix(points[0], points[1],points[2], points[3], points[4], points[5]);
           }
    }
    
    return true;
}

bool TupSvg2Qt::parsePointF(const QString &pointstr, QPointF &point)
{
    QString::const_iterator itr = pointstr.constBegin();
    QList<qreal> points = parseNumbersList(++itr);
        
    if (points.count() != 2) 
        return false;
        
    point.setX(points[0]);
    point.setY(points[1]);

    return true;
}

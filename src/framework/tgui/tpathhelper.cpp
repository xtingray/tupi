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

#include "tpathhelper.h"

TPathHelper::TPathHelper()
{
}

TPathHelper::~TPathHelper()
{
}

QPainterPath TPathHelper::toRect(const QPainterPath &p, const QRect &rect, float offset)
{
    QPainterPath path;
    
    QRectF br = p.boundingRect();
    QMatrix matrix;
    
    float sx = 1, sy = 1;

    if (rect.width() < br.width())
        sx = static_cast<float>(rect.width()-offset) / static_cast<float>(br.width());

    if (rect.height() < br.height())
        sy = static_cast<float>(rect.height()-offset) / static_cast<float>(br.height());
    
    float factor = qMin(sx, sy);
    matrix.scale(factor, factor);
    path = matrix.map(p);
    
    matrix.reset();
    
    QPointF pos = path.boundingRect().topLeft();
    
    float tx = offset/2-pos.x(), ty = offset/2-pos.y();
    
    matrix.translate(tx, ty);
    return matrix.map(path);
}

QList<QPainterPath> TPathHelper::toRect(const QList<QPainterPath> &l, const QRect &rect, float offset)
{
    QList<QPainterPath> returnList;
    QRectF br;
    
    foreach (QPainterPath in, l)
             br = br | in.boundingRect().toRect();
    
    foreach (QPainterPath path, l) {
             QMatrix matrix;
             float sx = 1, sy = 1;

             if (rect.width() < br.width())
                 sx = static_cast<float>(rect.width()-offset) / static_cast<float>(br.width());

             if (rect.height() < br.height())
                 sy = static_cast<float>(rect.height()-offset) / static_cast<float>(br.height());
        
             float factor = qMin(sx, sy);
             matrix.scale(factor, factor);
             path = matrix.map(path);
        
             matrix.reset();
        
             QPointF pos = path.boundingRect().topLeft();
        
             float tx = offset/2-pos.x(), ty = offset/2-pos.y();
        
             matrix.translate(tx, ty);
             returnList << matrix.map(path);
    }
    
    return returnList;
}

QPainterPath TPathHelper::buildPath(const QStringList &polygonsStr, QChar sep)
{
    // SQA: It doesn't work for curves
    QPainterPath path;
    
    foreach (QString polTmp, polygonsStr) {
        QStringList points = polTmp.trimmed().split(' ');
        
        QPolygonF polygon;
        
        foreach (QString p, points) {
                 bool valid = false;
                 double x = p.section(sep, 0, 0).toDouble(&valid);
                 double y = p.section(sep, 1, 1).toDouble(&valid);
            
                 if (valid)
                     polygon << QPointF(x, y);
        }
        
        path.addPolygon(polygon);
    }

    return path;
}

QPainterPath TPathHelper::buildPath(const QString &svgpath)
{
    Q_UNUSED(svgpath);

    #ifdef K_DEBUG
        QString msg = "TPathHelper::buildPath() - Warning: method pending for implementation";
        #ifdef Q_OS_WIN32
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif
    
    return QPainterPath();
}


QPainterPath TPathHelper::fromElements(const QList<QPainterPath::Element>& elements)
{
    QPainterPath shape;
    QVector<QPointF> curve;
    
    foreach (QPainterPath::Element e, elements) {
             switch(e.type) {
                    case QPainterPath::MoveToElement:
                    {
                         shape.moveTo(e.x, e.y);
                         break;
                    }
                    case QPainterPath::LineToElement:
                    {
                         shape.lineTo(e.x, e.y);
                         break;
                    }
                    case QPainterPath::CurveToDataElement:
                    {
                         curve << e;
                         if (curve.count() == 3)
                             shape.cubicTo(curve[0], curve[1], curve[2]);
                         break;
                    }
                    case QPainterPath::CurveToElement:
                    {
                         curve.clear();
                         curve << e;
                         break;
                    }
             }
    }

    return shape;
}

QList<QPainterPath> TPathHelper::toSubpaths(const QPainterPath &path)
{
    QList<QPainterPath> paths;
    QList<QPainterPath::Element> elements;
    
    for (int index = 0; index < path.elementCount(); index++) {
         QPainterPath::Element e = path.elementAt(index);
        
         if (e.type == QPainterPath::MoveToElement && !elements.isEmpty()) {
             QPainterPath path = TPathHelper::fromElements(elements);
             paths << path;
             elements.clear();
         }
        
         elements << e;
    }
    
    if (! elements.isEmpty()) {
        QPainterPath path = TPathHelper::fromElements(elements);
        paths << path;
        elements.clear();
    }
    
    return paths;
}

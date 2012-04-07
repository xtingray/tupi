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

#ifndef __TUPGRAPHICALGORITHM_H__
#define __TUPGRAPHICALGORITHM_H__

#include <QString>
#include <QPolygon>
#include <QPainterPath>
#include "tupglobal.h"

/**
 * @author Jorge Cuadrado
 */

class TUPI_EXPORT TupGraphicalAlgorithm
{
    private:
        TupGraphicalAlgorithm() {}
        ~TupGraphicalAlgorithm() {};
        
    public:
        static QPainterPath bezierFit(QPolygonF &points_, float error, int from = 0, int to = -1);
        static QPolygonF polygonFit(const QPolygonF &points);
        static bool intersectLine(const QPointF &start, const QPointF& end, const QRectF& rect);
        static char calculateCode(const QPointF &point, const QRectF &window);
        static double distanceToPoint(const QPointF &pos);
        static double angleForPos(const QPointF &pos, const QPointF &anchor = QPointF(0,0));
        
    private:
        enum CohenSutherlandCode
        {
            Bit1 = 1<<1,
            Bit2 = 1<<2,
            Bit3 = 1<<3,
            Bit4 = 1<<4
        };
        
        static void printCode(char code);
};

#endif

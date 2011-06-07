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

#include "kindicatordrawer.h"
#include "tdebug.h"

#include <QRadialGradient>
#include <cmath> //abs

struct KIndicatorDrawer::Private
{
    int face;
};

KIndicatorDrawer::KIndicatorDrawer(): k(new Private)
{
    k->face = 1;
}

KIndicatorDrawer::~KIndicatorDrawer()
{
    delete k;
}

void KIndicatorDrawer::paint(QPainter *painter, const QRectF & rect, const QColor &color, const QColor &fg)
{
    painter->save();
    
    painter->scale(rect.width()/54, rect.height()/54);
    painter->setPen(Qt::NoPen);
    
    int r = color.red();
    int g = color.green();
    int b = color.blue();
    
    for (int i = 0; i < 8; i++) {

         painter->save();
         float data = 8 + k->face - i;     
         float value = std::abs(data);    
         int result = (int) value;
         int alpha = 31*(8-(result%8));
        
         QColor bg(r, g, b, alpha);
        
         QRectF rect(12,0,15,15);
        
         QRadialGradient grad(rect.center(), 15.0/2.0, rect.topLeft());
        
         QColor w = fg;
         w.setAlpha(alpha);
        
         grad.setColorAt(0.0, w);
         grad.setColorAt(1.0, bg);
        
         painter->setBrush(grad);
        
         // painter->setBrush(QBrush(bg, Qt::DiagCrossPattern));
        
         painter->translate(27,27);
         painter->rotate(45*i);
        
         painter->drawEllipse(rect);
       
         painter->restore();
    }
    
    painter->restore();
}

void KIndicatorDrawer::advance()
{
    k->face = (k->face+1) % 8;
}


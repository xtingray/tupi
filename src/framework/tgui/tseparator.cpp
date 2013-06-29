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

#include "tseparator.h"
#include <QStyleOption>
#include <QPainter>

TSeparator::TSeparator(QWidget* parent) : QFrame(parent)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation(Qt::Horizontal);
}

TSeparator::TSeparator(Qt::Orientation orientation, QWidget* parent)
   : QFrame(parent)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation(orientation);
}

void TSeparator::setOrientation(Qt::Orientation orientation)
{
   if (orientation == Qt::Vertical) {
      setFrameShape(QFrame::VLine);
      setFrameShadow(QFrame::Sunken);
      setMinimumSize(2, 0);
   } else {
      setFrameShape(QFrame::HLine);
      setFrameShadow(QFrame::Sunken);
      setMinimumSize(0, 2);
   }
}

Qt::Orientation TSeparator::orientation() const
{
   return (frameStyle() & VLine) ? Qt::Vertical : Qt::Horizontal;
}

void TSeparator::virtual_hook(int, void*)
{ 
    /*BASE::virtual_hook( id, data );*/ 
}

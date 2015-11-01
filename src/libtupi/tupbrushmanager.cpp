/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupbrushmanager.h"

/**
 * This class handles the set of brushes for painting.
 * Here is where methods to set/unset brushes are defined.
 * @author David Cuadrado
*/

struct TupBrushManager::Private
{
    Private() : pen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap)), brush(Qt::transparent) {}
    QPen pen;
    QBrush brush;
};

TupBrushManager::TupBrushManager(QObject * parent) : QObject(parent), k(new Private)
{
}

TupBrushManager::TupBrushManager(const QPen &pen, const QBrush &brush, QObject * parent) : QObject(parent), k(new Private)
{
    k->pen = pen;
    k->brush = brush;
}

TupBrushManager::~TupBrushManager()
{
    delete k;
}

void TupBrushManager::setPen(const QPen &pen)
{
    k->pen = pen;
    emit penChanged(k->pen);
}

void TupBrushManager::setPenColor(const QColor &color)
{
    QBrush brush = k->pen.brush();
    brush.setColor(color);
    k->pen.setBrush(brush);

    emit penChanged(k->pen);
}

QPen TupBrushManager::pen() const
{
    return k->pen;
}

void TupBrushManager::setBrush(const QBrush &brush)
{
    k->brush = brush;
    emit brushChanged(brush);
}

QBrush TupBrushManager::brush() const
{
    return k->brush;
}

int TupBrushManager::penWidth() const
{
    return k->pen.width();
}

QColor TupBrushManager::penColor() const
{
    return k->pen.color();
}

QBrush TupBrushManager::penBrush() const
{
    return k->pen.brush();
}

QBrush TupBrushManager::brushColor() const
{
    return k->brush.color();
}

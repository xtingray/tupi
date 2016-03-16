/***************************************************************************
 *   Project TUPI: Open 2D Magic                                           *
 *   Component: tupi.mobile                                                *
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

#include "tcolorcell.h"

#include <QPainter>
#include <QDebug>

struct TColorCell::Private
{
    QBrush brush;
    FillType index;
    bool selected;
    QSize size;
};

TColorCell::TColorCell(FillType index, const QBrush &brush, const QSize &size) : k(new Private)
{
    k->index = index;
    k->selected = false;
    k->brush = brush;
    k->size = size;
    setFixedSize(k->size);
}

TColorCell::~TColorCell()
{
}

QSize TColorCell::sizeHint() const 
{
    return k->size;
}

void TColorCell::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), k->brush);
    QRect border = rect();

    if (k->selected) {
        painter.setPen(QPen(QColor(200, 200, 200), 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
        painter.setPen(QPen(QColor(190, 190, 190), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
        painter.setPen(QPen(QColor(150, 150, 150), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(border);
    } else {
        QRect frame = QRect(border.topLeft(), QSize(k->size.width()-1, k->size.height()-1));
        painter.setPen(QPen(QColor(190, 190, 190), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawRect(frame);
    }
}

void TColorCell::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    emit clicked(k->index);
    setSelected(true);
}

QColor TColorCell::color()
{
    return k->brush.color();
}

QBrush TColorCell::brush()
{
    return k->brush;
}

void TColorCell::setSelected(bool isSelected)
{
    k->selected = isSelected;
    update();
}

bool TColorCell::isSelected()
{
    return k->selected;
}

void TColorCell::setBrush(const QBrush &brush) 
{
    k->brush = brush;
    update();
}


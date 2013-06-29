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

#include "tflatbutton.h"
#include <QPainter>

TFlatButton::TFlatButton(QWidget *parent) : QAbstractButton(parent)
{
    setCheckable(true);
}

TFlatButton::TFlatButton(const QString &text, QWidget *parent) : QAbstractButton(parent)
{
    setText(text);
    setCheckable(true);
}

TFlatButton::~TFlatButton()
{
}

void TFlatButton::paintEvent(QPaintEvent *)
{
    QRect rect = contentsRect();
    
    QPainter painter(this);
    
    QPoint start = rect.topLeft();
    start.setY(rect.center().y());
    QLinearGradient gradient(start, rect.bottomLeft());
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0, palette().button().color());
    gradient.setColorAt(1, palette().background().color());
    
    painter.fillRect(rect, QBrush(gradient));
    
    painter.save();
    painter.setPen(QPen(palette().dark(), 3));
    painter.drawRect(rect.normalized().adjusted(0,0,-1,-1));
    
    if (isChecked ()) {
        QColor fillColor = palette().highlight().color();
        fillColor.setAlpha(70);
        painter.setPen(QPen(palette().highlight(), 2));
        painter.fillRect(rect.normalized().adjusted(2,2,-4,-4), fillColor);
        painter.drawRect(rect.normalized().adjusted(2,2,-2,-2));
    }
    
    painter.restore();
    
    painter.translate(rect.left(), rect.top());
    
    QRect textRect = fontMetrics().boundingRect(text());
    int textX = qMax(0, (rect.width() - textRect.width()) / 2);
    int textY = textRect.height();
    
    QPixmap pixmap = icon().pixmap(64,64);
    
    if (!pixmap.isNull()) {
        textX = qMax(textX, (pixmap.width() - textRect.width()) / 2);
        textY += 5 + pixmap.height();

        int pixX = qMax(qMax( 0, (textRect.width() - pixmap.width()) / 2), (rect.width() - pixmap.width()) / 2);
        painter.drawPixmap(pixX, textY-pixmap.height()-textRect.height(), pixmap);
    }
    
    painter.setFont(QFont("Times", 12, QFont::Bold));
    painter.drawText(textX, textY, text());
}

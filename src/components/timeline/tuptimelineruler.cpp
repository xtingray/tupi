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

#include "tuptimelineruler.h"

TupTimeLineRuler::TupTimeLineRuler(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineRuler()]";
        #else
            TINIT;
        #endif
    #endif

    setHighlightSections(true);
    setStyleSheet("QHeaderView { background-color: #CCCCCC; }");
}

TupTimeLineRuler::~TupTimeLineRuler()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupTimeLineRuler()]";
        #else
            TEND;
        #endif
    #endif
}

void TupTimeLineRuler::paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const
{
    if (!model() || !rect.isValid())
        return;

    painter->save();

    QModelIndex currentSelection = currentIndex(); 
    int column = currentSelection.row(); 

    if (selectionModel()->isSelected(model()->index(column, logicalIndex))) {
        QBrush brush(QColor(0, 135, 0, 80));
        painter->fillRect(rect, brush);
    } else {
        if ((logicalIndex + 1) == 1 || (logicalIndex+1) % 5 == 0) {
            QBrush brush(QColor(150, 150, 150, 255));
            painter->fillRect(rect, brush);
        }
    }

    logicalIndex++;

    int x = rect.bottomRight().x();
    int topY = rect.topRight().y();
    int bottomY = rect.bottomRight().y();
    painter->drawLine(x, bottomY, x, bottomY - 6);
    painter->drawLine(x, topY, x, topY + 4);

    if (logicalIndex == 1 || logicalIndex % 5 == 0) {
        QFont font = this->font();
        font.setPointSize(7);
        // QFont label("Arial", 7, QFont::Normal, false);
        QFontMetrics fm(font);

        QString number = QString::number(logicalIndex);
	
        painter->setFont(font);	
        painter->drawText((int)(rect.center().x() - (fm.width(number)/2)), 
                          (int)(rect.center().y() + (fm.height()/2)) - 2, number);
    }

    QPen pen = painter->pen();
    pen.setWidth(4);
    painter->setPen(pen); 
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    painter->restore();
}

void TupTimeLineRuler::mousePressEvent(QMouseEvent *event)
{
    emit headerSelectionChanged(logicalIndexAt(event->pos()));
}

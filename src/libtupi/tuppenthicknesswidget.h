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

#ifndef TUPPENTHICKNESSWIDGET_H
#define TUPPENTHICKNESSWIDGET_H

#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QSize>

/**
 * This class shows a preview of the pen thickness/opacity
 * @author Gustav Gonzalez <info@maefloresta.com>
 **/

class TUPI_EXPORT TupPenThicknessWidget : public QWidget
{
    Q_OBJECT

    public:
        TupPenThicknessWidget(QWidget *parent = 0);
        ~TupPenThicknessWidget();

        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        void setColor(const QColor color);
        void setBrush(int index);
        void setBrush(const QBrush brush);
        void setOpacity(double opacity);

    public slots:
        void render(int thickness);
        void render(double thickness);
        
    protected:
        void paintEvent(QPaintEvent *e);
   
    private:
        struct Private;
        Private *const k;
};

#endif

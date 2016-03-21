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

#ifndef TUPBRUSHSTATUS_H
#define TUPBRUSHSTATUS_H

#include "tglobal.h"
#include "tcolorcell.h"
#include "tupcolorwidget.h"
#include "tseparator.h"

#include <QPixmap>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QLabel>
#include <QHBoxLayout>
#include <QColorDialog>

class TupColorWidget;

class TUPI_EXPORT TupBrushStatus : public QWidget
{
    Q_OBJECT

    public:
        TupBrushStatus(const QString &label, const QPixmap &pix, bool bg);
        ~TupBrushStatus();

        void setForeground(const QPen &pen);
        void setColor(const QColor &color);
        void setTooltip(const QString &tip);
        void updateContourColorButton(bool status);

    signals:
        void colorRequested();
        void colorUpdated(const QColor);

    private slots:
        void updateColour(TColorCell::FillType); 

    private:
        // TupColorWidget *brush;
        TColorCell *brushCell;
        bool background;
        bool buttonIsChecked;
};

#endif

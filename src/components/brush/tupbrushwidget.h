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

#ifndef TUPBRUSHWIDGET_H
#define TUPBRUSHWIDGET_H

#include "tglobal.h"
#include "tupmodulewidgetbase.h"
#include "teditspinbox.h"
#include "tconfig.h"
#include "tuppaintareaevent.h"
#include "tuppenthicknesswidget.h"

#include <QPen>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

class TupPaintAreaEvent;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupBrushWidget : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupBrushWidget(QWidget *parent = 0);
        ~TupBrushWidget();
        QPen pen() const;
        void setPenThickness(int thickness);
        
    private:
        void updatePenProperties();
        void updateBrushProperties();
        void addBrushesList();
        
    public slots:
        void init(int thickness);
        void setThickness(int thickness);
        void setPenColor(const QColor color);
        void setBrush(const QBrush brush);
        
    private slots:
        void setStyle(int style);
        void setBrushStyle(QListWidgetItem *item);

        void enableRoundCapStyle();
        void enableSquareCapStyle();
        void enableFlatCapStyle();

        void enableRoundJoinStyle();
        void enableMiterJoinStyle();
        void enableBevelJoinStyle();
        
    signals:
        void paintAreaEventTriggered(const TupPaintAreaEvent *event);

    private:
        struct Private;
        Private *const k;

};

#endif

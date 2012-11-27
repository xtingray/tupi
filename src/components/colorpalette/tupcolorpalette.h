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

#ifndef TupCOLORPALETTE_H
#define TupCOLORPALETTE_H

#include "tupmodulewidgetbase.h"
#include "tdualcolorbutton.h"
#include "ticon.h"

class TupColorPalette;

/**
 * @author Jorge Cuadrado
*/

class TupColorPalette : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        enum BrushType { Solid = 0, Gradient };

        TupColorPalette(QWidget *parent = 0);
        ~TupColorPalette();

        //SQA: change this for QBrush
        QPair<QColor, QColor> color();
        void parsePaletteFile(const QString &file);
        void init();

    private:
        struct Private;
        Private *const k;

    private:
        void setupButtons();
        void setupDisplayColor();
        void setupMainPalette();
        void setupChooserTypeColor();
        void setupGradientManager();
        void setGlobalColors(const QBrush &brush);
        QIcon setComboColor(const QColor &color) const;

    // protected:
    //    void mousePressEvent(QMouseEvent * e);

    private slots:
        void setColor(const QBrush &brush);
        // void setFG(const QBrush &brush);
        // void setBG(const QBrush &brush);
        void updateColor();
        // void changeTypeColor(TDualColorButton::ColorSpace s);
        void syncHsv(int h, int s, int v);
        void setHS(int h, int s);
        void setColorSpace(int type);
        void updateColorFromPalette(const QBrush& brush);
        void updateColorFromDisplay(const QBrush& brush);
        void updateColorSpace(TDualColorButton::ColorSpace space);
        void updateGradientColor(const QBrush &brush);
        void switchColors();
        void resetColors();
        void updateColorType(int index);
		
    signals:
        void paintAreaEventTriggered(const TupPaintAreaEvent *event);
};

#endif

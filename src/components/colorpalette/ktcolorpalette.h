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

#ifndef KTCOLORPALETTE_H
#define KTCOLORPALETTE_H

#include "ktmodulewidgetbase.h"
#include "kdualcolorbutton.h"

class KTColorPalette;

/**
 * @author Jorge Cuadrado <kuadrosx@toonka.com>
*/
class KTColorPalette : public KTModuleWidgetBase
{
    Q_OBJECT
    public:
        enum TypeBrush{ Solid = 0, Gradient };
        KTColorPalette(QWidget *parent = 0);
        ~KTColorPalette();
        //FIXME: change this for QBrush
        QPair<QColor, QColor> color();
        void parsePaletteFile(const QString &file);

    private:
        struct Private;
        Private *const k;

    private:
        void setupButtons();
        void setupChooserTypeColor();
        void setupGradientManager();
        void setupDisplayColor();

    protected:
        void mousePressEvent(QMouseEvent * e);

    public slots:
        void setColor(const QBrush &brush);
        void setFG(const QBrush &brush);
        void setBG(const QBrush &brush);
        void updateColor();
        void changeTypeColor(KDualColorButton::DualColor s);
        void syncHsv(int h, int s, int v);
        void setHS(int h, int s);
        void changeBrushType(const QString&);
		
    signals:
        void paintAreaEventTriggered(const KTPaintAreaEvent *event);
};

#endif

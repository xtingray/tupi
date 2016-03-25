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

#ifndef TUPCOLORPALETTE_H
#define TUPCOLORPALETTE_H

#include "tglobal.h"
#include "tupmodulewidgetbase.h"
#include "ticon.h"
#include "timagebutton.h"
#include "tcolorcell.h"
#include "tconfig.h"
#include "tuppaintareaevent.h"
#include "tupcolorform.h"
#include "tupviewcolorcells.h"
#include "tupcolorpicker.h"
#include "tslider.h"
#include "tupgradientcreator.h"
#include "tvhbox.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QToolTip>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>
#include <QMenu>
#include <QTabWidget>

/**
 * @author Jorge Cuadrado
*/

class TUPI_EXPORT TupColorPalette : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        enum BrushType { Solid = 0, Gradient };

        TupColorPalette(QWidget *parent = 0);
        ~TupColorPalette();

        // SQA: change this for QBrush
        QPair<QColor, QColor> color();
        void parsePaletteFile(const QString &file);

    public slots:
        void init();

    private slots:
        void updateColorMode(TColorCell::FillType flag);
        void initBg();
        void setColorOnAppFromHTML(const QBrush &brush);
        void updateColorFromHTML();
        void updateBgColorFromHTML();
        void syncColor(const QColor &color);
        void setHS(int h, int s);
        void updateColorFromPalette(const QBrush& brush);
        void updateColorFromDisplay(const QBrush& brush);
        void updateGradientColor(const QBrush &brush);
        void switchColors();
        void updateColorType(int index);
		
    signals:
        void bgColorChanged(const QColor color);
        void paintAreaEventTriggered(const TupPaintAreaEvent *event);

    private:
        void setupButtons();
        void setupColorDisplay();
        void setupMainPalette();
        void setupColorChooser();
        void setupGradientManager();
        void setGlobalColors(const QBrush &brush);
        void updateLuminancePicker(const QColor &color);

        struct Private;
        Private *const k;
};

#endif

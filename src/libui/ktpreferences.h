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

#ifndef KTPREFERENCES_H
#define KTPREFERENCES_H

/**
 * @file ktpreferences.h
 * @brief Include this file if you need the class KTPreferences
 */

#include <qlineedit.h>
#include <qlabel.h>
#include <qradiobutton.h>

#include "kfontchooser.h"
#include "ktabdialog.h"
#include "kconfigurationdialog.h"

#include "ktthemeselector.h"
#include "ktglobal.h"

class ColorSchemePref;
class KTPaintAreaConfig;

class KTPreferences : public TConfigurationDialog
{
    Q_OBJECT;
    
    public:
        KTPreferences(QWidget *parent);
        ~KTPreferences();
        
    private:
        class GeneralPage;
        class FontPage;
        
        KTThemeSelector *m_themeSelector;
        FontPage *m_fontChooser;
        GeneralPage *m_generalPage;
        KTPaintAreaConfig *m_drawingAreaProperties;
        
    public slots:
        void apply();
        void ok();

    signals:
        void timerChanged();
};

#endif


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

#include "tuppreferencesdialog.h"
#include "tapplicationproperties.h"
#include "tupgeneralpreferences.h"
#include "tuppaintareapreferences.h"
#include "tupthemepreferences.h"

/**
 * This class handles the preferences dialog for Tupi.
 * Here is where the forms of the Tupi preferences dialog are defined.
 * @author David Cuadrado
*/

struct TupPreferencesDialog::Private
{
    TupGeneralPreferences *general;
    TupThemePreferences *theme;
    TupPaintAreaPreferences *workspace;
};

TupPreferencesDialog::TupPreferencesDialog(QWidget *parent) : TConfigurationDialog(parent), k(new Private)
{
    setWindowTitle(tr("Tupi Preferences"));

    k->general = new TupGeneralPreferences;
    addPage(k->general, tr("General"), QPixmap(THEME_DIR + "icons/tupi_general_preferences.png"));

    k->theme = new TupThemePreferences;
    addPage(k->theme, tr("Theme"), QPixmap(THEME_DIR + "icons/tupi_theme_preferences.png"));

    k->workspace = new TupPaintAreaPreferences;
    addPage(k->workspace, tr("Workspace"), QIcon(THEME_DIR + "icons/tupi_workspace_preferences.png"));

    setCurrentItem(0);
}

TupPreferencesDialog::~TupPreferencesDialog()
{
}

void TupPreferencesDialog::apply()
{
    k->general->saveValues();
    k->theme->saveValues();
    k->workspace->saveValues();

    accept();
}

QSize TupPreferencesDialog::sizeHint() const
{
    return QSize(600, 430);
}

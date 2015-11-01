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

#ifndef TUPTHEMESELECTOR_H
#define TUPTHEMESELECTOR_H

#include "tglobal.h"
#include "themedocument.h"
#include "tvhbox.h"
#include "tapplicationproperties.h"

#include <QMap>
#include <QTreeWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QColorDialog>
#include <QLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QGridLayout>
#include <QButtonGroup>

class QCheckBox;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupThemeSelector : public TVHBox
{
    Q_OBJECT

    public:
        TupThemeSelector(QWidget *parent = 0);
        ~TupThemeSelector();
        
        ThemeDocument document() const;
        QString lastFile() const;
        bool applyColors() const;
        void setPalette(const QPalette &) {};
        
    private slots:
        void chooseGeneralColor(QAbstractButton *);
        void chooseEffectsColor(QAbstractButton *);
        void chooseSelectionsColor(QAbstractButton *);
        void chooseTextEffectsColor(QAbstractButton *);
        void saveSchema();
        void loadSchemaFromListView(QTreeWidgetItem *, int column);
        
    private:
        void setupChooseColor();
        void loadSchemes();
        
    private:
        QGroupBox *m_general;
        ThemeKey m_generalSection;
        QButtonGroup m_generalButtonGroup;
        
        QGroupBox *m_effects;
        ThemeKey m_effectsSection;
        QButtonGroup m_effectsButtonGroup;
        
        QGroupBox *m_selections;
        ThemeKey m_selectionsSection;
        QButtonGroup m_selectionsButtonGroup;
        
        QGroupBox *m_textEffects;
        ThemeKey m_textEffectsSection;
        QButtonGroup m_textEffectsButtonGroup;
        
        QTreeWidget *m_allSchemes;
        QCheckBox *m_useColors;
        
        QString m_lastFile;
};

#endif

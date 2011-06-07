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

#ifndef KOPTIONALDIALOG_H
#define KOPTIONALDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include "tglobal.h"

/**
 * @if english
 * This class represents a dialog that is optional
 * @elseif spanish
 * Esta clase representa un dialogo que es opcional
 * @endif
 * 
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_GUI_EXPORT KOptionalDialog : public QDialog
{
    Q_OBJECT
    public:
        /**
         * @if english
         * Default constructor
         * @elseif spanish
         * Constructor por defecto
         * @endif
         * @param text 
         * @param title 
         * @param parent 
         * @return 
         */
        KOptionalDialog(const QString &text, const QString &title = 0, QWidget *parent = 0);

        /**
         * Destructor
         * @return 
         */
        ~KOptionalDialog();
        
        /**
         * @if english
         * This function returns true when the user wants to see the dialog again
         * @elseif spanish
         * Esta funcion retorna true cuando el usuario quiere volver a ver el dialogo
         * @endif
         * @return 
         */
        bool shownAgain();
        
    private:
        QBoxLayout *m_layout;
        QCheckBox *m_checkBox;
};

#endif

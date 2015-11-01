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

#ifndef TEDITSPINBOX_H
#define TEDITSPINBOX_H

#include "tglobal.h"

#include <QHBoxLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>

/**
 * @if english
 * This class represents a spinbox editable
 * @elseif spanish
 * Esta clase representa un spinbox editable
 * @endif
 * @author Jorge Cuadrado
*/

class T_GUI_EXPORT TEditSpinBox : public QGroupBox
{
    Q_OBJECT

    public:
        /**
         * @if english
         * Default constructor
         * @elseif spanish
         * Constructor por defecto
         * @endif
         * @param value 
         * @param valueMin 
         * @param valueMax 
         * @param step 
         * @param text 
         * @param parent 
         * @param name 
         * @return 
         */
        TEditSpinBox(int value, int valueMin, int valueMax, int step, QString text, QWidget *parent = 0, const char *name = 0);
        ~TEditSpinBox();
        /**
         * @if english
         * Set a range
         * @elseif spanish
         * Pone un rango
         * @endif
         * @param min 
         * @param max 
         */
        void setRange(int min, int max);
        /**
         * @if english
         * Returns the current value
         * @elseif spanish
         * Retorna el valor actual
         * @endif
         * @return 
         */
        int value();
        
    private:
        QSlider *m_slider;
        QSpinBox *m_spin;
        void setupConnects();
    
    public slots:
        /**
         * @if english
         * Sets the actual value
         * @elseif spanish
         * Pone el valor actual
         * @endif
         * @param value 
         */
        void setValue(int value);
        
    signals:
        /**
         * @if english
         * This signal is emitted when value is changed
         * @elseif spanish
         * Este signal es emitido cuando el valor cambia
         * @endif
         * @param  
         */
        void valueChanged(int);
};

#endif

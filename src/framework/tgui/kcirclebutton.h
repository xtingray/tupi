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

#ifndef CCBUTTON_H
#define CCBUTTON_H
/**
 * @file kcirclebutton.h
 * Include this file if you need the class KCircleButton
 */
#include "tglobal.h"
#include <QPushButton>
#include <QStyleOptionButton>

/**
 * @short The KCircleButton class provides a circular button
 * @author David Cuadrado
*/

class K_GUI_EXPORT KCircleButton : public QPushButton
{
    Q_OBJECT
    public:
        /**
         * Constructs a KCircleButton
         */
        KCircleButton(int diameter, bool animate = true, QWidget *parent = 0);
        /**
         * Destructor
         */
        ~KCircleButton();
        
        QStyleOptionButton styleOption() const;
        QSize sizeHint() const;
        
    protected:
        /**
         * Paints the button
         * @param e 
         */
        void paintEvent(QPaintEvent *e);
        
        /**
         * Init animation
         */
        void enterEvent(QEvent *);
        /**
         * End animation
         */
        void leaveEvent(QEvent *);
        
        /**
         * Creates mask of button
         */
        virtual void paintMask();
        
    private slots:
        void animate();
        
    private:
        QPixmap m_mask;
        QPixmap m_pix;
        int m_diameter : 22;
        
        class Animator;
        Animator *m_animator;
};

#endif

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

#ifndef CCBAR_H
#define CCBAR_H

/**
 * @file kcirclebuttonbar.h
 * Include this file if you need the class KCircleButtonBar
 */

#include <qframe.h>
#include <QPixmap>
#include <QPolygon>
#include <QBoxLayout>
#include <QBitmap>
#include <QPainter>
#include <QPainterPath>
#include <QImage>

#include "kcirclebutton.h"
#include "tglobal.h"

class QPainterPath;

/**
 * @short The KCircleButtonBar class provides a circular button bar
 * @author David Cuadrado <krawek@gmail.com>
*/

class K_GUI_EXPORT KCircleButtonBar : public QFrame
{
    public:
        /**
         * Constructs a KCircleButtonBar
         */
        KCircleButtonBar(int radio = 40, QWidget *parent= 0);

        /**
         * Destructor
         */
        ~KCircleButtonBar();

        /**
         * Add button in KCircleButtonBar
         * @param pix image of new KCircleButton
         * @return pointer of new KCircleButton
         */
         KCircleButton *addButton(const QPixmap &pix);

    private:
        QPixmap m_mask;
        int m_radio;
        int m_buttonCount;

        QBoxLayout *m_layout;
        int m_offset;

    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);

        QPainterPath m_border;
};

#endif

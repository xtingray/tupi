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

#ifndef TANIMWIDGET_H
#define TANIMWIDGET_H
/**
 * @file tanimwidget.h
 * @brief Include this file if you need the class TAnimWidget
 */

#include <QPixmap>
#include <QHideEvent>
#include <QShowEvent>
#include <QList>
#include <QWidget>

#include "tglobal.h"

typedef QList<QPixmap> ListOfPixmaps;

/**
 * @if english
 * @short The class TAnimWidget provides a widget containing a simple animation of moving text or image sequences  
 * @elseif spanish
 * @short La clase TAnimWidget provee de un widget que hace una simple animacion, de un texto o una secuencia de imagenes.
 * 
 * @Author David Cuadrado
 * @endif
 */

class T_GUI_EXPORT TAnimWidget : public QWidget
{
    public:
        enum Type { AnimText = 0, AnimPixmap };

        /**
          * Builds a TAnimWidget object with a background, an animated text and a parent widget
          * @param px background
          * @param text text to animate 
          * @param parent 
          */
        TAnimWidget(const QPixmap &px, const QString &text, QWidget *parent = 0);

        /**
          * Builds a TAnimWidget object with a background, an image list and a parent widget
          * @param lop images to animate 
          * @param parent 
          */
        TAnimWidget(ListOfPixmaps lop, QWidget *parent = 0);

        /**
         * Destructor
         */
        ~TAnimWidget();

        /**
         * set an image as animation background
         * @param px background 
         */
        void setBackgroundPixmap(const QPixmap &px);

    protected:
        /**
         * Starts animation
         */
        void showEvent(QShowEvent * e);

        /**
         * Stops animation 
         */
        void hideEvent(QHideEvent * e);

    protected:
        /**
         * Handles animation flow
         */
        void timerEvent(QTimerEvent *e);

        /**
         * Draws animation
         */
        void paintEvent(QPaintEvent *e);

    private:
        Type m_type;
        class Controller;
        Controller *m_controller;
        QPixmap m_background;
        QString m_text;
        QRectF m_textRect;

        ListOfPixmaps m_pixmaps;
        int m_pixmapIndex;
        int m_counter;
        int m_lines;
        int fontSize;
        int m_end;
};

#endif

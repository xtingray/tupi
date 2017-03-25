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

#ifndef TDUALCOLORBUTTON_H
#define TDUALCOLORBUTTON_H

#include "tglobal.h"

// class QBitmap;

#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QPixmap>
#include <QPaintEvent>
#include <QDropEvent>
#include <QBrush>
#include <QWidget>
#include <qdrawutil.h>
#include <QPainter>
#include <QBitmap>
#include <QApplication>

/**
 * @note This class was adapted to Tupi by David Cuadrado
 * 
 * @short A widget for selecting two related colors.
 *
 * TDualColorButton allows the user to select two cascaded colors (usually a
 * foreground and background color). Other features include drag and drop
 * from other KDE color widgets, a reset to black and white control, and a
 * swap colors control.
 *
 * When the user clicks on the foreground or background rectangle the
 * rectangle is first sunken and the currentChanged() signal is emitted.
 * Further clicks will present a color dialog and emit either the fgChanged()
 * or bgChanged() if a new color is selected.
 *
 * Note: With drag and drop when dropping a color the current selected color
 * will be set, while when dragging a color it will use whatever color
 * rectangle the mouse was pressed inside.
 *
 * @author Daniel M. Duley <mosfet@kde.org>
 */

class T_GUI_EXPORT TDualColorButton : public QWidget
{
    Q_OBJECT

    Q_ENUMS(ColorSpace)
    Q_PROPERTY(QBrush foreground READ foreground WRITE setForeground)
    Q_PROPERTY(QBrush background READ background WRITE setBackground)
    Q_PROPERTY(QBrush currentColor READ currentColor WRITE setCurrentColor STORED false DESIGNABLE false)
    Q_PROPERTY(ColorSpace current READ current WRITE setCurrent)

    public:

        enum ColorSpace {Foreground, Background};
        /**
        * Constructs a new TDualColorButton using the default black and white
        * colors.
        *
        */

        /// KDE4 remove name argument
        TDualColorButton(QWidget *parent=0);

        /**
        * Constructs a new TDualColorButton with the supplied foreground and
        * background colors.
        */
        /// KDE4 remove name argument
        TDualColorButton(const QBrush &fgColor, const QBrush &bgColor,
                           QWidget *parent=0);
        ~TDualColorButton();

        /**
        * Returns the current foreground color.
        */
        QBrush foreground() const;

        /**
        * Returns the current background color.
        */
        QBrush background() const;

        /**
        * Returns the current color item selected by the user.
        */
        ColorSpace current() const;

        /**
        * Returns the color of the selected item.
        */
        QBrush currentColor() const;

        /**
        * Returns the minimum size needed to display the widget and all its
        * controls.
        */
        virtual QSize sizeHint() const;

    public slots:
        /**
         * Sets the foreground color.
         */
        void setForeground(const QBrush &c);

        /**
         * Sets the background color.
         */
        void setBackground(const QBrush &c);

        /**
         * Sets the current selected color item.
         */
        void setCurrent(ColorSpace s);

        /**
         * Sets the color of the selected item.
         */
        void setCurrentColor(const QBrush &c);

    signals:
        // void fgChanged(const QBrush &c);
        // void bgChanged(const QBrush &c);

        /**
         * Emitted when the user changes the current color selection.
         */
        // void currentChanged(TDualColorButton::ColorSpace s);
        void selectionChanged(TDualColorButton::ColorSpace space);
        void switchColors();
        void resetColors();

    protected:
        /**
         * Sets the supplied rectangles to the proper size and position for the
         * current widget size. You can reimplement this to change the layout
         * of the widget. Restrictions are that the swap control will always
         * be at the top right, the reset control will always be at the bottom
         * left, and you must leave at least a 14x14 space in those corners.
         */
        virtual void metrics(QRect &fgRect, QRect &bgRect);
        virtual void paintEvent(QPaintEvent *ev);
        virtual void mousePressEvent(QMouseEvent *ev);

    private:
        struct Private;
        Private *const k;
};

#endif

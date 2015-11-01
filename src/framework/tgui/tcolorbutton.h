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

#ifndef TCOLORBUTTON_H
#define TCOLORBUTTON_H

#include "tglobal.h"

#include <QAbstractButton>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QColorDialog>
#include <QPalette>
#include <QCursor>
#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QDrag>
#include <QStyle>
#include <QStyleOptionButton>
#include <QPixmap>
#include <QMouseEvent>
#include <QMimeData>

class QColor;

class T_GUI_EXPORT TColorButton : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor);

    public:
        TColorButton(QWidget* parent = 0);
        ~TColorButton();

        void setColor(const QColor&);
        QColor color() const;

        QSize sizeHint() const;
        QSize minimumSizeHint() const;
        void setPalette(const QPalette &);

    public slots:
        virtual void showEditor();

    signals:
        void colorChanged(const QColor &color);

    protected:
        void paintEvent(QPaintEvent *e);
        void mousePressEvent(QMouseEvent* e);
        void mouseMoveEvent(QMouseEvent* e);
        void dragEnterEvent(QDragEnterEvent* e);
        void dragMoveEvent(QDragMoveEvent* e);
        void dropEvent(QDropEvent* e);

    private:
        QColor m_color;
        QPoint m_position;
};

#endif //TCOLORBUTTON_H

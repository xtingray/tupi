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

#ifndef TUPPAINTAREASTATUS_H
#define TUPPAINTAREASTATUS_H

#include <QBrush>
#include <QSize>
#include <QPaintEvent>
#include <QWidget>

/**
 * This class defines the options panel in the bottom of the paint area.
 * Controls for Rotation, Antialising and OpenGL
 * @author David Cuadrado
*/

class TupColorWidget : public QWidget
{
    Q_OBJECT

    public:
        TupColorWidget() : m_brush(Qt::transparent) {};
        ~TupColorWidget() {};
        void setBrush(const QBrush &brush);
        QSize sizeHint() const;
        QColor color();

    protected:
        void paintEvent(QPaintEvent *);
        void mousePressEvent(QMouseEvent * event);

    signals:
        void clicked();        

    private:
        QBrush m_brush;
};

#endif

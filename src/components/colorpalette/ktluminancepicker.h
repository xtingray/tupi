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

#ifndef KTLUMINACEPICKER_H
#define KTLUMINACEPICKER_H

#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QWidget>

class KTLuminancePicker : public QWidget
{
    Q_OBJECT

    public:
        KTLuminancePicker(QWidget* parent=0);
        ~KTLuminancePicker();
        int value();

    public slots:
        void setColor(int h, int s, int v);
        void setColor(int h, int s);
        void setValue(int v);

    signals:
        void newHsv(int h, int s, int v);

    protected:
        void paintEvent(QPaintEvent*);
        void mouseMoveEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);

    private:
        enum { foff = 3, coff = 4 }; //frame and contents offset

    struct Private;
    Private *const k;
    int y2val(int y);
    int val2y(int val);
};

#endif


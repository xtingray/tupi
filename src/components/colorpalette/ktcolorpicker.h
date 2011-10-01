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

#include <QFrame>

#ifndef KTCOLORPICKER_H
#define KTCOLORPICKER_H

class KTColorPicker : public QFrame
{
    Q_OBJECT

    public:
        KTColorPicker(QWidget* parent);
        ~KTColorPicker();
        int hue();
        int saturation();

    public slots:
        void setColor(int h, int s);
        void setH(int h);
        void setS(int s);

    signals:
        void newColor(int h, int s);

    protected:
        QSize sizeHint() const;
        void paintEvent(QPaintEvent*);
        void mouseMoveEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);

    private:
        struct Private;
        Private *const k;

        QPoint colorPoint();
        int huePoint(const QPoint &pt);
        int saturationPoint(const QPoint &pt);
        void setColor(const QPoint &pt);
};

#endif


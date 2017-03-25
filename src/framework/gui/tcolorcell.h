/***************************************************************************
 *   Project TUPI: 2D Magic                                                *
 *   Component: tupi.mobile                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *                                                                         *
 *   Developers:                                                           *
 *   2012:                                                                 *
 *    Gustavo Gonzalez / @xtingray                                         *
 *    Andres Calderon / @andresfcalderon                                   *
 *    Antonio Vanegas / @hpsaturn                                          *
 *                                                                         *
 *   Tupi is a fork of the KTooN project                                   *
 *   KTooN's versions:                                                     *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2012 Mae Floresta - http://www.maefloresta.com          *
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

#ifndef TCOLORCELL_H
#define TCOLORCELL_H

#include "tglobal.h"

#include <QBrush>
#include <QSize>
#include <QPaintEvent>
#include <QWidget>

class TUPI_EXPORT TColorCell : public QWidget
{
    Q_OBJECT

    public:
        enum FillType{Contour = 0, Inner, Background, PreviousFrames, NextFrames, Layers};
        TColorCell(FillType index, const QBrush &brush, const QSize &size);
        ~TColorCell();
        QSize sizeHint() const;
        QBrush brush();
        QColor color();
        void setEnabled(bool isEnabled);
        void setChecked(bool isChecked);
        bool isChecked();
        void setBrush(const QBrush &brush);

    protected:
        void paintEvent(QPaintEvent *painter);
        void mousePressEvent(QMouseEvent *event);

    signals:
        void clicked(TColorCell::FillType index);

    private:
        struct Private;
        Private *const k;
};

#endif

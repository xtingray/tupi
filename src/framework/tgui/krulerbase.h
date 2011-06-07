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

#ifndef RULER_H
#define RULER_H

#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMenu>
#include <QPolygonF>
#include <QFrame>
#include "tglobal.h"

#define UNITCOUNT 5

class KRulerBase;

/**
 * @author Jorge Cuadrado
*/

class K_GUI_EXPORT KRulerBase : public QFrame
{
    Q_OBJECT
    
    public:

    /***
         enum Unit {
             SC_POINTS      = 0,
             SC_PT          = 0,
             SC_MILLIMETERS = 1,
             SC_MM          = 1,
             SC_INCHES      = 2,
             SC_IN          = 2,
             SC_PICAS       = 3,
             SC_P           = 3,
             SC_CENTIMETERS = 4,
             SC_CM          = 4,
             SC_CICERO      = 5,
             SC_C           = 5
         };
    */
        
        KRulerBase(Qt::Orientation orientation=Qt::Horizontal, QWidget *parent = 0);
        virtual ~KRulerBase();
        
        // const double unitGetRatioFromIndex(const int index);
        // const double pts2mm(double pts);
        // const double mm2pts(double mm);

        virtual void drawScale(QPainter *painter);
        Qt::Orientation orientation();
        
        int separation() const;
        double scaleFactor() const;
        QPointF zero() const;
        
        void translateArrow(double dx, double dy);
        
    public slots:

        void setZeroAt(const QPointF & pos);
        void scale(double factor);
        
    private:
        enum { ChangeScaleToFive, ChangeScaleToTen  };
        
        struct Private;
        Private *const k;
        
    signals:
        void displayMenu(KRulerBase *, QPoint pos);
        
    protected:
        virtual void paintEvent(QPaintEvent * e);
        virtual void resizeEvent(QResizeEvent *);
        virtual void mouseMoveEvent(QMouseEvent * e);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual QSize sizeHint() const;
        
    public slots:
        virtual void movePointers(const QPointF &pos) = 0;
        void setSeparation(int sep);
        void setDrawPointer(bool yes = true);
        void slide(int value);
        
        virtual void showMenu(KRulerBase *, QPoint pos);
        
    private slots:
        void changeScaleTo5pts();
        void changeScaleTo10pts();
};

#endif

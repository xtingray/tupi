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

#include "trulerbase.h"
#include "tdebug.h"

#include <QMouseEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QPaintEvent> 
#include <QPainter>

struct TRulerBase::Private
{
    int position;
    int separation;
    int width; 
    int height;
    bool drawPointer;

    Qt::Orientation orientation;

    QMenu *menu;
    QPointF zero;
    QPolygonF pArrow;

    double scaleFactor;
};

TRulerBase::TRulerBase(Qt::Orientation orientation, QWidget *parent) : QFrame(parent), k(new Private)
{
    k->position = 0;
    k->orientation = orientation;
    k->drawPointer = false;
    k->separation = 10;

    k->zero = QPointF(0,0);
    k->pArrow = QPolygonF(3);

    k->scaleFactor = 1.0;

    if (k->orientation == Qt::Horizontal) {

        setMaximumHeight(20);
        setMinimumHeight(20);

        k->width = width();
        k->height = height();

        k->pArrow << QPointF(0.0, 0.0);
        k->pArrow << QPointF(5.0, 5.0);
        k->pArrow << QPointF(10.0, 0.0);

        k->pArrow.translate(0,13);

    } else {

        setMaximumWidth(20);
        setMinimumWidth(20);

        k->width = height();
        k->height =  width();

        k->pArrow << QPointF(0.0, 0.0);
        k->pArrow << QPointF(5.0, 5.0);
        k->pArrow << QPointF(0.0, 10.0);

        k->pArrow.translate(13,0);
    }

    setMouseTracking(true);

    connect(this, SIGNAL(displayMenu(TRulerBase *, QPoint)), this, SLOT(showMenu(TRulerBase *, QPoint)));

    k->menu = new QMenu(this);

    QAction *to5 = k->menu->addAction(tr("Change scale to 5..."));
    QAction *to10 = k->menu->addAction(tr("Change scale to 10..."));

    connect(to5, SIGNAL(triggered()), this, SLOT(changeScaleTo5pts()));
    connect(to10, SIGNAL(triggered()), this, SLOT(changeScaleTo10pts()));
}


TRulerBase::~TRulerBase()
{
    delete k;
}

void TRulerBase::paintEvent ( QPaintEvent * )
{
    QPainter p(this);

    if (k->orientation == Qt::Vertical)
        p.scale(1.0, k->scaleFactor);
    else
        p.scale(k->scaleFactor, 1.0);

    drawScale(&p);
    p.setBrush(palette ().color(QPalette::Foreground));

    p.save();

    p.drawConvexPolygon(k->pArrow);
    p.restore();

    p.end();
	
}

void TRulerBase::drawScale(QPainter *painter)
{
    painter->save();
    QFont tfont(font().family(), 7);
    QFontMetrics fm(tfont);
    painter->setFont(tfont);

    int fact = 1;

    if (k->orientation == Qt::Horizontal) {
        painter->translate(k->zero.x(), 0);
        painter->drawLine(-390, height()-1, width(), height()-1);
    } else {
        painter->drawLine(width()-1, 0, width()-1, height());
        fact = -1;
        painter->translate(0, k->zero.y());
        painter->rotate(90);
    }

    int ypos = k->height*fact;
    int ytext = k->height/2;

    for (int i = 0; i < k->width; i += k->separation) {
         QSize sizeFont = fm.size(Qt::TextSingleLine, QString::number(i));
         if (i % 100 == 0) { // FIX ME
             painter->drawLine (i, ypos, i, 0);
             int dx = i + 3;
             if (k->orientation == Qt::Vertical)
                 painter->drawText(QPoint(dx, ytext - sizeFont.height()), QString::number(i));
             else
                 painter->drawText(QPoint(dx, ytext), QString::number(i));
         } else {
                 painter->drawLine (i, ypos, i, ypos - k->height/4*fact);
         }
    }

   for (int i = 0; i > -390 ; i -= k->separation) {
         //cout << "Var: " << i << endl;
         QSize sizeFont = fm.size (Qt::TextSingleLine, QString::number(i));
         if (i % 100  == 0) { // FIX ME
             painter->drawLine(i, ypos, i, 0);
             int dx = i + 3;
             if (k->orientation == Qt::Vertical)
                 painter->drawText(QPoint(dx, ytext - sizeFont.height()), QString::number(i));
             else
                 painter->drawText(QPoint(dx, ytext), QString::number(i));
         } else {
             painter->drawLine(i, ypos, i, ypos - k->height/4*fact);
         }
    }

    painter->restore();
}

void TRulerBase::resizeEvent(QResizeEvent *)
{
    if (k->orientation == Qt::Horizontal) {
        k->width = width();
        k->height = height();
    } else if (k->orientation == Qt::Vertical) {
        k->width = height();
        k->height =  width();
    }

    update();
}

void TRulerBase::mouseMoveEvent(QMouseEvent * e)
{
    if (k->drawPointer)
        movePointers(e->pos());
}

void TRulerBase::setDrawPointer(bool yes)
{
    k->drawPointer = yes;
    update();
}

void TRulerBase::setSeparation(int sep)
{
    if (sep > 0 && sep <= 10000) {
        k->separation = sep;
        update();
    } else {
        tError() << "I can't assign separation : " << sep << endl;
    }
}

void TRulerBase::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        emit displayMenu(this, e->globalPos());
}

Qt::Orientation TRulerBase::orientation()
{
    return k->orientation;
}

int TRulerBase::separation() const
{
    return k->separation;
}

double TRulerBase::scaleFactor() const
{
    return k->scaleFactor;
}

void TRulerBase::showMenu(TRulerBase *ruler, QPoint pos)
{
    if (ruler)
        k->menu->popup(pos);
}

void TRulerBase::slide(int value)
{
    int distance = -value + k->height;

    if (k->orientation == Qt::Horizontal)
        move(distance, pos().y());
    else
        move(pos().x(), distance);
}

QPointF TRulerBase::zero() const
{
    return k->zero;
}

void TRulerBase::translateArrow(double dx, double dy)
{
    k->pArrow.translate(dx, dy);
}

void TRulerBase::setZeroAt(const QPointF & pos)
{
    k->zero = pos;
    update();
}

void TRulerBase::scale(double factor)
{
    k->scaleFactor = factor;
    update();
}

QSize TRulerBase::sizeHint() const
{
    int distance = k->width/3;

    if (k->orientation == Qt::Horizontal)
        return QSize(distance, height());

    return QSize(width(), distance);
}

void TRulerBase::changeScaleTo5pts()
{
    setSeparation(5);
}

void TRulerBase::changeScaleTo10pts()
{
    setSeparation(10);
}

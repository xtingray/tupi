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

#include "krulerbase.h"
#include "tdebug.h"

#include <QMouseEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QPaintEvent> 
#include <QPainter>

struct KRulerBase::Private
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

KRulerBase::KRulerBase(Qt::Orientation orientation, QWidget *parent) : QFrame(parent), k(new Private)
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

    setMouseTracking (true);

    connect(this, SIGNAL(displayMenu(KRulerBase *, QPoint)), this, SLOT(showMenu(KRulerBase *, QPoint)));

    k->menu = new QMenu(this);

    QAction *to5 = k->menu->addAction( tr("Change scale to 5..."));
    QAction *to10 = k->menu->addAction( tr("Change scale to 10..."));

    connect(to5, SIGNAL(triggered()), this, SLOT(changeScaleTo5pts()));
    connect(to10, SIGNAL(triggered()), this, SLOT(changeScaleTo10pts()));
}


KRulerBase::~KRulerBase()
{
    delete k;
}

void KRulerBase::paintEvent ( QPaintEvent * )
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

void KRulerBase::drawScale(QPainter *painter)
{
    painter->save();
    QFont kfont(font().family(), 7);
    QFontMetrics fm(kfont);
    painter->setFont(kfont);

    int fact = 1;
    int init;

    if (k->orientation == Qt::Horizontal) {
        painter->translate(k->zero.x(), 0);
        init = (int)k->zero.x();
        painter->drawLine(-390, height()-1, width(), height()-1);
    } else {
        painter->drawLine(width()-1, 0, width()-1, height());
        fact = -1;
        painter->translate(0, k->zero.y());
        painter->rotate(90);
        init = (int)k->zero.y();
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

void KRulerBase::resizeEvent(QResizeEvent *)
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

void KRulerBase::mouseMoveEvent(QMouseEvent * e)
{
    if (k->drawPointer)
        movePointers(e->pos());
}

void KRulerBase::setDrawPointer(bool yes)
{
    k->drawPointer = yes;
    update();
}

void KRulerBase::setSeparation(int sep)
{
    if (sep > 0 && sep <= 10000) {
        k->separation = sep;
        update();
    } else {
        tError() << "I can't assign separation : " << sep << endl;
    }
}

void KRulerBase::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        emit displayMenu(this, e->globalPos());
}

Qt::Orientation KRulerBase::orientation()
{
    return k->orientation;
}

int KRulerBase::separation() const
{
    return k->separation;
}

double KRulerBase::scaleFactor() const
{
    return k->scaleFactor;
}

void KRulerBase::showMenu(KRulerBase *ruler, QPoint pos)
{
    if (ruler)
        k->menu->popup(pos);
}

void KRulerBase::slide(int value)
{
    int distance = -value + k->height;

    if (k->orientation == Qt::Horizontal)
        move(distance, pos().y());
    else
        move(pos().x(), distance);
}

QPointF KRulerBase::zero() const
{
    return k->zero;
}

void KRulerBase::translateArrow(double dx, double dy)
{
    k->pArrow.translate(dx, dy);
}

void KRulerBase::setZeroAt(const QPointF & pos)
{
    k->zero = pos;
    update();
}

void KRulerBase::scale(double factor)
{
    k->scaleFactor = factor;
    update();
}

QSize KRulerBase::sizeHint() const
{
    int distance = k->width/3;

    if (k->orientation == Qt::Horizontal)
        return QSize(distance, height());

    return QSize(width(), distance);
}

void KRulerBase::changeScaleTo5pts()
{
    setSeparation(5);
}

void KRulerBase::changeScaleTo10pts()
{
    setSeparation(10);
}

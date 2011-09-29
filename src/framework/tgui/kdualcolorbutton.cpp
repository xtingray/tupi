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

#include "kdualcolorbutton.h"
#include "tdebug.h"

#include "kcolorarrow.xpm"
#include "kcolorreset.xpm"

#include <qdrawutil.h>
#include <QPainter>
#include <QBitmap>
#include <QDragEnterEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QDropEvent>
#include <QApplication>

KDualColorButton::KDualColorButton(QWidget *parent) : QWidget(parent)
{
    arrowBitmap = new QPixmap((const char **)dcolorarrow_bits);
    arrowBitmap->setMask(*arrowBitmap); // heh

    resetPixmap = new QPixmap((const char **)dcolorreset_xpm);
    fg = QBrush(Qt::black, Qt::SolidPattern);
    bg = QBrush(QColor(0, 0, 0, 0), Qt::SolidPattern);
	
    curColor = Foreground;
    dragFlag = false;
    miniCtlFlag = false;

    if (sizeHint().isValid()) {
        setMinimumSize(sizeHint());
        // setMaximumSize(sizeHint()); // FIXME: Remove if is necessary
    }

    setAcceptDrops(true);
}

KDualColorButton::KDualColorButton(const QBrush &fgColor, const QBrush &bgColor, QWidget *parent) : QWidget(parent)
{
    arrowBitmap = new QPixmap((const char **)dcolorarrow_bits);
    arrowBitmap->setMask(*arrowBitmap);
    resetPixmap = new QPixmap((const char **)dcolorreset_xpm);
    fg = fgColor;
    bg = bgColor;
    curColor = Foreground;
    dragFlag = false;
    miniCtlFlag = false;

    if (sizeHint().isValid()) {
        setMinimumSize(sizeHint());
        // setMaximumSize(sizeHint());
    }

    setAcceptDrops(true);
}

KDualColorButton::~KDualColorButton()
{
    delete arrowBitmap;
    delete resetPixmap;
}

QBrush KDualColorButton::foreground() const
{
    return fg;
}

QBrush KDualColorButton::background() const
{
    return bg;
}

KDualColorButton::ColorSpace KDualColorButton::current() const
{
    return curColor;
}

QBrush KDualColorButton::currentColor() const
{
    return (curColor == Background ? bg : fg);
}

QSize KDualColorButton::sizeHint() const
{
    return QSize(34, 34);
}

void KDualColorButton::setForeground(const QBrush &c)
{
    fg = c;
    update();

    emit fgChanged(fg.color());
}

void KDualColorButton::setBackground(const QBrush &c)
{
    bg = c;
    update();

    emit bgChanged(bg.color());
}

void KDualColorButton::setCurrentColor(const QBrush &c)
{
    if (curColor == Background)
        bg = c;
    else
        fg = c;

    update();
}

void KDualColorButton::setCurrent(ColorSpace s)
{
    curColor = s;
    update();
}

void KDualColorButton::metrics(QRect &fgRect, QRect &bgRect)
{
    fgRect = QRect(0, 0, width()-14, height()-14);
    bgRect = QRect(14, 14, width()-14, height()-14);
}

void KDualColorButton::paintEvent(QPaintEvent *)
{
    QRect fgRect, bgRect;
    QPalette pal = palette();
    QPainter p(this);

    metrics(fgRect, bgRect);
    QBrush defBrush = pal.color(QPalette::Button);

    QBrush bgAdjusted = bg;
    QBrush fgAdjusted = fg;

    QMatrix m;
    m.scale((float)(bgRect.width()) / 100.0, (float)(bgRect.height()) / 100.0);
    bgAdjusted.setMatrix(m);
	
    m.reset();
    m.scale((float)(fgRect.width()) / 100.0, (float)(fgRect.height()) / 100.0);
    fgAdjusted.setMatrix(m);

    qDrawShadeRect(&p, bgRect, pal, curColor == Background, 2, 0, isEnabled() ? & bgAdjusted: &defBrush);
    qDrawShadeRect(&p, fgRect,  pal, curColor == Foreground, 2, 0, isEnabled() ? &fgAdjusted : &defBrush);
    p.setPen(QPen(palette().shadow().color()));

    p.drawPixmap(fgRect.right()+2, 0, *arrowBitmap);
    p.drawPixmap(0, fgRect.bottom()+2, *resetPixmap);
}

/*
void KDualColorButton::dragEnterEvent(QDragEnterEvent *ev)
{
    // ev->accept(isEnabled() && KColorDrag::canDecode(ev));
    Q_UNUSED(ev);
}

void KDualColorButton::dropEvent(QDropEvent *ev)
{
    Q_UNUSED(ev);
      QColor c;
      if (KColorDrag::decode(ev, c)) {
          if (curColor == Foreground) {
              fg.setColor(c);
              emit fgChanged(c);
          } else {
              bg.setColor(c);
              emit(bgChanged(c));
          }
          update();
      }
}
*/

void KDualColorButton::mousePressEvent(QMouseEvent *ev)
{
    QRect fgRect, bgRect;
    metrics(fgRect, bgRect);
    mPos = ev->pos();
    tmpColor = curColor;
    dragFlag = false;

    if (fgRect.contains(mPos)) {
        curColor = Foreground;
        tFatal() << "KDualColorButton::mousePressEvent() - emitting foreground signal!";
        emit selectionChanged(Foreground);
        miniCtlFlag = false;
    } else if (bgRect.contains(mPos)) {
               curColor = Background;
               tFatal() << "KDualColorButton::mousePressEvent() - emitting background signal!";
               emit selectionChanged(Background);
               miniCtlFlag = false;
    } else if (ev->pos().x() > fgRect.width()) {
               // We handle the swap and reset controls as soon as the mouse is
               // is pressed and ignore further events on this click (mosfet).
               QBrush c = fg;
               fg = bg;
               bg = c;
               emit fgChanged(fg);
               emit bgChanged(bg);
               miniCtlFlag = true;
    } else if (ev->pos().x() < bgRect.x()) {
               fg.setColor(Qt::black);
               bg.setColor(QColor(0,0,0,0));
               emit fgChanged(fg);
               emit bgChanged(bg);
               miniCtlFlag = true;
    }

    update();
}

/*
void KDualColorButton::mouseMoveEvent(QMouseEvent* ev)
{
    QWidget::mouseMoveEvent(ev);

    if ((ev->pos() - mPos).manhattanLength() <  QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QPixmap pix(25, 25);

    QRect fgRect, bgRect;
    metrics(fgRect, bgRect);
    QPainter painter(&pix);

    QBrush curr;

    if (fgRect.contains(mPos)) {
        curr = foreground();
    } else if (bgRect.contains(mPos)) {
               curr = background();
    } else {
        return;
    }

    painter.fillRect(pix.rect(), curr);
    painter.drawRect(0, 0, pix.width(), pix.height());
    painter.end();

    QMimeData *mimeData = new QMimeData;
    mimeData->setColorData(curr.color());

    drag->setMimeData(mimeData);
    drag->setPixmap(pix);

    // Qt::DropAction dropAction = 
    drag->start(Qt::MoveAction);
}
*/

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

#include "tclicklineedit.h"

#include <QPainter>

struct TClickLineEdit::Private
{
    QString clickMessage;
    bool drawClickMsg;
};

TClickLineEdit::TClickLineEdit(const QString &msg, QWidget *parent) : QLineEdit(parent), k(new Private)
{
    k->drawClickMsg = true;
    setClickMessage(msg);
    
    setFocusPolicy(Qt::ClickFocus);
    
    setStyleSheet(QString(":enabled { padding-right: %1; }").arg(8));
}

TClickLineEdit::~TClickLineEdit()
{
    delete k;
}

void TClickLineEdit::setClickMessage(const QString &msg)
{
    k->clickMessage = msg;
    repaint();
}

QString TClickLineEdit::clickMessage() const
{
    return k->clickMessage;
}

void TClickLineEdit::setText(const QString &txt)
{
    k->drawClickMsg = txt.isEmpty();
    repaint();
    QLineEdit::setText(txt);
}

void TClickLineEdit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);
    
    QPainter p(this);
    if (k->drawClickMsg == true && !hasFocus()) {
        QPen tmp = p.pen();
        p.setPen(palette().color(QPalette::Disabled, QPalette::Text));
        QRect cr = contentsRect();
        
        cr.adjust(3, 0, 0 ,0);
        p.drawText(cr, Qt::AlignVCenter, k->clickMessage);
        p.setPen( tmp );
    }
}

void TClickLineEdit::focusInEvent(QFocusEvent *ev)
{
    if (k->drawClickMsg == true) {
        k->drawClickMsg = false;
        repaint();
    }
    QLineEdit::focusInEvent(ev);
}

void TClickLineEdit::focusOutEvent(QFocusEvent *ev)
{
    if (text().isEmpty()) {
        k->drawClickMsg = true;
        repaint();
    }
    QLineEdit::focusOutEvent(ev);
}

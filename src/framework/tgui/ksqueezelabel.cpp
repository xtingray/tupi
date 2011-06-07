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

#include "ksqueezelabel.h"

#include <qtooltip.h>
//Added by qt3to4:
#include <QLabel>
#include <QResizeEvent>

#include "tdebug.h"

KSqueezeLabel::KSqueezeLabel(QWidget *parent, const char *name) : QLabel(parent)
{
    setObjectName(name);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

KSqueezeLabel::KSqueezeLabel(const QString &text , QWidget *parent, const char *name)
    : QLabel(parent)
{
    setObjectName(name);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_text = text;
    squeezeText();
}

KSqueezeLabel::~KSqueezeLabel()
{
}

void KSqueezeLabel::squeezeText()
{
    QFontMetrics fm(fontMetrics());
    int labelWidth = size().width();
    int textWidth = fm.width(m_text);
    
    if (textWidth > labelWidth) {
        QString squeezedText = squeezer(m_text, fm, labelWidth);
        QLabel::setText(squeezedText);
        setToolTip(m_text);
    } else {
        QLabel::setText(m_text);
    }
}

QString KSqueezeLabel::squeezer(const QString &s, const QFontMetrics& fm, uint width)
{
    if (s.isEmpty() || uint(fm.width(s)) <= width) {
        return s;
    }
    
    const unsigned int length = s.length();
    if (length == 2) 
        return s;

    const int maxWidth = width - fm.width('.') * 3;

    if (maxWidth <= 0)
        return "...";
    
    unsigned int leftIdx = 0, rightIdx = length;
    unsigned int leftWidth = fm.charWidth(s, leftIdx++);
    unsigned int rightWidth = fm.charWidth(s, --rightIdx);
    
    while (leftWidth + rightWidth < uint(maxWidth)) {
        while (leftWidth <= rightWidth && leftWidth + rightWidth < uint( maxWidth)) {
            leftWidth += fm.charWidth(s, leftIdx++);
        }
        while (rightWidth <= leftWidth && leftWidth + rightWidth < uint( maxWidth)) {
               rightWidth += fm.charWidth(s, --rightIdx);
        }
    }
    
    if (leftWidth > rightWidth) {
        --leftIdx;
    } else {
        ++rightIdx;
    }
     
    rightIdx = length - rightIdx;

    if ((leftIdx == 0 && rightIdx == 1) || (leftIdx == 1 && rightIdx == 0))
        return "...";
    
    return s.left(leftIdx) + "..." + s.right(rightIdx);
}

void KSqueezeLabel::resizeEvent(QResizeEvent *)
{
    squeezeText();
}

void KSqueezeLabel::setAlignment(Qt::Alignment alignment)
{
    QString tmp(m_text);
    QLabel::setAlignment(alignment);
    m_text = tmp;
}

QSize KSqueezeLabel::sizeHint() const
{
    return QSize(contentsRect().width(), QLabel::sizeHint().height());
}

QSize KSqueezeLabel::minimumSizeHint() const
{
    QSize sh = QLabel::minimumSizeHint();
    sh.setWidth(-1);

    return sh;
}

void KSqueezeLabel::setText(const QString &text) 
{
    m_text = text;
    squeezeText();
}

QString KSqueezeLabel::completeText() const
{
    return m_text;
}

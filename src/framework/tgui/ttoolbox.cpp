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

#include "ttoolbox.h"

#include <QPolygon>
#include <QPainter>

TToolBox::TToolBox(QWidget *parent) : QToolBox(parent), m_lastIndex(0)
{
    setFrameStyle(QFrame::StyledPanel);
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(changeIcon(int)));
    createIcon();
}

TToolBox::~TToolBox()
{
}

void TToolBox::createIcon()
{
    QPolygon m_pArrow(3);
    QPixmap pixmap (22,22);
    
    m_pArrow.setPoint(0, 0, 0);
    m_pArrow.setPoint(1, 10, 5);
    m_pArrow.setPoint(2, 0, 10);
    
    QPainter p(&pixmap);
    
    p.fillRect(pixmap.rect(), palette().background());
    p.setBrush(palette().foreground());
    p.translate(pixmap.width()/3, 5);
    p.drawConvexPolygon(m_pArrow);

    p.end();
    
    m_icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
    
    ///////////
    
    QPixmap pixmap2(22,22);
    
    m_pArrow.setPoint(0, 0, 0);
    m_pArrow.setPoint(1, 5, 10);
    m_pArrow.setPoint(2, 10, 0);
    
    QPainter p2(&pixmap2);
    
    p2.fillRect(pixmap2.rect(), palette().background());
    p2.setBrush(palette().foreground());
    p2.translate(pixmap2.width()/3, 5);
    p2.drawConvexPolygon(m_pArrow);

    p2.end();
    
    m_icon.addPixmap(pixmap2, QIcon::Normal, QIcon::On);
}

void TToolBox::changeIcon(int index)
{
    setItemIcon(m_lastIndex, m_icon.pixmap (QSize(22,22), QIcon::Normal, QIcon::Off));
    setItemIcon(index, m_icon.pixmap (QSize(22,22), QIcon::Normal, QIcon::On));
    m_lastIndex = index;
}

void TToolBox::addPage(QWidget *page, const QString &title)
{
    addItem(page, m_icon, title);
}

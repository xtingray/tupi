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

#include "tformfactory.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <QLabel>

TFormFactory::TFormFactory()
{
}

TFormFactory::~TFormFactory()
{
}

QBoxLayout *TFormFactory::makeLine(const QString &text, QWidget *widget,  Qt::Orientation o)
{
    QBoxLayout *layout;
    
    if (o == Qt::Vertical)
        layout = new QVBoxLayout;
    else
        layout = new QHBoxLayout;
    
    layout->addWidget(new QLabel(text));
    layout->addWidget(widget);
    
    if (o == Qt::Vertical)
        layout->addStretch(3);
    
    return layout;
}

QGridLayout *TFormFactory::makeGrid(const QStringList &texts, const QWidgetList &widgets, Qt::Alignment alignment)
{
    Q_ASSERT(texts.count() != widgets.count());
    
    QGridLayout *layout = new QGridLayout;
    
    // layout->setColumnStretch(0, 1);
    
    for (int i = 0; i < widgets.count(); i++) {
         layout->addWidget(new QLabel(texts[i]), i, 0, Qt::AlignLeft);
         layout->addWidget(widgets[i], i, 1, alignment);
    }
    
    layout->setColumnStretch(2, 1);
    
    return layout;
}


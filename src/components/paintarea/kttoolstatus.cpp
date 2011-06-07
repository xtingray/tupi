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

#include "kttoolstatus.h"

#include <QLabel>
#include <QHBoxLayout>

#include "tdebug.h"
#include "tconfig.h"

KTToolStatus::KTToolStatus()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(2);

    tool = new QLabel("");
    QPixmap pix(THEME_DIR + "icons/pencil.png");
    tool->setToolTip(tr("Current Tool"));
    tool->setPixmap(pix);
    tool->setMaximumSize(20, 20);

    layout->addWidget(new QLabel(tr("Current Tool")));
    layout->addSpacing(3);
    layout->addWidget(tool);
}

KTToolStatus::~KTToolStatus()
{
}

void KTToolStatus::updateTooltip(const QString &currentTool)
{
    tool->setToolTip(currentTool);
}

void KTToolStatus::updatePixmap(const QPixmap &pixmap)
{
    tool->setPixmap(pixmap);
}

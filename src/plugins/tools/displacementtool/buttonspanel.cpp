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

#include "buttonspanel.h"
#include "kimagebutton.h"
#include "kdebug.h"

#include <QHBoxLayout>
#include <QBoxLayout>

ButtonsPanel::ButtonsPanel(QWidget *parent) : QWidget(parent)
{
    KImageButton *editButton = new KImageButton(QPixmap(THEME_DIR + "icons/tweener.png"), 22);
    editButton->setToolTip(tr("Edit Tween"));
    connect(editButton, SIGNAL(clicked()), this, SIGNAL(clickedEditTween()));

    KImageButton *removeButton = new KImageButton(QPixmap(THEME_DIR + "icons/minus_sign.png"), 22);
    removeButton->setToolTip(tr("Remove Tween"));
    connect(removeButton, SIGNAL(clicked()), this, SIGNAL(clickedRemoveTween()));

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    controlLayout->setMargin(1);
    controlLayout->setSpacing(5);
    controlLayout->addWidget(editButton);
    controlLayout->addWidget(removeButton);

    layout->addLayout(controlLayout);
}

ButtonsPanel::~ButtonsPanel()
{
}

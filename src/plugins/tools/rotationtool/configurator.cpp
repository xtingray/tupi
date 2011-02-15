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

#include "configurator.h"
#include "tweenmanager.h"

#include "ktitemtweener.h"
#include "kosd.h"
#include "kdebug.h"

#include <QLabel>
#include <QBoxLayout>

struct Configurator::Private
{
    QBoxLayout *layout;
    QBoxLayout *settingsLayout;
    TweenManager *tweenManager;

    KTItemTweener *currentTween;

    Settings::Mode mode;
    GuiState state;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->mode = Settings::View;
    k->state = Manager;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QLabel *title = new QLabel(tr("Rotation Tween"));
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(QFont("Arial", 8, QFont::Bold));

    k->layout->addWidget(title);

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    k->settingsLayout->setMargin(0);
    k->settingsLayout->setSpacing(0);

    setTweenManagerPanel();

    k->layout->addLayout(k->settingsLayout);
    k->layout->addStretch(2);
}

Configurator::~Configurator()
{
    delete k;
}

void Configurator::loadTweenList(QList<QString> tweenList)
{
    k->tweenManager->loadTweenList(tweenList);
    // if (tweenList.count() > 0)
    //     activeButtonsPanel(true);
}

void Configurator::setCurrentTween(KTItemTweener *currentTween)
{
    k->currentTween = currentTween;
}

void Configurator::setTweenManagerPanel()
{
    k->tweenManager = new TweenManager(this);

    k->settingsLayout->addWidget(k->tweenManager);
    k->state = Manager;
}

void Configurator::activeTweenManagerPanel(bool enable)
{
    if (enable)
        k->tweenManager->show();
    else
        k->tweenManager->hide();

    // if (k->tweenManager->listSize() > 0)
    //     activeButtonsPanel(enable);
}

void Configurator::closeSettingsPanel()
{
    if (k->state == Properties) {
        activeTweenManagerPanel(true);
        // activePropertiesPanel(false);
        k->mode = Settings::View;
        k->state = Manager;
    }
}

void Configurator::resetUI()
{
    k->tweenManager->resetUI();
    closeSettingsPanel();
}


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
#include "buttonspanel.h"

#include "ktitemtweener.h"
#include "kosd.h"
#include "kdebug.h"

#include <QLabel>
#include <QBoxLayout>

struct Configurator::Private
{
    QBoxLayout *layout;
    QBoxLayout *settingsLayout;
    Settings *settingsPanel;
    TweenManager *tweenManager;
    ButtonsPanel *controlPanel;

    KTItemTweener *currentTween;

    int framesTotal;
    int currentFrame;

    Settings::Mode mode;
    GuiState state;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->framesTotal = 1;
    k->currentFrame = 0;

    k->mode = Settings::View;
    k->state = Manager;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QLabel *title = new QLabel(tr("Opacity Tween"));
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(QFont("Arial", 8, QFont::Bold));

    k->layout->addWidget(title);

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    k->settingsLayout->setMargin(0);
    k->settingsLayout->setSpacing(0);

    setTweenManagerPanel();
    setButtonsPanel();
    setPropertiesPanel();

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
    if (tweenList.count() > 0)
        activeButtonsPanel(true);
}

void Configurator::setPropertiesPanel()
{
    k->settingsPanel = new Settings(this);

    // connect(k->settingsPanel, SIGNAL(startingPointChanged(int)), this, SIGNAL(startingPointChanged(int)));
    // connect(k->settingsPanel, SIGNAL(clickedSelect()), this, SIGNAL(clickedSelect()));
    // connect(k->settingsPanel, SIGNAL(clickedCreatePath()), this, SIGNAL(clickedCreatePath()));
    // connect(k->settingsPanel, SIGNAL(clickedApplyTween()), this, SLOT(applyItem()));
    connect(k->settingsPanel, SIGNAL(clickedResetTween()), this, SLOT(closeTweenProperties()));

    k->settingsLayout->addWidget(k->settingsPanel);

    activePropertiesPanel(false);
}

void Configurator::activePropertiesPanel(bool enable)
{
    if (enable)
        k->settingsPanel->show();
    else
        k->settingsPanel->hide();
}

void Configurator::setCurrentTween(KTItemTweener *currentTween)
{
    k->currentTween = currentTween;
}

void Configurator::setTweenManagerPanel()
{
    k->tweenManager = new TweenManager(this);
    connect(k->tweenManager, SIGNAL(addNewTween(const QString &)), this, SLOT(addTween(const QString &)));

    k->settingsLayout->addWidget(k->tweenManager);
    k->state = Manager;
}

void Configurator::activeTweenManagerPanel(bool enable)
{
    if (enable)
        k->tweenManager->show();
    else
        k->tweenManager->hide();

    if (k->tweenManager->listSize() > 0)
        activeButtonsPanel(enable);
}

void Configurator::setButtonsPanel()
{
    k->controlPanel = new ButtonsPanel(this);
    connect(k->controlPanel, SIGNAL(clickedEditTween()), this, SLOT(editTween()));
    connect(k->controlPanel, SIGNAL(clickedRemoveTween()), this, SLOT(removeTween()));

    k->settingsLayout->addWidget(k->controlPanel);

    activeButtonsPanel(false);
}

void Configurator::activeButtonsPanel(bool enable)
{
    if (enable)
        k->controlPanel->show();
    else
        k->controlPanel->hide();
}

void Configurator::addTween(const QString &name)
{
    k->mode = Settings::Add;

    k->settingsPanel->setParameters(name, k->framesTotal, k->currentFrame);

    activeTweenManagerPanel(false);
    activePropertiesPanel(true);

    k->state = Properties;
}

void Configurator::editTween()
{
    k->mode = Settings::Edit;
    k->state = Properties;

    k->settingsPanel->setParameters(k->currentTween);

    activeTweenManagerPanel(false);
    activePropertiesPanel(true);

    emit editModeOn();
}

void Configurator::removeTween()
{
    QString name = k->tweenManager->currentTweenName();
    k->tweenManager->removeItemFromList();

    if (k->tweenManager->listSize() == 0)
        activeButtonsPanel(false);

    removeTween(name);
}

QString Configurator::currentTweenName() const
{
    QString oldName = k->tweenManager->currentTweenName();
    QString newName = k->settingsPanel->currentTweenName();

    if (oldName.compare(newName) != 0)
        k->tweenManager->updateTweenName(newName);

    return newName;
}

void Configurator::removeTween(const QString &name)
{
    emit clickedRemoveTween(name);
}

void Configurator::closeTweenProperties()
{
    if (k->mode == Settings::Add) {
        k->tweenManager->removeItemFromList();
    } else if (k->mode == Settings::Edit) {
        closeSettingsPanel();
    }

    emit clickedResetInterface();

    closeSettingsPanel();
}

void Configurator::closeSettingsPanel()
{
    if (k->state == Properties) {
        activeTweenManagerPanel(true);
        activePropertiesPanel(false);
        k->mode = Settings::View;
        k->state = Manager;
    }
}

void Configurator::resetUI()
{
    k->tweenManager->resetUI();
    closeSettingsPanel();
}


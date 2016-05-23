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

#include "configurator.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "lipsyncmanager.h"

struct Configurator::Private
{
    QBoxLayout *settingsLayout;
    Settings *settingsPanel;
    LipSyncManager *manager;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QLabel *toolTitle = new QLabel;
    toolTitle->setAlignment(Qt::AlignHCenter);
    QPixmap pic(THEME_DIR + "icons/papagayo.png");
    toolTitle->setPixmap(pic.scaledToWidth(20, Qt::SmoothTransformation));
    toolTitle->setToolTip(tr("Papagayo LipSync Files"));
    layout->addWidget(toolTitle);
    layout->addWidget(new TSeparator(Qt::Horizontal));

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    k->settingsLayout->setMargin(0);
    k->settingsLayout->setSpacing(0);

    setLipSyncManagerPanel();
    setPropertiesPanel();

    layout->addLayout(k->settingsLayout);
    layout->addStretch(2);
}

Configurator::~Configurator()
{
    delete k;
}

void Configurator::loadLipSyncList(QList<QString> list)
{
    k->manager->loadLipSyncList(list);
}

void Configurator::setPropertiesPanel()
{
    k->settingsPanel = new Settings(this);
    connect(k->settingsPanel, SIGNAL(selectMouth(const QString &, int)), this, SIGNAL(selectMouth(const QString &, int)));
    connect(k->settingsPanel, SIGNAL(closeLipSyncProperties()), this, SLOT(closeSettingsPanel())); 
    connect(k->settingsPanel, SIGNAL(initFrameHasChanged(int)), this, SIGNAL(initFrameHasChanged(int)));
    connect(k->settingsPanel, SIGNAL(xPosChanged(int)), this, SIGNAL(xPosChanged(int)));
    connect(k->settingsPanel, SIGNAL(yPosChanged(int)), this, SIGNAL(yPosChanged(int)));

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

void Configurator::setLipSyncManagerPanel()
{
    k->manager = new LipSyncManager(this);
    connect(k->manager, SIGNAL(importLipSync()), this, SIGNAL(importLipSync()));
    connect(k->manager, SIGNAL(editCurrentLipSync(const QString &)), this, SLOT(editCurrentLipSync(const QString &)));
    connect(k->manager, SIGNAL(removeCurrentLipSync(const QString &)), this, SIGNAL(removeCurrentLipSync(const QString &)));

    k->settingsLayout->addWidget(k->manager);
}

void Configurator::activeLipSyncManagerPanel(bool enable)
{
    if (enable)
        k->manager->show();
    else
        k->manager->hide();
}

void Configurator::addLipSyncRecord(const QString &name)
{
    k->manager->addNewRecord(name);
}

void Configurator::editCurrentLipSync(const QString &name)
{
    emit editLipSyncSelection(name);

    activeLipSyncManagerPanel(false);
    activePropertiesPanel(true);
}

void Configurator::openLipSyncProperties(TupLipSync *lipsync)
{
    k->settingsPanel->openLipSyncProperties(lipsync);
}

void Configurator::resetUI()
{
    k->manager->resetUI();
    closeSettingsPanel();
}

void Configurator::closeSettingsPanel()
{
    emit closeLipSyncProperties();
    closePanels();
}

void Configurator::closePanels()
{
    activeLipSyncManagerPanel(true);
    activePropertiesPanel(false);
}

void Configurator::updateInterfaceRecords()
{
    k->settingsPanel->updateInterfaceRecords();
}

void Configurator::setPos(const QPointF &point)
{
    k->settingsPanel->setPos(point);
}

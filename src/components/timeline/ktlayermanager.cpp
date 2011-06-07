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

#include "ktlayermanager.h"
#include "tapplication.h"
#include "tdebug.h"

////////////////////////////////

struct KTLayerManager::Private
{
    Private() : allSelected(false), allVisible(true), allLock(false), rowHeight(20), sceneIndex(0) {}
    
    bool allSelected, allVisible, allLock;
    int rowHeight;
    int sceneIndex;
    KTLayerIndex *layerIndex;
    KTLayerControls *layerControls;
};

KTLayerManager::KTLayerManager(int sceneIndex, QWidget *parent) : QWidget(parent), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif

    k->layerIndex = new KTLayerIndex(sceneIndex); 
    k->layerIndex->setFixedWidth(200);
    k->layerControls = new KTLayerControls(sceneIndex);
    k->layerControls->setFixedWidth(44);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 16);
    layout->setSpacing(0);
    layout->addWidget(k->layerIndex);
    layout->addWidget(k->layerControls);

    setLayout(layout);


}

KTLayerManager::~KTLayerManager()
{
    #ifdef K_DEBUG
        TEND;
    #endif

    delete k;
}

KTLayerIndex* KTLayerManager::getLayerIndex()
{
    return k->layerIndex;
} 

KTLayerControls* KTLayerManager::getLayerControls()
{
    return k->layerControls;
}

void KTLayerManager::insertLayer(int position, const QString &name)
{
    k->layerIndex->insertLayer(position, name);
    k->layerControls->insertLayer(position);
}

void KTLayerManager::insertSoundLayer(int position, const QString &name)
{
}

void KTLayerManager::removeLayer(int position)
{
}

void KTLayerManager::renameLayer(int position, const QString &name)
{
}

void KTLayerManager::resizeEvent(QResizeEvent *event)
{
    k->layerIndex->refresh();
}

void KTLayerManager::moveLayer(int position, int newPosition)
{
}

void KTLayerManager::lockLayer(int position, bool locked)
{
}

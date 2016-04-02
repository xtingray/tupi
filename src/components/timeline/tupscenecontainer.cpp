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

#include "tupscenecontainer.h"

struct TupSceneContainer::Private
{
    QList<TupTimeLineTable *> scenes;
    QList<TupTimeLineTable *> undoScenes;
};

TupSceneContainer::TupSceneContainer(QWidget *parent) : QTabWidget(parent), k(new Private)
{
}

TupSceneContainer::~TupSceneContainer()
{
}

void TupSceneContainer::addScene(int sceneIndex, TupTimeLineTable *framesTable, const QString &sceneName)
{
    k->scenes << framesTable; 
    QTabWidget::insertTab(sceneIndex, framesTable, sceneName);
}

void TupSceneContainer::restoreScene(int sceneIndex, const QString &sceneName)
{
    TupTimeLineTable *framesTable = k->undoScenes.takeLast();
    k->scenes << framesTable;
    QTabWidget::insertTab(sceneIndex, framesTable, sceneName);
}

void TupSceneContainer::removeScene(int sceneIndex)
{
    k->undoScenes << k->scenes.takeAt(sceneIndex);  
    QTabWidget::removeTab(sceneIndex);
}

void TupSceneContainer::removeAllScenes()
{
    clear();
    k->scenes.clear();
    k->undoScenes.clear();
}

TupTimeLineTable * TupSceneContainer::currentScene()
{
    int index = currentIndex();
    TupTimeLineTable *framesTable = k->scenes.at(index);

    return framesTable;
}

TupTimeLineTable * TupSceneContainer::getTable(int index)
{
    TupTimeLineTable *framesTable = k->scenes.at(index);
    return framesTable;
}

int TupSceneContainer::scenesCount()
{
    return k->scenes.count();
}

#ifndef QT_NO_WHEELEVENT
void TupSceneContainer::wheelEvent(QWheelEvent *ev)
{
    QRect rect = tabBar()->rect();
    rect.setWidth(width());

    if (rect.contains(ev->pos()))
        wheelMove(ev->delta());
}

void TupSceneContainer::wheelMove(int delta)
{
    if (count() > 1) {
        int current = currentIndex();
        if (delta < 0) {
            current = (current + 1) % count();
        } else {
            current--;
            if (current < 0)
                current = count() - 1;
        }
        setCurrentIndex(current);
    }
}

#endif

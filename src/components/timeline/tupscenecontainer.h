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

#ifndef TUPSCENECONTAINER_H
#define TUPSCENECONTAINER_H

#include "tglobal.h"
#include "tuptimelinetable.h"

#include <QTabWidget>
#include <QWheelEvent>
#include <QTabBar>

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT TupSceneContainer : public QTabWidget
{
    Q_OBJECT

    public:
        TupSceneContainer(QWidget *parent = 0);
        ~TupSceneContainer();
        void addScene(int sceneIndex, TupTimeLineTable *framesTable, const QString &title);
        void restoreScene(int sceneIndex, const QString &title);
        void removeScene(int sceneIndex);
        TupTimeLineTable * currentScene();
        
    public slots:
        void removeAllTabs();
        
    protected:
    #ifndef QT_NO_WHEELEVENT
            virtual void wheelEvent(QWheelEvent *e);
    #endif

    protected slots:
    #ifndef QT_NO_WHEELEVENT
            virtual void wheelMove(int delta);
    #endif

    private:
        struct Private;
        Private *const k;
};

#endif

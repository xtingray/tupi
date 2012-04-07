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

#ifndef TupSCENESWIDGET_H
#define TupSCENESWIDGET_H

#include "tupmodulewidgetbase.h"
#include "tupsceneslist.h"
#include "treewidgetsearchline.h"

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QPixmap>
#include <QList>
#include <QTreeWidgetItem>

class TupSceneResponse;

class TupScenesWidget : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupScenesWidget(QWidget *parent = 0);
        ~TupScenesWidget();

    public slots:
        void closeAllScenes();

    private:
        void setupButtons();
        void setupTableScenes();

    private slots:
        void emitRequestInsertScene();
        void emitRequestRemoveScene();
        void renameObject(QTreeWidgetItem* item);
        void refreshItem(QTreeWidgetItem *item);

    protected:
        virtual void sceneResponse(TupSceneResponse *e);

    public slots:
        void sendEvent(int action);
        void selectScene(int index);

    private:
        struct Private;
        Private *const k;
};

#endif

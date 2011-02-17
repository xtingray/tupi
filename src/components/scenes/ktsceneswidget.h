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

#ifndef KTSCENESWIDGET_H
#define KTSCENESWIDGET_H

#include "ktmodulewidgetbase.h"
#include "ktsceneslist.h"
#include "ktreewidgetsearchline.h"

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
//#include <QLineEdit>
#include <QPixmap>
#include <QList>
#include <QTreeWidgetItem>

class KTSceneResponse;

class KTScenesWidget : public KTModuleWidgetBase
{
    Q_OBJECT

    public:
        KTScenesWidget(QWidget *parent = 0);
        ~KTScenesWidget();

    public slots:
        void closeAllScenes();

    private:
        struct Private;
        Private *const k;

    private:
        void setupButtons();
        void setupTableScenes();

    private slots:
        // void sceneDobleClick(QTreeWidgetItem * item, int col);
        void emitRequestInsertScene();
        void emitRequestRemoveScene();
        void renameObject(QTreeWidgetItem* item);
        void refreshItem(QTreeWidgetItem *item);
        // void emitRequestRenameScene(QTreeWidgetItem *item);

    protected:
        virtual void sceneResponse(KTSceneResponse *e);

    public slots:
        void sendEvent(int action);
        void selectScene(const QString & name, int index);
};

#endif

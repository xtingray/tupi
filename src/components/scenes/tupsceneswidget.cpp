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

#include "tupsceneswidget.h"
#include "tupprojectrequest.h"
#include "tupprojectactionbar.h"
#include "tuprequestbuilder.h"

#include "tglobal.h"
#include "tdebug.h"
#include "timagebutton.h"
#include "toptionaldialog.h"
#include "tconfig.h"

#include <QToolTip>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

struct TupScenesWidget::Private
{
    QButtonGroup *buttonGroup;
    TupScenesList *scenesTable;

    bool renaming;
    QString oldId;
};

TupScenesWidget::TupScenesWidget(QWidget *parent) : TupModuleWidgetBase(parent, "TupScenesWidget"), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowTitle(tr("Scenes Manager"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/scenes.png"));
    setupButtons();
    setupTableScenes();
}

TupScenesWidget::~TupScenesWidget()
{
    #ifdef K_DEBUG
           TEND;
    #endif
    delete k;
}

void TupScenesWidget::setupButtons()
{
    TupProjectActionBar *bar = new TupProjectActionBar(QString("TimeLine"), 
                              TupProjectActionBar::InsertScene | TupProjectActionBar::RemoveScene);
    bar->button(TupProjectActionBar::InsertScene)->setIcon(QIcon(THEME_DIR + "icons/plus_sign.png"));
    bar->button(TupProjectActionBar::RemoveScene)->setIcon(QIcon(THEME_DIR + "icons/minus_sign.png"));
    bar->insertBlankSpace(1);

    connect(bar, SIGNAL(actionSelected(int)), this, SLOT(sendEvent(int)));

    addChild(bar, Qt::AlignCenter);
}

void TupScenesWidget::setupTableScenes()
{
    k->scenesTable = new TupScenesList(this);

    TreeWidgetSearchLine *searcher = new TreeWidgetSearchLine(this, k->scenesTable);
    searcher->setClickMessage(tr("Filter here..."));

    addChild(searcher);
    addChild(k->scenesTable);

    // SQA: All these connections are really necessary? Please check! 

    connect(k->scenesTable, SIGNAL(changeCurrent(int)), this, SLOT(selectScene(int)));

    connect(k->scenesTable, SIGNAL(itemRenamed(QTreeWidgetItem *)), this,
            SLOT(renameObject(QTreeWidgetItem*)));

    connect(k->scenesTable, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                                   SLOT(refreshItem(QTreeWidgetItem*)));
}

void TupScenesWidget::sendEvent(int action)
{
    switch (action) {
            case TupProjectActionBar::InsertScene:
                 {
                  emitRequestInsertScene();
                 }
                 break;
            case TupProjectActionBar::RemoveScene:
                 {
                  emitRequestRemoveScene();
                 }
                 break;
            default: 
                 break;
    }
}

void TupScenesWidget::selectScene(int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
    emit requestTriggered(&event);
}

void TupScenesWidget::emitRequestInsertScene()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int index = k->scenesTable->scenesCount();
    int label = index + 1;
    QString name = tr("Scene %1").arg(label);

    while (k->scenesTable->nameExists(name)) {
           label++;
           name = tr("Scene %1").arg(label);
    }

    TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Add, name); 
    emit requestTriggered(&event);

    event = TupRequestBuilder::createLayerRequest(index, 0, TupProjectRequest::Add, tr("Layer %1").arg(1));
    emit requestTriggered(&event);

    event = TupRequestBuilder::createFrameRequest(index, 0, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
    emit requestTriggered(&event);

    event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
    emit requestTriggered(&event);
}

void TupScenesWidget::emitRequestRemoveScene()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int index = k->scenesTable->currentSceneIndex();

    if (k->scenesTable->scenesCount() == 1) {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Reset, 
                                                                      tr("Scene %1").arg(1));
        emit requestTriggered(&event);
    } else {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Remove);
        emit requestTriggered(&event);

        if (k->scenesTable->scenesCount() == index)
            index--;
        event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit requestTriggered(&event);
    }
}

void TupScenesWidget::closeAllScenes()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->scenesTable->resetUI();
}

void TupScenesWidget::sceneResponse(TupSceneResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scenes");
           SHOW_VAR(e->action());
    #endif

    int index = e->sceneIndex();

    switch (e->action()) {
            case TupProjectRequest::Add:
             {
               k->scenesTable->insertScene(index, e->arg().toString());
             }
            break;
            case TupProjectRequest::Remove:
             {
               k->scenesTable->removeScene(index);
             }
            break;
            case TupProjectRequest::Reset:
             {
               k->scenesTable->renameScene(index, e->arg().toString());
             }
            break;
            case TupProjectRequest::Rename:
             {
               k->scenesTable->renameScene(index, e->arg().toString());
             }
            break;
            case TupProjectRequest::Select:
             {
               if (k->scenesTable->currentSceneIndex() != index)
                   k->scenesTable->selectScene(index);
             }
            break;
            default: 
            break;
    }
}

void TupScenesWidget::renameObject(QTreeWidgetItem* item)
{
    if (item) {
        k->renaming = true;
        k->oldId = item->text(1);
        k->scenesTable->editItem(item, 0);
    } 
}

void TupScenesWidget::refreshItem(QTreeWidgetItem *item)
{
    if (k->renaming) {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(k->scenesTable->currentSceneIndex(), 
                                                                      TupProjectRequest::Rename, item->text(0));
        emit requestTriggered(&event);
        k->renaming = false;
    }
}

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

#include "ktsceneswidget.h"
#include "ktprojectrequest.h"
#include "ktprojectactionbar.h"
#include "ktrequestbuilder.h"

#include "tglobal.h"
#include "tdebug.h"
#include "kimagebutton.h"
#include "koptionaldialog.h"
#include "tconfig.h"

#include <QToolTip>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

struct KTScenesWidget::Private
{
    QButtonGroup *buttonGroup;
    KTScenesList *scenesTable;

    bool renaming;
    QString oldId;
};

KTScenesWidget::KTScenesWidget(QWidget *parent) : KTModuleWidgetBase(parent, "KTScenesWidget"), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowTitle(tr("Scenes Manager"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/scenes.png"));
    setupButtons();
    setupTableScenes();
}

KTScenesWidget::~KTScenesWidget()
{
    #ifdef K_DEBUG
           TEND;
    #endif
    delete k;
}

void KTScenesWidget::setupButtons()
{
    KTProjectActionBar *bar = new KTProjectActionBar(QString("TimeLine"), 
                              KTProjectActionBar::InsertScene | KTProjectActionBar::RemoveScene);
    bar->button(KTProjectActionBar::InsertScene)->setIcon(QIcon(THEME_DIR + "icons/plus_sign.png"));
    bar->button(KTProjectActionBar::RemoveScene)->setIcon(QIcon(THEME_DIR + "icons/minus_sign.png"));
    bar->insertBlankSpace(1);

    connect(bar, SIGNAL(actionSelected(int)), this, SLOT(sendEvent(int)));

    addChild(bar, Qt::AlignCenter);
}

void KTScenesWidget::setupTableScenes()
{
    k->scenesTable = new KTScenesList(this);

    KTreeWidgetSearchLine *searcher = new KTreeWidgetSearchLine(this, k->scenesTable);
    searcher->setClickMessage(tr("Filter here..."));

    addChild(searcher);
    addChild(k->scenesTable);

    // SQA: All these connections are really necessary? Please check! 

    connect(k->scenesTable, SIGNAL(changeCurrent(QString, int)), 
            this, SLOT(selectScene(QString, int)));

    /*
    connect(k->scenesTable, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), 
            this, SLOT(sceneDobleClick(QTreeWidgetItem *, int)));
    */

    connect(k->scenesTable, SIGNAL(itemRenamed(QTreeWidgetItem *)), this,
            SLOT(renameObject(QTreeWidgetItem*)));

    connect(k->scenesTable, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                                   SLOT(refreshItem(QTreeWidgetItem*)));
}

void KTScenesWidget::sendEvent(int action)
{
    switch (action) {
            case KTProjectActionBar::InsertScene:
                 {
                  emitRequestInsertScene();
                 }
                 break;
            case KTProjectActionBar::RemoveScene:
                 {
                  emitRequestRemoveScene();
                 }
                 break;
            default: 
                 break;
    }
}

void KTScenesWidget::selectScene(QString name, int index)
{
    Q_UNUSED(name);
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    // int total = k->scenesTable->scenesCount();

    KTProjectRequest event = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Select);
    emit requestTriggered(&event);
}

void KTScenesWidget::emitRequestInsertScene()
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

    KTProjectRequest event = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Add, name); 
    emit requestTriggered(&event);

    event = KTRequestBuilder::createLayerRequest(index, 0, KTProjectRequest::Add, tr("Layer %1").arg(1));
    emit requestTriggered(&event);

    event = KTRequestBuilder::createFrameRequest(index, 0, 0, KTProjectRequest::Add, tr("Frame %1").arg(1));
    emit requestTriggered(&event);

    k->scenesTable->selectScene(index);
}

void KTScenesWidget::emitRequestRemoveScene()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int index = k->scenesTable->indexCurrentScene();

    if (k->scenesTable->scenesCount() == 1) {
        KTProjectRequest event = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Reset, 
                                                                      tr("Scene %1").arg(1));
        emit requestTriggered(&event);
    } else {
        KTProjectRequest event = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Remove);
        emit requestTriggered(&event);
    }
}

void KTScenesWidget::closeAllScenes()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->scenesTable->resetUI();
    // k->scenesTable->removeAll();
}

void KTScenesWidget::sceneResponse(KTSceneResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scenes");
           SHOW_VAR(e->action());
    #endif

    int index = e->sceneIndex();

    switch (e->action()) {
            case KTProjectRequest::Add:
             {
               k->scenesTable->insertScene(index, e->arg().toString());
             }
            break;
            case KTProjectRequest::Remove:
             {
               k->scenesTable->removeScene(index);
             }
            break;
            case KTProjectRequest::Reset:
             {
               k->scenesTable->renameScene(index, e->arg().toString());
             }
            break;
            case KTProjectRequest::Rename:
             {
               k->scenesTable->renameScene(index, e->arg().toString());
             }
            break;
            case KTProjectRequest::Select:
             {
               k->scenesTable->selectScene(index);
             }
            break;
            default: 
            break;
    }
}

void KTScenesWidget::renameObject(QTreeWidgetItem* item)
{
    if (item) {
        k->renaming = true;
        k->oldId = item->text(1);
        k->scenesTable->editItem(item, 0);
    } 
}

void KTScenesWidget::refreshItem(QTreeWidgetItem *item)
{
    if (k->renaming) {
        KTProjectRequest event = KTRequestBuilder::createSceneRequest(k->scenesTable->indexCurrentScene(), 
                                                                      KTProjectRequest::Rename, item->text(0));
        emit requestTriggered(&event);
        k->renaming = false;
    }
}

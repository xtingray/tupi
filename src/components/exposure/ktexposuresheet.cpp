/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "ktexposuresheet.h"
#include "tapplication.h"

#include "tdebug.h"
#include "tglobal.h"
#include "koptionaldialog.h"

#include <QToolTip>
#include <QPixmap>
#include <QHBoxLayout>
#include <QList>
#include <QMenu>

#include "ktprojectrequest.h"
#include "ktrequestbuilder.h"

struct KTExposureSheet::Private
{
    KTSceneTabWidget *scenes;
    KTExposureTable *currentTable;
    KTProjectActionBar *actionBar;
    QMenu *menu;
    QString nameCopyFrame;
    bool fromMenu;
};

KTExposureSheet::KTExposureSheet(QWidget *parent) : KTModuleWidgetBase(parent, "Exposure Sheet"), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->currentTable = 0;
    k->fromMenu = false;
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/exposure_sheet.png"));

    k->actionBar = new KTProjectActionBar(QString("Exposure"), KTProjectActionBar::InsertLayer |
                        KTProjectActionBar::RemoveLayer |
                        KTProjectActionBar::Separator |
                        KTProjectActionBar::InsertFrame |
                        KTProjectActionBar::RemoveFrame |
                        KTProjectActionBar::MoveFrameUp |
                        KTProjectActionBar::MoveFrameDown | 
                        KTProjectActionBar::LockFrame);

    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(applyAction(int)));
    addChild(k->actionBar, Qt::AlignCenter);

    k->scenes = new KTSceneTabWidget(this);
    connect(k->scenes->TabWidget(), SIGNAL(currentChanged(int)), this, SLOT(emitRequestChangeScene(int)));
    addChild(k->scenes);
    createMenu();
}

KTExposureSheet::~KTExposureSheet()
{
    delete k;
    #ifdef K_DEBUG
           TEND;
    #endif
}

void KTExposureSheet::createMenu()
{
    k->menu = new QMenu(tr("actions"));
    //k->menu->addAction(tr("Insert layer"))->setData(KTProjectActionBar::InsertLayer);
    //k->menu->addAction(tr("Remove layer"))->setData(KTProjectActionBar::RemoveLayer);

    QMenu *insertMenu = new QMenu(tr("Insert"));

    QAction *insertOne = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("1 frame"), this); 
    insertOne->setIconVisibleInMenu(true);
    insertMenu->addAction(insertOne);
    connect(insertOne, SIGNAL(triggered()), this, SLOT(insertOneFrame()));

    QAction *insertFive = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("5 frames"), this);                        
    insertFive->setIconVisibleInMenu(true);
    insertMenu->addAction(insertFive);
    connect(insertFive, SIGNAL(triggered()), this, SLOT(insertFiveFrames()));

    QAction *insertTen = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("10 frames"), this);
    insertTen->setIconVisibleInMenu(true);
    insertMenu->addAction(insertTen);
    connect(insertTen, SIGNAL(triggered()), this, SLOT(insertTenFrames()));

    k->menu->addMenu(insertMenu);

    QAction *removeOne = new QAction(QIcon(THEME_DIR + "icons/remove_frame.png"), tr("Remove frame"), this);
    removeOne->setIconVisibleInMenu(true);
    k->menu->addAction(removeOne);
    connect(removeOne, SIGNAL(triggered()), this, SLOT(removeOne()));

    QAction *lockAction = new QAction(QIcon(THEME_DIR + "icons/padlock.png"), tr("Lock/Unlock frame"), this);
    lockAction->setIconVisibleInMenu(true);
    k->menu->addAction(lockAction);
    connect(lockAction, SIGNAL(triggered()), this, SLOT(lockFrame()));

    QAction *copyAction = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("Copy frame"), this);
    copyAction->setIconVisibleInMenu(true);
    k->menu->addAction(copyAction);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(emitRequestCopyCurrentFrame()));

    QAction *pasteAction = new QAction(QIcon(THEME_DIR + "icons/paste.png"), tr("Paste in frame"), this);
    pasteAction->setIconVisibleInMenu(true);
    k->menu->addAction(pasteAction);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(emitRequestPasteInCurrentFrame()));

    QMenu *expandMenu = new QMenu(tr("Expand"));
    expandMenu->addAction(tr("1 frame"), this, SLOT(expandCurrentFrameOnce()));
    expandMenu->addAction(tr("5 frames"), this, SLOT(expandCurrentFrameFive()));
    expandMenu->addAction(tr("10 frames"), this, SLOT(expandCurrentFrameTen()));
    expandMenu->setDisabled(true);
    
    k->menu->addMenu(expandMenu);
    connect(k->menu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));
    //connect(expandMenu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));
}

void KTExposureSheet::addScene(int index, const QString &name)
{
    #ifdef K_DEBUG
           T_FUNCINFO << " index: " << index << " name: " << name;
    #endif

    KTExposureTable *newScene = new KTExposureTable;
    newScene->setMenu(k->menu);

    connect(newScene, SIGNAL(requestSetUsedFrame(int, int)), 
                             this, SLOT(insertFrame(int, int)));

    connect(newScene, SIGNAL(requestRenameFrame(int, int,const QString &)), 
                             this, SLOT(renameFrame(int, int, const QString &)));

    connect(newScene, SIGNAL(requestSelectFrame(int, int)), SLOT(selectFrame(int, int)));

    connect(newScene, SIGNAL(requestRenameLayer(int, const QString &)), 
                             this, SLOT(renameLayer(int, const QString &)));

    connect(newScene, SIGNAL(requestMoveLayer(int,int)), this, SLOT(moveLayer(int, int)));

    connect(newScene, SIGNAL(requestChangeVisibilityLayer(int , bool)),
                             this, SLOT(changeVisibilityLayer(int, bool)));

    k->scenes->addScene(index, name, newScene);
}

void KTExposureSheet::renameScene(int index, const QString &name)
{
    k->scenes->TabWidget()->setTabText(index, name);
}

void KTExposureSheet::applyAction(int action)
{
    #ifdef K_DEBUG
           T_FUNCINFO<< "action: " << action;
    #endif

     k->currentTable = k->scenes->getCurrentTable();

    if (k->currentTable == 0) {
        #ifdef K_DEBUG
               tFatal() << "KTExposureSheet::applyAction: No layer view!!" << endl;
        #endif
        return;
    }

    switch (action) {
            case KTProjectActionBar::InsertLayer:
               {
                 int layer = k->currentTable->columnCount();

                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(k->scenes->currentIndex(),
                                                                               layer, KTProjectRequest::Add, tr("Layer %1").arg(layer + 1));
                 emit requestTriggered(&event);

                 int framesNum = k->currentTable->usedFrames(k->currentTable->currentColumn());

                 for (int i=0;i < framesNum;i++) {
                      event = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), layer, i, 
                                                                   KTProjectRequest::Add, tr("Frame %1").arg(i + 1));
                      emit requestTriggered(&event);
                 }
               }
               break;
            case KTProjectActionBar::RemoveLayer:
               {
                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(k->scenes->currentIndex(), 
                                                                               k->currentTable->currentLayer(), 
                                                                               KTProjectRequest::Remove);
                 emit requestTriggered(&event);
               }
               break;
            case KTProjectActionBar::InsertFrame:
               {
                 int usedFrames = k->currentTable->usedFrames(k->currentTable->currentColumn());
                 //int target = k->currentTable->currentRow();

                 if (k->currentTable->currentRow() >= usedFrames) {
                     
                     for (int layer=0; layer < k->currentTable->layersTotal(); layer++) { 
                          if (usedFrames >= k->currentTable->usedFrames(layer)) {
                              int finish = k->currentTable->currentFrame() + 1;
                              for (int frame = usedFrames; frame <= finish; frame++)
                                   insertFrame(layer, frame);
                          }
                     }
                     // selectFrame(k->currentTable->currentLayer(), target);

                 } else {
                     insertFrames(1);
                 }

               }
               break;
            case KTProjectActionBar::RemoveFrame:
               {
                 int scene = k->scenes->currentIndex();
                 int layer = k->currentTable->currentLayer();
                 int lastFrame = k->currentTable->framesTotalAtCurrentLayer() - 1;
                 int target = k->currentTable->currentFrame();

                 if (k->currentTable->currentRow() > lastFrame)
                     return;

                 if (k->currentTable->frameIsLocked(layer, target))
                     k->actionBar->emitActionSelected(KTProjectActionBar::LockFrame);

                 if (k->currentTable->framesTotalAtCurrentLayer() == 1) {
                     KTProjectRequest event = KTRequestBuilder::createFrameRequest(scene,
                                              layer, target, KTProjectRequest::Reset);
                     emit requestTriggered(&event);
                     k->fromMenu = false;

                     return;
                 }

                 // SQA: Take care about the first frame case and paint a message on the workspace 
                 if (target == lastFrame) {
                     KTProjectRequest event = KTRequestBuilder::createFrameRequest(scene, 
                                              layer, target, KTProjectRequest::Remove);
                     emit requestTriggered(&event);
                     if (target > 0)
                         selectFrame(layer, target-1);
                     else 
                         k->currentTable->clearSelection();
                 } else {
                     // When the item deleted is not the last one

                     for (int index=target+1; index <= lastFrame; index++) {
                          KTProjectRequest event = KTRequestBuilder::createFrameRequest(scene,
                                                   layer, index, KTProjectRequest::Move, index - 1);
                          emit requestTriggered(&event);
                     }

                     selectFrame(layer, target);

                     KTProjectRequest request = KTRequestBuilder::createFrameRequest(scene, 
                                                layer, lastFrame, KTProjectRequest::Remove);
                     emit requestTriggered(&request);
                 }

                 k->fromMenu = false;
               }
               break;
            case KTProjectActionBar::MoveFrameUp:
               {
                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                          k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                          KTProjectRequest::Exchange, k->currentTable->currentFrame()-1);
                 emit requestTriggered(&event);
               }
               break;
            case KTProjectActionBar::MoveFrameDown:
               {
                 if (k->currentTable->currentFrame()+1 == k->currentTable->framesTotalAtCurrentLayer())
                     insertFrames(1);

                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                          k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                          KTProjectRequest::Exchange, k->currentTable->currentFrame()+1);
                 emit requestTriggered(&event);
               }
               break;
            case KTProjectActionBar::LockFrame:
               {
                 bool locked = k->currentTable->frameIsLocked(k->currentTable->currentColumn(), 
                               k->currentTable->currentFrame());

                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                          k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                          KTProjectRequest::Lock, !locked);
                                          emit requestTriggered(&event);
               }
               break;
    }
}

void KTExposureSheet::setScene(int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->scenes->count() >= index) {
        k->scenes->blockSignals(true);
        k->scenes->TabWidget()->setCurrentIndex(index);		
        k->currentTable = k->scenes->getCurrentTable();
        k->scenes->blockSignals(false);
    }
}

void KTExposureSheet::emitRequestChangeScene(int index)
{
    if (k->scenes->count() > 1) {
        KTProjectRequest request = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void KTExposureSheet::emitRequestCopyCurrentFrame()
{
    KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                                                    k->currentTable->currentLayer(), 
                                                                    k->currentTable->currentFrame(), 
                                                                    KTProjectRequest::Copy);
    emit localRequestTriggered(&request);
}

void KTExposureSheet::emitRequestPasteInCurrentFrame()
{
    if (k->nameCopyFrame.isEmpty())
        return;

    if (k->currentTable->usedFrames(k->currentTable->currentLayer()) <= k->currentTable->currentRow()) {
        for(int i = k->currentTable->usedFrames(k->currentTable->currentLayer()); 
            i <= k->currentTable->currentRow(); i++) {
            insertFrame(k->currentTable->currentLayer(), i);

            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                                         k->currentTable->currentLayer(), i, 
                                                         KTProjectRequest::Paste);
            emit localRequestTriggered(&request);
        }
    } else {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                                         k->currentTable->currentLayer(), 
                                                         k->currentTable->currentFrame(),
                                                         KTProjectRequest::Paste);
            emit localRequestTriggered(&request);
    }
}

void KTExposureSheet::emitRequestExpandCurrentFrame(int n)
{
    KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(),
                                                 k->currentTable->currentLayer(),
                                                 k->currentTable->currentFrame(),
                                                 KTProjectRequest::Expand, n);
    emit requestTriggered(&request);
}

void KTExposureSheet::expandCurrentFrameOnce()
{
    emitRequestExpandCurrentFrame(1);
}

void KTExposureSheet::expandCurrentFrameFive()
{
    emitRequestExpandCurrentFrame(5);
}

void KTExposureSheet::expandCurrentFrameTen()
{
    emitRequestExpandCurrentFrame(10);
}


void KTExposureSheet::insertFrame(int indexLayer, int indexFrame)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), 
                                                 indexLayer, indexFrame, KTProjectRequest::Add, tr("Frame %1").arg(indexFrame + 1));
    emit requestTriggered(&request);
}

void KTExposureSheet::renameFrame(int indexLayer, int indexFrame, const QString & name)
{
    KTProjectRequest event = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex() , indexLayer, indexFrame,
                                               KTProjectRequest::Rename, name);
    emit requestTriggered(&event);
}

void KTExposureSheet::selectFrame(int indexLayer, int indexFrame)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif 

    KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scenes->currentIndex(), indexLayer, 
                                                 indexFrame, KTProjectRequest::Select, "1");
    emit localRequestTriggered(&request);
    // emit requestTriggered(&request);
}

void KTExposureSheet::changeVisibilityLayer(int visualIndexLayer, bool visibility)
{
    KTProjectRequest event = KTRequestBuilder::createLayerRequest(k->scenes->currentIndex(), visualIndexLayer,
                                               KTProjectRequest::View, visibility);
    emit requestTriggered(&event);
}

void KTExposureSheet::renameLayer(int indexLayer, const QString & name)
{
    KTProjectRequest event = KTRequestBuilder::createLayerRequest(k->scenes->currentIndex(), indexLayer,
                                               KTProjectRequest::Rename, name);
    emit requestTriggered(&event);
}

void KTExposureSheet::moveLayer(int oldIndex, int newIndex)
{
    KTProjectRequest event = KTRequestBuilder::createLayerRequest(k->scenes->currentIndex(), oldIndex,
                                               KTProjectRequest::Move, newIndex);
    emit requestTriggered(&event);
}

void KTExposureSheet::actionTriggered(QAction *action)
{
    bool ok;
    int id = action->data().toInt(&ok);

    if (ok)
        applyAction(id);
}

void KTExposureSheet::closeAllScenes()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->scenes->blockSignals(true);

    delete k->currentTable;
    k->scenes->removeAllTabs();
    k->currentTable = 0;

    k->scenes->blockSignals(false);
}

void KTExposureSheet::sceneResponse(KTSceneResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("exposure");
    #endif

    switch(e->action()) {
           case KTProjectRequest::Add:
            {
                addScene(e->sceneIndex(), e->arg().toString());
            }
           break;
           case KTProjectRequest::Remove:
            {
                k->scenes->blockSignals(true);
                //QWidget * widget = k->scenes->getTable(e->sceneIndex());
                k->scenes->TabWidget()->removeTab(e->sceneIndex());
                //delete widget;
                k->scenes->blockSignals(false);
            }
           break;
           case KTProjectRequest::Move:
            {
            }
           break;
           case KTProjectRequest::Lock:
            {
            }
           break;
           case KTProjectRequest::Rename:
            {
                renameScene(e->sceneIndex(), e->arg().toString());
            }
           break;
           case KTProjectRequest::Select:
            {
                setScene(e->sceneIndex());
            }
           break;
    }
}

void KTExposureSheet::layerResponse(KTLayerResponse *e)
{
    KTExposureTable *table = k->scenes->getTable(e->sceneIndex());

    if (table) {
        switch (e->action()) {
                case KTProjectRequest::Add:
                 {
                     table->insertLayer(e->layerIndex(), e->arg().toString());
                 }
                break;
                case KTProjectRequest::Remove:
                 {
                     table->removeLayer(e->layerIndex());

                     if (table->layersTotal() == 0) {
                         KTProjectRequest request = KTRequestBuilder::createLayerRequest(0, 0, KTProjectRequest::Add, tr("Layer %1").arg(1));
                         emit requestTriggered(&request);

                         request = KTRequestBuilder::createFrameRequest(0, 0, 0, KTProjectRequest::Add, tr("Frame %1").arg(1));
                         emit requestTriggered(&request);
                     }
                 }
                break;
                case KTProjectRequest::Move:
                 {
                     table->moveLayer(e->layerIndex(), e->arg().toInt());
                 }
                break;
                case KTProjectRequest::Rename:
                 {
                     table->setLayerName(e->layerIndex(), e->arg().toString());
                 }
                break;
                case KTProjectRequest::Lock:
                 {
                     table->setLockLayer(e->layerIndex(), e->arg().toBool());
                 }
                break;
                case KTProjectRequest::Select:
                 {
                     setScene(e->sceneIndex());
                     table->blockSignals(true);
                     table->selectFrame(e->layerIndex(), 0);
                     table->blockSignals(false);
                 }
                case KTProjectRequest::View:
                 {
                     table->setVisibilityChanged(e->layerIndex(), e->arg().toBool());
                 }
                break;
                default:
                     #ifdef K_DEBUG
                            tFatal() << "KTExposureSheet::layerResponse - Layer option undefined! -> " << e->action();
                     #endif
                break;
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "KTExposureSheet::layerResponse -> Scene index invalid: " << e->sceneIndex();
        #endif
    }
}

void KTExposureSheet::frameResponse(KTFrameResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    KTExposureTable *table = k->scenes->getTable(e->sceneIndex());

    if (table) {
        switch (e->action()) {
                case KTProjectRequest::Add:
                 {
                     table->insertFrame(e->layerIndex(), e->frameIndex(), e->arg().toString(), e->external());

                     if (e->layerIndex() == 0 && e->frameIndex() == 0) {
                         setScene(e->sceneIndex());
                         table->selectFrame(0, 0);
                     }
                 }
                break;
                case KTProjectRequest::Remove:
                 {
                     table->removeFrame(e->layerIndex(), e->frameIndex(), k->fromMenu);
                 }
                break;
                case KTProjectRequest::Reset:
                 {
                     // No action required for this specific request
                     return;
                 }
                break;
                case KTProjectRequest::Exchange:
                 {
                     if (e->arg().toInt() < table->framesTotalAtCurrentLayer()) {
                         table->exchangeFrame(e->layerIndex(), e->frameIndex(), e->layerIndex(), e->arg().toInt(),
                                              e->external());
                     } else {
                         // insertFrame(e->layerIndex(), e->arg().toInt());
                         //insertFrames(1);
                         table->exchangeFrame(e->layerIndex(), e->frameIndex(), e->layerIndex(), e->arg().toInt(),
                                              e->external());
                     }
                 }
                break;
                case KTProjectRequest::Move:
                 {
                     // No action required for this specific request
                     return;
                 }
                break;
                case KTProjectRequest::Lock:
                 {
                     table->setLockFrame(e->layerIndex(), e->frameIndex(),  e->arg().toBool());
                 }
                break;
                case KTProjectRequest::Rename:
                 {
                     table->setFrameName(e->layerIndex(), e->frameIndex(), e->arg().toString());
                 }
                break;
                case KTProjectRequest::Select:
                 {
                     table->blockSignals(true);
                     // setScene(e->sceneIndex());
                     table->selectFrame(e->layerIndex(), e->frameIndex());
                     table->blockSignals(false);
                 }
                break;
                case KTProjectRequest::Expand:
                 {
                     tFatal() << "KTExposureSheet::frameResponse - Expand! -> Just Tracing!";
                     tFatal() << "KTExposureSheet::frameResponse - Starting point: -> " << e->frameIndex();
                     tFatal() << "KTExposureSheet::frameResponse - Range: -> " << e->arg().toInt();
                     for(int i = 0; i < e->arg().toInt(); i++)
                         table->insertFrame(e->layerIndex(), e->frameIndex()+i+1, 
                                            table->frameName(e->layerIndex(), e->frameIndex()), 
                                            e->external());
                 }
                break;
                case KTProjectRequest::Copy:
                 {
                     k->nameCopyFrame = table->frameName(e->layerIndex(), e->frameIndex());
                 }
                break;
                case KTProjectRequest::Paste:
                 {
                     if (e->frameIndex() >= table->usedFrames(e->layerIndex())) {
                         if (e->mode() == KTProjectResponse::Undo) {
                             if (e->arg().toString().isEmpty())
                                 table->removeFrame(e->layerIndex(), e->frameIndex(), false);
                         } else {
                                 table->insertFrame(e->layerIndex(), e->frameIndex(), 
                                                     k->nameCopyFrame + "- copy", e->external());
                         }
                     }
                 }
                break;
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "KTExposureSheet::frameResponse -> Scene index invalid: " << e->sceneIndex();
        #endif
    }
}

void KTExposureSheet::insertOneFrame()
{
    insertFrames(1);
}

void KTExposureSheet::insertFiveFrames()
{
    insertFrames(5);
}

void KTExposureSheet::insertTenFrames()
{
    insertFrames(10);
}

void KTExposureSheet::insertFrames(int n)
{
    int scene = k->scenes->currentIndex();
    int layer = k->currentTable->currentLayer();
    int target = k->currentTable->currentFrame() + 1;
    int lastFrame = k->currentTable->framesTotalAtCurrentLayer() - 1;

    for (int i=0; i<n; i++)
         insertFrame(layer, k->currentTable->framesTotalAtCurrentLayer());

    for (int index=lastFrame; index >= target; index--) {
         KTProjectRequest event = KTRequestBuilder::createFrameRequest(scene,
                                  layer, index,
                                  KTProjectRequest::Move, index + n);
         emit requestTriggered(&event);
    }

    selectFrame(layer, k->currentTable->currentFrame());
}

void KTExposureSheet::removeOne()
{
    k->fromMenu = true;
    k->actionBar->emitActionSelected(KTProjectActionBar::RemoveFrame);
}

void KTExposureSheet::lockFrame()
{
    k->actionBar->emitActionSelected(KTProjectActionBar::LockFrame);
}

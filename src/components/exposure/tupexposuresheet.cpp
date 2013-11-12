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

#include "tupexposuresheet.h"
#include "tapplication.h"
#include "tdebug.h"
#include "tglobal.h"
#include "toptionaldialog.h"
#include "tupprojectrequest.h"
#include "tuprequestbuilder.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupframe.h"

#include <QToolTip>
#include <QPixmap>
#include <QHBoxLayout>
#include <QList>
#include <QMenu>

struct TupExposureSheet::Private
{
    TupSceneTabWidget *scenesContainer;
    TupExposureTable *currentTable;
    TupProjectActionBar *actionBar;
    QMenu *menu;
    QString nameCopyFrame;
    bool fromMenu;
};

TupExposureSheet::TupExposureSheet(QWidget *parent) : TupModuleWidgetBase(parent, "Exposure Sheet"), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->currentTable = 0;
    k->fromMenu = false;
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QPixmap(kAppProp->themeDir() + "icons/exposure_sheet.png"));

    k->actionBar = new TupProjectActionBar(QString("Exposure"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameUp |
                        TupProjectActionBar::MoveFrameDown | 
                        TupProjectActionBar::LockFrame);

    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(applyAction(int)));
    addChild(k->actionBar, Qt::AlignCenter);

    k->scenesContainer = new TupSceneTabWidget(this);
    connect(k->scenesContainer, SIGNAL(currentChanged(int)), this, SLOT(emitRequestChangeScene(int)));
    addChild(k->scenesContainer);
    createMenu();
}

TupExposureSheet::~TupExposureSheet()
{
    delete k;
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupExposureSheet::createMenu()
{
    k->menu = new QMenu(tr("actions"));
    //k->menu->addAction(tr("Insert layer"))->setData(TupProjectActionBar::InsertLayer);
    //k->menu->addAction(tr("Remove layer"))->setData(TupProjectActionBar::RemoveLayer);

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

    QAction *insertTwenty = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("20 frames"), this);
    insertTwenty->setIconVisibleInMenu(true);
    insertMenu->addAction(insertTwenty);
    connect(insertTwenty, SIGNAL(triggered()), this, SLOT(insertTwentyFrames()));

    QAction *insertFifty = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("50 frames"), this);
    insertFifty->setIconVisibleInMenu(true);
    insertMenu->addAction(insertFifty);
    connect(insertFifty, SIGNAL(triggered()), this, SLOT(insertFiftyFrames()));

    QAction *insertHundred = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("100 frames"), this);
    insertHundred->setIconVisibleInMenu(true);
    insertMenu->addAction(insertHundred);
    connect(insertHundred, SIGNAL(triggered()), this, SLOT(insertHundredFrames()));

    k->menu->addMenu(insertMenu);

    QAction *removeOne = new QAction(QIcon(THEME_DIR + "icons/remove_frame.png"), tr("Remove frame"), this);
    removeOne->setIconVisibleInMenu(true);
    k->menu->addAction(removeOne);
    connect(removeOne, SIGNAL(triggered()), this, SLOT(removeOne()));

    QAction *clearAction = new QAction(QIcon(THEME_DIR + "icons/new.png"), tr("Clear frame"), this);
    clearAction->setIconVisibleInMenu(true);
    k->menu->addAction(clearAction);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearFrame()));

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
    //connect(expandMenu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));

    QMenu *timeLineMenu = new QMenu(tr("Copy TL forward"));
    timeLineMenu->addAction(QIcon(THEME_DIR + "icons/copy.png"), tr("1 time"), this, SLOT(copyTimeLineOnce()));
    timeLineMenu->addAction(QIcon(THEME_DIR + "icons/copy.png"), tr("2 times"), this, SLOT(copyTimeLineTwoTimes()));
    timeLineMenu->addAction(QIcon(THEME_DIR + "icons/copy.png"), tr("3 times"), this, SLOT(copyTimeLineThreeTimes()));
    timeLineMenu->addAction(QIcon(THEME_DIR + "icons/copy.png"), tr("4 times"), this, SLOT(copyTimeLineFourTimes()));
    timeLineMenu->addAction(QIcon(THEME_DIR + "icons/copy.png"), tr("5 times"), this, SLOT(copyTimeLineFiveTimes()));
    k->menu->addMenu(timeLineMenu);

    connect(k->menu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));
}

void TupExposureSheet::addScene(int index, const QString &name)
{
    #ifdef K_DEBUG
           T_FUNCINFO << " index: " << index << " name: " << name;
    #endif

    TupExposureTable *newScene = new TupExposureTable;
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

    k->scenesContainer->addScene(index, name, newScene);
}

void TupExposureSheet::renameScene(int index, const QString &name)
{
    k->scenesContainer->renameScene(index, name);
}

void TupExposureSheet::applyAction(int action)
{
    #ifdef K_DEBUG
           T_FUNCINFO << "TupExposureSheet::applyAction() - action: " << action;
    #endif

    k->currentTable = k->scenesContainer->getCurrentTable();

    if (k->currentTable == 0) {
        #ifdef K_DEBUG
               tFatal() << "TupExposureSheet::applyAction: No layer view!!" << endl;
        #endif
        return;
    }

    switch (action) {

            case TupProjectActionBar::InsertLayer:
               {
                 int layer = k->currentTable->columnCount();

                 TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(),
                                                                                 layer, TupProjectRequest::Add, tr("Layer %1").arg(layer + 1));
                 emit requestTriggered(&request);

                 int framesNum = k->currentTable->usedFrames(k->currentTable->currentColumn());

                 for (int i=0;i < framesNum;i++) {
                      request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), layer, i, 
                                                                     TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                      emit requestTriggered(&request);
                 }
               }
               break;

            case TupProjectActionBar::RemoveLayer:
               {
                 TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), 
                                                                                 k->currentTable->currentLayer(), 
                                                                                 TupProjectRequest::Remove);
                 emit requestTriggered(&request);
               }
               break;

            case TupProjectActionBar::InsertFrame:
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

            case TupProjectActionBar::RemoveFrame:
               {
                 int scene = k->scenesContainer->currentIndex();
                 int layer = k->currentTable->currentLayer();
                 int lastFrame = k->currentTable->framesTotalAtCurrentLayer() - 1;
                 int target = k->currentTable->currentFrame();

                 if (k->currentTable->currentRow() > lastFrame)
                     return;

                 if (k->currentTable->frameIsLocked(layer, target))
                     k->actionBar->emitActionSelected(TupProjectActionBar::LockFrame);

                 if (k->currentTable->framesTotalAtCurrentLayer() == 1) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, target, 
                                                                                     TupProjectRequest::Reset);
                     emit requestTriggered(&request);
                     k->fromMenu = false; 
                     return;
                 }

                 // SQA: Take care about the first frame case and paint a message on the workspace 
                 if (target == lastFrame) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, target, 
                                                                                     TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     if (target > 0)
                         selectFrame(layer, target-1);
                     else 
                         k->currentTable->clearSelection();
                 } else {
                     // When the item deleted is not the last one

                     for (int index=target+1; index <= lastFrame; index++) {
                          TupExposureTable::FrameType type;
                          type = k->currentTable->frameState(layer, index);
                          k->currentTable->updateFrameState(layer, index - 1, type);

                          QString label = k->currentTable->frameName(layer, index);
                          renameFrame(layer, index - 1, label);

                          TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, index, 
                                                                                          TupProjectRequest::Move, index - 1);
                          emit requestTriggered(&request);
                     }

                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, lastFrame, TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     selectFrame(layer, target);
                 }

                 k->fromMenu = false;
               }
               break;

            case TupProjectActionBar::MoveFrameUp:
               {
                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                            k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                            TupProjectRequest::Exchange, k->currentTable->currentFrame()-1);
                 emit requestTriggered(&request);
               }
               break;

            case TupProjectActionBar::MoveFrameDown:
               {
                 if (k->currentTable->currentFrame()+1 == k->currentTable->framesTotalAtCurrentLayer())
                     insertFrames(1);

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                            k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                            TupProjectRequest::Exchange, k->currentTable->currentFrame()+1);
                 emit requestTriggered(&request);
               }
               break;

            case TupProjectActionBar::LockFrame:
               {
                 bool locked = k->currentTable->frameIsLocked(k->currentTable->currentColumn(), 
                               k->currentTable->currentFrame());

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                            k->currentTable->currentLayer(), k->currentTable->currentFrame(),
                                            TupProjectRequest::Lock, !locked);
                 emit requestTriggered(&request);
               }
               break;
    }
}

void TupExposureSheet::setScene(int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->scenesContainer->count() >= index) {
        k->scenesContainer->blockSignals(true);
        k->scenesContainer->setCurrentIndex(index);		
        k->currentTable = k->scenesContainer->getTable(index);
        // k->currentTable = k->scenesContainer->getCurrentTable();
        k->scenesContainer->blockSignals(false);
    } else {
        #ifdef K_DEBUG
               tError() << "TupExposureSheet::setScene() - Invalid scene index -> " << index;
               tError() << "TupExposureSheet::setScene() - Scenes total -> " << k->scenesContainer->count(); 
        #endif
    }
}

void TupExposureSheet::emitRequestChangeScene(int index)
{
    if (k->scenesContainer->count() > 1) {
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupExposureSheet::emitRequestCopyCurrentFrame()
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                                    k->currentTable->currentLayer(), 
                                                                    k->currentTable->currentFrame(), 
                                                                    TupProjectRequest::Copy);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::emitRequestPasteInCurrentFrame()
{
    if (k->nameCopyFrame.isEmpty()) {
        #ifdef K_DEBUG
               tError() << "TupExposureSheet::emitRequestPasteInCurrentFrame() - The copied frame name is empty!";
        #endif

        return;
    }

    if (k->currentTable->usedFrames(k->currentTable->currentLayer()) <= k->currentTable->currentRow()) {
        for(int i = k->currentTable->usedFrames(k->currentTable->currentLayer()); 
            i <= k->currentTable->currentRow(); i++) {
            insertFrame(k->currentTable->currentLayer(), i);

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                         k->currentTable->currentLayer(), i, 
                                                         TupProjectRequest::Paste);
            emit localRequestTriggered(&request);
        }
    } else {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                         k->currentTable->currentLayer(), 
                                                         k->currentTable->currentFrame(),
                                                         TupProjectRequest::Paste);
            emit localRequestTriggered(&request);
    }
}

void TupExposureSheet::emitRequestExpandCurrentFrame(int n)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(),
                                                 k->currentTable->currentLayer(),
                                                 k->currentTable->currentFrame(),
                                                 TupProjectRequest::Expand, n);
    emit requestTriggered(&request);
}

void TupExposureSheet::expandCurrentFrameOnce()
{
    emitRequestExpandCurrentFrame(1);
}

void TupExposureSheet::expandCurrentFrameFive()
{
    emitRequestExpandCurrentFrame(5);
}

void TupExposureSheet::expandCurrentFrameTen()
{
    emitRequestExpandCurrentFrame(10);
}


void TupExposureSheet::insertFrame(int indexLayer, int indexFrame)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                 indexLayer, indexFrame, TupProjectRequest::Add, tr("Frame %1").arg(indexFrame + 1));
    emit requestTriggered(&request);
}

void TupExposureSheet::renameFrame(int indexLayer, int indexFrame, const QString & name)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex() , indexLayer, indexFrame,
                                                 TupProjectRequest::Rename, name);
    emit requestTriggered(&request);
}

void TupExposureSheet::selectFrame(int indexLayer, int indexFrame)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif 

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), indexLayer, 
                                                 indexFrame, TupProjectRequest::Select, "1");
    emit localRequestTriggered(&request);
    // emit requestTriggered(&request);
}

void TupExposureSheet::changeVisibilityLayer(int visualIndexLayer, bool visibility)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), visualIndexLayer,
                                                  TupProjectRequest::View, visibility);
    emit localRequestTriggered(&request);
    // emit requestTriggered(&event);
}

void TupExposureSheet::renameLayer(int indexLayer, const QString & name)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), indexLayer,
                                                 TupProjectRequest::Rename, name);
    emit requestTriggered(&request);
}

void TupExposureSheet::moveLayer(int oldIndex, int newIndex)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), oldIndex,
                                                 TupProjectRequest::Move, newIndex);
    emit requestTriggered(&request);
}

void TupExposureSheet::actionTriggered(QAction *action)
{
    bool ok;
    int id = action->data().toInt(&ok);

    if (ok)
        applyAction(id);
}

void TupExposureSheet::closeAllScenes()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->scenesContainer->blockSignals(true);

    delete k->currentTable;
    k->scenesContainer->removeAllTabs();
    k->currentTable = 0;

    k->scenesContainer->blockSignals(false);
}

void TupExposureSheet::sceneResponse(TupSceneResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("exposure");
    #endif

    switch(e->action()) {
           case TupProjectRequest::Add:
            {
                addScene(e->sceneIndex(), e->arg().toString());
            }
           break;
           case TupProjectRequest::Remove:
            {
                k->scenesContainer->removeScene(e->sceneIndex());

                /*
                k->currentTable = k->scenesContainer->getCurrentTable();
                int layer = k->currentTable->currentLayer();
                int frame = k->currentTable->currentFrame() + 1;
              
                if (e->sceneIndex() == 0 && k->scenesContainer->count() == 1) {
                    TupProjectRequest request = TupRequestBuilder::createFrameRequest(0, layer,
                                               frame, TupProjectRequest::Select, "1");
                    emit requestTriggered(&request);
                } else {
                    if (e->sceneIndex() > 0) {
                        TupProjectRequest request = TupRequestBuilder::createFrameRequest(e->sceneIndex() - 1, layer,
                                                   frame, TupProjectRequest::Select, "1");
                        emit requestTriggered(&request);
                    }
                }
                */
            }
           break;
           case TupProjectRequest::Reset:
            {
                setScene(e->sceneIndex());
                renameScene(e->sceneIndex(), e->arg().toString());

                TupProjectRequest request = TupRequestBuilder::createFrameRequest(e->sceneIndex(), 0,
                                            0, TupProjectRequest::Select, "1");
                emit requestTriggered(&request);

                k->currentTable->reset();
            }
           break;
           case TupProjectRequest::Move:
            {
            }
           break;
           case TupProjectRequest::Lock:
            {
            }
           break;
           case TupProjectRequest::Rename:
            {
                renameScene(e->sceneIndex(), e->arg().toString());
            }
           break;
           case TupProjectRequest::Select:
            {
                setScene(e->sceneIndex());
                if (k->currentTable && k->scenesContainer) {
                    k->scenesContainer->blockSignals(true);
                    k->currentTable->selectFrame(0, 0);
                    k->scenesContainer->blockSignals(false);
                }
            }
           break;
    }
}

void TupExposureSheet::layerResponse(TupLayerResponse *e)
{
    TupExposureTable *table = k->scenesContainer->getTable(e->sceneIndex());

    if (table) {
        switch (e->action()) {
                case TupProjectRequest::Add:
                 {
                     table->insertLayer(e->layerIndex(), e->arg().toString());
                 }
                break;
                case TupProjectRequest::Remove:
                 {
                     table->removeLayer(e->layerIndex());

                     if (table->layersTotal() == 0) {
                         TupProjectRequest request = TupRequestBuilder::createLayerRequest(0, 0, TupProjectRequest::Add, tr("Layer %1").arg(1));
                         emit requestTriggered(&request);

                         request = TupRequestBuilder::createFrameRequest(0, 0, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
                         emit requestTriggered(&request);
                     }
                 }
                break;
                case TupProjectRequest::Move:
                 {
                     table->moveLayer(e->layerIndex(), e->arg().toInt());
                 }
                break;
                case TupProjectRequest::Rename:
                 {
                     table->setLayerName(e->layerIndex(), e->arg().toString());
                 }
                break;
                case TupProjectRequest::Lock:
                 {
                     table->setLockLayer(e->layerIndex(), e->arg().toBool());
                 }
                break;
                case TupProjectRequest::Select:
                 {
                     setScene(e->sceneIndex());
                     table->blockSignals(true);
                     table->selectFrame(e->layerIndex(), 0);
                     table->blockSignals(false);
                 }
                case TupProjectRequest::View:
                 {
                     table->setVisibilityChanged(e->layerIndex(), e->arg().toBool());
                 }
                break;
                default:
                     #ifdef K_DEBUG
                            tFatal() << "TupExposureSheet::layerResponse - Layer option undefined! -> " << e->action();
                     #endif
                break;
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupExposureSheet::layerResponse -> Scene index invalid: " << e->sceneIndex();
        #endif
    }
}

void TupExposureSheet::frameResponse(TupFrameResponse *e)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupExposureTable *table = k->scenesContainer->getTable(e->sceneIndex());

    if (table) {
        switch (e->action()) {
                case TupProjectRequest::Add:
                 {
                     table->insertFrame(e->layerIndex(), e->frameIndex(), e->arg().toString(), e->external());

                     if (e->layerIndex() == 0 && e->frameIndex() == 0) {
                         setScene(e->sceneIndex());
                         table->selectFrame(0, 0);
                     }
                 }
                break;
                case TupProjectRequest::Remove:
                 {
                     table->removeFrame(e->layerIndex(), e->frameIndex(), k->fromMenu);
                 }
                break;
                case TupProjectRequest::Reset:
                 {
                     table->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Empty);
                     return;
                 }
                break;
                case TupProjectRequest::Exchange:
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
                case TupProjectRequest::Move:
                 {
                     // No action required for this specific request
                     return;
                 }
                break;
                case TupProjectRequest::Lock:
                 {
                     table->setLockFrame(e->layerIndex(), e->frameIndex(),  e->arg().toBool());
                 }
                break;
                case TupProjectRequest::Rename:
                 {
                     table->setFrameName(e->layerIndex(), e->frameIndex(), e->arg().toString());
                 }
                break;
                case TupProjectRequest::Select:
                 {
                     table->blockSignals(true);
                     // setScene(e->sceneIndex());
                     table->selectFrame(e->layerIndex(), e->frameIndex());
                     table->blockSignals(false);
                 }
                break;
                case TupProjectRequest::Expand:
                 {
                     tFatal() << "TupExposureSheet::frameResponse - Expand! -> Just Tracing!";
                     tFatal() << "TupExposureSheet::frameResponse - Starting point: -> " << e->frameIndex();
                     tFatal() << "TupExposureSheet::frameResponse - Range: -> " << e->arg().toInt();
                     for(int i = 0; i < e->arg().toInt(); i++)
                         table->insertFrame(e->layerIndex(), e->frameIndex()+i+1, 
                                            table->frameName(e->layerIndex(), e->frameIndex()), 
                                            e->external());
                 }
                break;
                case TupProjectRequest::Copy:
                 {
                     k->nameCopyFrame = table->frameName(e->layerIndex(), e->frameIndex());
                 }
                break;
                case TupProjectRequest::Paste:
                 {
                     if (e->frameIndex() >= table->usedFrames(e->layerIndex())) {
                         if (e->mode() == TupProjectResponse::Undo) {
                             if (e->arg().toString().isEmpty())
                                 table->removeFrame(e->layerIndex(), e->frameIndex(), false);
                         } else {
                                 table->insertFrame(e->layerIndex(), e->frameIndex(), 
                                                     k->nameCopyFrame + "- copy", e->external());
                         }
                     }

                     if (e->frameIsEmpty())
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Empty);
                     else
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Used);
                 }
                break;
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupExposureSheet::frameResponse - [ Fatal Error ] - Scene index is invalid -> " << e->sceneIndex();
        #endif
    }
}

void TupExposureSheet::itemResponse(TupItemResponse *e)
{
    switch (e->action()) {
            case TupProjectRequest::Add:
                 {
                     if (e->spaceMode() == TupProject::FRAMES_EDITION && e->itemIndex() == 0)
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Used);
                 }
                 break;
            case TupProjectRequest::Remove:
                 {
                     if (e->spaceMode() == TupProject::FRAMES_EDITION && e->frameIsEmpty())
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Empty);
                 }
                 break;
            case TupProjectRequest::SetTween:
                 {
                     // tError() << "TupExposureSheet::itemResponse() - Tracing!";
                 }
                 break;
            default:
                 break;
    }
}

void TupExposureSheet::libraryResponse(TupLibraryResponse *e)
{
    switch (e->action()) {
            case TupProjectRequest::Add:
            case TupProjectRequest::InsertSymbolIntoFrame:
                 {
                     if (e->spaceMode() == TupProject::FRAMES_EDITION && !e->frameIsEmpty() 
                         && k->currentTable->frameState(e->layerIndex(), e->frameIndex()) == TupExposureTable::Empty)
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Used);
                 }
                 break;
            case TupProjectRequest::Remove:
                 {
                     if (e->spaceMode() == TupProject::FRAMES_EDITION && e->frameIsEmpty())
                         k->currentTable->updateFrameState(e->layerIndex(), e->frameIndex(), TupExposureTable::Used);
                 }
                 break;
            default:
                 break;
    }
}

void TupExposureSheet::insertOneFrame()
{
    insertFrames(1);
}

void TupExposureSheet::insertFiveFrames()
{
    insertFrames(5);
}

void TupExposureSheet::insertTenFrames()
{
    insertFrames(10);
}

void TupExposureSheet::insertTwentyFrames()
{
    insertFrames(20);
}

void TupExposureSheet::insertFiftyFrames()
{
    insertFrames(50);
}

void TupExposureSheet::insertHundredFrames()
{
    insertFrames(100);
}

void TupExposureSheet::insertFrames(int n)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scene = k->scenesContainer->currentIndex();
    int layer = k->currentTable->currentLayer();
    int target = k->currentTable->currentFrame() + 1;
    int lastFrame = k->currentTable->framesTotalAtCurrentLayer() - 1;

    for (int i=0; i<n; i++)
         insertFrame(layer, k->currentTable->framesTotalAtCurrentLayer());

    for (int index=lastFrame; index >= target; index--) {
         TupProjectRequest event = TupRequestBuilder::createFrameRequest(scene,
                                  layer, index, TupProjectRequest::Move, index + n);
         emit requestTriggered(&event);
    }
   
    selectFrame(layer, k->currentTable->currentFrame());
}

void TupExposureSheet::removeOne()
{
    k->fromMenu = true;
    k->actionBar->emitActionSelected(TupProjectActionBar::RemoveFrame);
}

void TupExposureSheet::clearFrame()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scene = k->scenesContainer->currentIndex();
    int layer = k->currentTable->currentLayer();
    int frame = k->currentTable->currentFrame();

    TupProjectRequest event = TupRequestBuilder::createFrameRequest(scene, layer, frame, TupProjectRequest::Reset);
    emit requestTriggered(&event);

    k->currentTable->updateFrameState(layer, frame, TupExposureTable::Empty);
}

void TupExposureSheet::lockFrame()
{
    k->actionBar->emitActionSelected(TupProjectActionBar::LockFrame);
}

void TupExposureSheet::updateFramesState(TupProject *project)
{
    for (int i=0; i < project->scenesTotal(); i++) {
         TupScene *scene = project->scene(i);
         TupExposureTable *tab = k->scenesContainer->getTable(i);
         for (int j=0; j < scene->layersTotal(); j++) {
              TupLayer *layer = scene->layer(j);
              for (int k=0; k < layer->framesTotal(); k++) {
                   TupFrame *frame = layer->frame(k);
                   if (frame->isEmpty())
                       tab->updateFrameState(j, k, TupExposureTable::Empty);
                   else
                       tab->updateFrameState(j, k, TupExposureTable::Used);
              }
         }
    }
}


void TupExposureSheet::copyTimeLineOnce()
{
    copyTimeLine(1);
}

void TupExposureSheet::copyTimeLineTwoTimes()
{
    copyTimeLine(2);
}

void TupExposureSheet::copyTimeLineThreeTimes()
{
    copyTimeLine(3);
}

void TupExposureSheet::copyTimeLineFourTimes()
{
    copyTimeLine(4);
}

void TupExposureSheet::copyTimeLineFiveTimes()
{
    copyTimeLine(5);
}

void TupExposureSheet::copyTimeLine(int times) 
{
    int currentScene = k->scenesContainer->currentIndex();
    int currentLayer = k->currentTable->currentLayer();
    int currentFrame = k->currentTable->currentFrame();
    int framesTotal = k->currentTable->usedFrames(k->currentTable->currentLayer());

    for (int i=0; i < times; i++) {
         for (int j=0; j < framesTotal; j++) {
              TupProjectRequest request = TupRequestBuilder::createFrameRequest(currentScene,
                                                                                currentLayer,
                                                                                j,
                                                                                TupProjectRequest::Copy);
              emit localRequestTriggered(&request);
              int frameIndex = k->currentTable->usedFrames(currentLayer);
              insertFrame(currentLayer, frameIndex);

              request = TupRequestBuilder::createFrameRequest(currentScene,
                                                              currentLayer, frameIndex,
                                                              TupProjectRequest::Paste);
              emit localRequestTriggered(&request);
         }
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(currentScene,                                   
                                                                      currentLayer,                                 
                                                                      currentFrame,      
                                                                      TupProjectRequest::Select, "1");
    emit requestTriggered(&request);
}


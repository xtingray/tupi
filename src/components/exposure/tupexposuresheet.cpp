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

struct TupExposureSheet::Private
{
    TupProject *project;
    TupSceneTabWidget *scenesContainer;
    TupExposureTable *currentTable;
    TupProjectActionBar *actionBar;
    QMenu *menu;
    QString nameCopyFrame;
    bool fromMenu;
    bool localRequest;
    int previousScene;
    int previousLayer;
};

TupExposureSheet::TupExposureSheet(QWidget *parent, TupProject *project) : TupModuleWidgetBase(parent, "Exposure Sheet"), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet()]";
        #else
            TINIT;
        #endif
    #endif

    k->project = project;
    k->currentTable = 0;
    k->fromMenu = false;
    k->localRequest = false;
    k->previousScene = 0;
    k->previousLayer = 0;

    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QPixmap(kAppProp->themeDir() + "icons/exposure_sheet.png"));

    k->actionBar = new TupProjectActionBar(QString("Exposure"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameBackward |
                        TupProjectActionBar::MoveFrameForward | 
                        TupProjectActionBar::LockFrame);

    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(applyAction(int)));
    addChild(k->actionBar, Qt::AlignCenter);

    k->scenesContainer = new TupSceneTabWidget(this);
    connect(k->scenesContainer, SIGNAL(currentChanged(int)), this, SLOT(requestChangeScene(int)));
    connect(k->scenesContainer, SIGNAL(updateLayerOpacity(double)), this, SLOT(requestUpdateLayerOpacity(double)));
    addChild(k->scenesContainer);
    createMenu();
}

TupExposureSheet::~TupExposureSheet()
{
    delete k;

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupExposureSheet()]";
        #else
            TEND;
        #endif
    #endif
}

void TupExposureSheet::createMenu()
{
    k->menu = new QMenu(tr("actions"));
    QMenu *insertMenu = new QMenu(tr("Insert"));

    QAction *insertOne = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("1 frame"), this); 
    insertOne->setIconVisibleInMenu(true);
    insertMenu->addAction(insertOne);

    QAction *insertFive = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("5 frames"), this);                        
    insertFive->setIconVisibleInMenu(true);
    insertMenu->addAction(insertFive);

    QAction *insertTen = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("10 frames"), this);
    insertTen->setIconVisibleInMenu(true);
    insertMenu->addAction(insertTen);

    QAction *insertTwenty = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("20 frames"), this);
    insertTwenty->setIconVisibleInMenu(true);
    insertMenu->addAction(insertTwenty);

    QAction *insertFifty = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("50 frames"), this);
    insertFifty->setIconVisibleInMenu(true);
    insertMenu->addAction(insertFifty);

    QAction *insertHundred = new QAction(QIcon(THEME_DIR + "icons/add_frame.png"), tr("100 frames"), this);
    insertHundred->setIconVisibleInMenu(true);
    insertMenu->addAction(insertHundred);

    connect(insertMenu, SIGNAL(triggered(QAction *)), this, SLOT(insertFramesFromMenu(QAction*)));

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
    connect(copyAction, SIGNAL(triggered()), this, SLOT(requestCopyCurrentFrame()));

    QAction *pasteAction = new QAction(QIcon(THEME_DIR + "icons/paste.png"), tr("Paste in frame"), this);
    pasteAction->setIconVisibleInMenu(true);
    k->menu->addAction(pasteAction);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(requestPasteInCurrentFrame()));

    QMenu *timeLineMenu = new QMenu(tr("Copy TL forward"));

    QAction *copyTLOnce = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("1 time"), this);
    copyTLOnce->setIconVisibleInMenu(true);
    timeLineMenu->addAction(copyTLOnce);

    QAction *copyTLTwice = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("2 times"), this);
    copyTLTwice->setIconVisibleInMenu(true);
    timeLineMenu->addAction(copyTLTwice);

    QAction *copyTLThird = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("3 times"), this);
    copyTLThird->setIconVisibleInMenu(true);
    timeLineMenu->addAction(copyTLThird);

    QAction *copyTLFourth = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("4 times"), this);
    copyTLFourth->setIconVisibleInMenu(true);
    timeLineMenu->addAction(copyTLFourth);

    QAction *copyTLFifth = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("5 times"), this);
    copyTLFifth->setIconVisibleInMenu(true);
    timeLineMenu->addAction(copyTLFifth);

    connect(timeLineMenu, SIGNAL(triggered(QAction *)), this, SLOT(copyTimeLineFromMenu(QAction*)));

    k->menu->addMenu(timeLineMenu);
    connect(k->menu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));
}

void TupExposureSheet::addScene(int index, const QString &name)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::addScene()] - index: " << QString::number(index) << " name: " << name;
        #else
            T_FUNCINFO << " index: " << index << " name: " << name;
        #endif
    #endif

    TupExposureTable *scene = new TupExposureTable;
    scene->setMenu(k->menu);

    connect(scene, SIGNAL(frameUsed(int, int)), this, SLOT(insertFrame(int, int)));
    connect(scene, SIGNAL(frameRenamed(int, int, const QString &)), this, SLOT(renameFrame(int, int, const QString &)));
    connect(scene, SIGNAL(frameSelected(int, int)), SLOT(selectFrame(int, int)));
    connect(scene, SIGNAL(layerNameChanged(int, const QString &)), this, SLOT(requestRenameLayer(int, const QString &)));
    connect(scene, SIGNAL(layerMoved(int, int)), this, SLOT(moveLayer(int, int)));
    connect(scene, SIGNAL(layerVisibilityChanged(int, bool)), this, SLOT(changeVisibilityLayer(int, bool)));

    k->scenesContainer->addScene(index, name, scene);
}

void TupExposureSheet::renameScene(int index, const QString &name)
{
    k->scenesContainer->renameScene(index, name);
}

void TupExposureSheet::applyAction(int action)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "TupExposureSheet::applyAction() - action: " << QString::number(action);
        #else
            T_FUNCINFO << "TupExposureSheet::applyAction() - action: " << action;
        #endif
    #endif

    k->currentTable = k->scenesContainer->getCurrentTable();
    if (k->currentTable == 0) {
        #ifdef K_DEBUG
            QString msg = "TupExposureSheet::applyAction: No layer view!!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
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
                                                                      TupProjectRequest::Add, tr("Frame"));
                                                                      // TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
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
                 if (k->currentTable->currentRow() >= usedFrames) {
                     // SQA: Check if this code is really used
                     for (int layer=0; layer < k->currentTable->layersCount(); layer++) { 
                          if (usedFrames >= k->currentTable->usedFrames(layer)) {
                              int finish = k->currentTable->currentFrame() + 1;
                              for (int frame = usedFrames; frame <= finish; frame++)
                                   insertFrame(layer, frame);
                          }
                     }
                 } else {
                     insertFrames(1);
                 }
               }
               break;

            case TupProjectActionBar::RemoveFrame:
               {
                 k->localRequest = true;
                 int scene = k->scenesContainer->currentIndex();
                 int layer = k->currentTable->currentLayer();
                 int lastFrame = k->currentTable->framesCountAtCurrentLayer() - 1;
                 int target = k->currentTable->currentFrame();

                 if (k->currentTable->currentRow() > lastFrame)
                     return;

                 // SQA: Check if this condition is really working
                 if (k->currentTable->frameIsLocked(layer, target))
                     k->actionBar->emitActionSelected(TupProjectActionBar::LockFrame);

                 if (k->currentTable->framesCountAtCurrentLayer() == 1) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, target, TupProjectRequest::Reset);
                     emit requestTriggered(&request);
                     k->fromMenu = false; 
                     return;
                 }

                 // SQA: Take care about the first frame case and paint a message on the workspace 
                 if (target == lastFrame) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, target, TupProjectRequest::Remove);
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

                          TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, index, TupProjectRequest::Exchange, index - 1);
                          emit requestTriggered(&request);
                     }

                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, layer, lastFrame, TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     selectFrame(layer, target);
                 }

                 k->fromMenu = false;
               }
               break;

            case TupProjectActionBar::MoveFrameBackward:
               {
                 int frameIndex = k->currentTable->currentFrame();
                 if (frameIndex > 0) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                 k->currentTable->currentLayer(),  frameIndex,
                                                 TupProjectRequest::Exchange, k->currentTable->currentFrame()-1);
                     emit requestTriggered(&request);
                 }
               }
               break;

            case TupProjectActionBar::MoveFrameForward:
               {
                 int origin = k->currentTable->currentFrame();
                 int destination = k->currentTable->currentFrame() + 1;
                 if (destination == k->currentTable->framesCountAtCurrentLayer())
                     insertFrames(1);

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                             k->currentTable->currentLayer(), origin,
                                             TupProjectRequest::Exchange, destination);
                 emit requestTriggered(&request);
               }
               break;

            case TupProjectActionBar::LockFrame:
               {
                 bool locked = k->currentTable->frameIsLocked(k->currentTable->currentColumn(), k->currentTable->currentFrame());

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
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::setScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->scenesContainer->isTableIndexValid(index)) {
        k->scenesContainer->blockSignals(true);
        k->scenesContainer->setCurrentIndex(index);		
        k->currentTable = k->scenesContainer->getTable(index);
        k->scenesContainer->blockSignals(false);
    } else {
        #ifdef K_DEBUG
            QString msg1 = "TupExposureSheet::setScene() - Invalid scene index -> " + QString::number(index);
            QString msg2 = "TupExposureSheet::setScene() - Scenes total -> " + QString::number(k->scenesContainer->count());
            #ifdef Q_OS_WIN
                qDebug() << msg1;
                qDebug() << msg2;
            #else
                tError() << msg1;
                tError() << msg2;
            #endif
        #endif
    }
}

void TupExposureSheet::requestChangeScene(int index)
{
    if (k->scenesContainer->count() > 1) {
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupExposureSheet::requestCopyCurrentFrame()
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                                    k->currentTable->currentLayer(), 
                                                                    k->currentTable->currentFrame(), 
                                                                    TupProjectRequest::Copy);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::requestPasteInCurrentFrame()
{
    if (k->nameCopyFrame.isEmpty()) {
        #ifdef K_DEBUG
            QString msg = "TupExposureSheet::requestPasteInCurrentFrame() - The copied frame name is empty!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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

void TupExposureSheet::requestExpandCurrentFrame(int n)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(),
                                                 k->currentTable->currentLayer(),
                                                 k->currentTable->currentFrame(),
                                                 TupProjectRequest::Expand, n);
    emit requestTriggered(&request);
}

void TupExposureSheet::insertFrame(int indexLayer, int indexFrame)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::insertFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                   indexLayer, indexFrame, TupProjectRequest::Add, tr("Frame"));
                                                   // indexLayer, indexFrame, TupProjectRequest::Add, tr("Frame %1").arg(indexFrame + 1));
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
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::selectFrame()] - Layer: " << indexLayer;
            qDebug() << "[TupExposureSheet::selectFrame()] - Frame: " << indexFrame;
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), indexLayer, 
                                                   indexFrame, TupProjectRequest::Select, "1");
    emit localRequestTriggered(&request);
}

void TupExposureSheet::changeVisibilityLayer(int visualIndexLayer, bool visibility)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), visualIndexLayer,
                                                   TupProjectRequest::View, visibility);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::requestRenameLayer(int indexLayer, const QString & name)
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
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::closeAllScenes()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->scenesContainer->blockSignals(true);
    k->currentTable = 0;
    k->scenesContainer->removeAllTabs();
    k->scenesContainer->blockSignals(false);
}

void TupExposureSheet::sceneResponse(TupSceneResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::sceneResponse()]";
        #else
            T_FUNCINFOX("exposure");
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    switch(response->action()) {
           case TupProjectRequest::Add:
            {
                if (response->mode() == TupProjectResponse::Do) {
                    addScene(sceneIndex, response->arg().toString());
                    return;
                }

                if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                    TupScene *scene = k->project->scene(sceneIndex);
                    if (scene)
                        k->scenesContainer->restoreScene(sceneIndex, scene->sceneName());
                    return;
                }
            }
           break;
           case TupProjectRequest::Remove:
            {
                k->scenesContainer->removeScene(sceneIndex);
            }
           break;
           case TupProjectRequest::Reset:
            {
                setScene(sceneIndex);
                renameScene(sceneIndex, response->arg().toString());

                TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, 0, 0, TupProjectRequest::Select, "1");
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
                renameScene(sceneIndex, response->arg().toString());
            }
           break;
           case TupProjectRequest::Select:
            {
                setScene(sceneIndex);
                if (k->currentTable && k->scenesContainer) {
                    k->scenesContainer->blockSignals(true);
                    k->currentTable->selectFrame(0, 0);
                    k->scenesContainer->blockSignals(false);
                    if (k->previousScene != sceneIndex) {
                        k->previousScene = sceneIndex;
                        k->previousLayer = 0;
                        updateLayerOpacity(sceneIndex, 0);
                    }
                }
            }
           break;
    }
}

void TupExposureSheet::layerResponse(TupLayerResponse *response)
{
    int sceneIndex = response->sceneIndex();
    TupExposureTable *framesTable = k->scenesContainer->getTable(sceneIndex);

    if (framesTable) {
        int layerIndex = response->layerIndex();
        switch (response->action()) {
                case TupProjectRequest::Add:
                 {
                     if (response->mode() == TupProjectResponse::Do) {
                         framesTable->insertLayer(layerIndex, response->arg().toString());
                         return;
                     }

                     if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                         TupScene *scene = k->project->scene(sceneIndex);
                         if (scene) {
                             TupLayer *layer = scene->layer(layerIndex); 
                             if (layer) {
                                 framesTable->insertLayer(layerIndex, layer->layerName());
                                 QList<TupFrame *> frames = layer->frames();
                                 int total = frames.count();
                                 for(int i=0; i<total; i++) {
                                     TupFrame *frame = frames.at(i);
                                     framesTable->insertFrame(layerIndex, i, frame->frameName(), response->external());
                                     if (!frame->isEmpty())
                                         framesTable->updateFrameState(layerIndex, i, TupExposureTable::Used);
                                 }
                             } 
                         }
                         return;
                     }
                 }
                break;
                case TupProjectRequest::Remove:
                 {
                     framesTable->removeLayer(layerIndex);
                 }
                break;
                case TupProjectRequest::Move:
                 {
                     framesTable->moveLayer(layerIndex, response->arg().toInt());
                 }
                break;
                case TupProjectRequest::Rename:
                 {
                     framesTable->setLayerName(layerIndex, response->arg().toString());
                 }
                break;
                case TupProjectRequest::Lock:
                 {
                     framesTable->setLockLayer(layerIndex, response->arg().toBool());
                 }
                break;
                case TupProjectRequest::Select:
                 {
                     setScene(sceneIndex);
                     framesTable->blockSignals(true);
                     framesTable->selectFrame(layerIndex, 0);
                     framesTable->blockSignals(false);

                     if (k->previousScene != sceneIndex || k->previousLayer != layerIndex) {
                         k->previousScene = sceneIndex;
                         k->previousLayer = layerIndex;
                         updateLayerOpacity(sceneIndex, layerIndex);
                     }
                 }
                case TupProjectRequest::View:
                 {
                     framesTable->setLayerVisibility(layerIndex, response->arg().toBool());
                 }
                break;
                default:
                     #ifdef K_DEBUG
                         QString msg = "TupExposureSheet::layerResponse - Layer option undefined! -> " + QString::number(response->action());
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tFatal() << msg;
                         #endif
                     #endif
                break;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureSheet::layerResponse -> Scene index invalid: " + QString::number(sceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
    }
}

void TupExposureSheet::frameResponse(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::frameResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    TupExposureTable *table = k->scenesContainer->getTable(sceneIndex);
    if (table) {
        int layerIndex = response->layerIndex();
        int frameIndex = response->frameIndex();
        switch (response->action()) {
                case TupProjectRequest::Add:
                 {
                     if (response->mode() == TupProjectResponse::Do) {
                         table->insertFrame(layerIndex, frameIndex, response->arg().toString(), response->external());
                         if (layerIndex == 0 && frameIndex == 0) {
                             setScene(sceneIndex);
                             table->selectFrame(0, 0);
                         }
                         return;
                     }

                     if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                         TupScene *scene = k->project->scene(sceneIndex);
                         if (scene) {
                             TupLayer *layer = scene->layer(layerIndex);
                             if (layer) {
                                 TupFrame *frame = layer->frame(frameIndex);
                                 table->insertFrame(layerIndex, frameIndex, frame->frameName(), response->external());
                                 if (!frame->isEmpty())
                                     table->updateFrameState(layerIndex, frameIndex, TupExposureTable::Used);
                             }
                         }
                         return;
                     }
                 }
                break;
                case TupProjectRequest::Remove:
                 {
                     if (response->mode() == TupProjectResponse::Do) {
                         if (k->localRequest) {
                             k->localRequest = false;
                             table->removeFrame(layerIndex, frameIndex, k->fromMenu);
                         } else {
                             // int layer = k->currentTable->currentLayer();
                             int lastFrame = k->currentTable->framesCountAtCurrentLayer() - 1;
                             int target = frameIndex;

                             if (target == lastFrame) {
                                 table->removeFrame(layerIndex, target, k->fromMenu);
                                 if (target <= 0)
                                     k->currentTable->clearSelection();
                             } else {
                                 // When the item deleted is not the last one
                                 int layer = k->currentTable->currentLayer();
                                 for (int index=target+1; index <= lastFrame; index++) {
                                      TupExposureTable::FrameType type;
                                      type = k->currentTable->frameState(layer, index);
                                      k->currentTable->updateFrameState(layer, index - 1, type);

                                      QString label = k->currentTable->frameName(layer, index);
                                      renameFrame(layer, index - 1, label);
                                 }
                                 table->removeFrame(layerIndex, lastFrame, k->fromMenu);
                             }
                         }
                         k->fromMenu = false;
                         return;
                     }

                     if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                         int lastFrame = k->currentTable->framesCountAtCurrentLayer() - 1;
                         int target = frameIndex;
                         if (target == lastFrame) {
                             table->removeFrame(layerIndex, frameIndex, k->fromMenu);
                             if (frameIndex > 0)
                                 frameIndex--;
                             table->selectFrame(layerIndex, frameIndex);
                         } else {
                             table->removeFrame(layerIndex, frameIndex, k->fromMenu);
                         }
                     }
                 }
                break;
                case TupProjectRequest::Reset:
                 {
                     table->updateFrameState(layerIndex, frameIndex, TupExposureTable::Empty);
                     return;
                 }
                break;
                case TupProjectRequest::Exchange:
                 {
                     table->exchangeFrame(layerIndex, frameIndex, layerIndex, response->arg().toInt(), response->external());
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
                     table->setLockFrame(layerIndex, frameIndex, response->arg().toBool());
                 }
                break;
                case TupProjectRequest::Rename:
                 {
                     table->setFrameName(layerIndex, frameIndex, response->arg().toString());
                 }
                break;
                case TupProjectRequest::Select:
                 {
                     table->blockSignals(true);
                     table->selectFrame(layerIndex, frameIndex);
                     table->blockSignals(false);
                     if (k->previousScene != sceneIndex || k->previousLayer != layerIndex) {
                         k->previousScene = sceneIndex;
                         k->previousLayer = layerIndex;
                         updateLayerOpacity(sceneIndex, layerIndex);
                     }
                 }
                break;
                case TupProjectRequest::Expand:
                 {
                     // SQA: It's very possible this feature will be deprecated. Please confirm!
                     for(int i = 0; i < response->arg().toInt(); i++)
                         table->insertFrame(layerIndex, frameIndex + i + 1, 
                                            table->frameName(layerIndex, frameIndex), 
                                            response->external());
                 }
                break;
                case TupProjectRequest::Copy:
                 {
                     k->nameCopyFrame = table->frameName(layerIndex, frameIndex);
                 }
                break;
                case TupProjectRequest::Paste:
                 {
                     if (frameIndex >= table->usedFrames(layerIndex)) {
                         if (response->mode() == TupProjectResponse::Undo) {
                             if (response->arg().toString().isEmpty())
                                 table->removeFrame(layerIndex, frameIndex, false);
                         } else {
                                 table->insertFrame(layerIndex, frameIndex, 
                                                    k->nameCopyFrame + "- copy", response->external());
                         }
                     }

                     if (response->frameIsEmpty())
                         k->currentTable->updateFrameState(layerIndex, frameIndex, TupExposureTable::Empty);
                     else
                         k->currentTable->updateFrameState(layerIndex, frameIndex, TupExposureTable::Used);
                 }
                break;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureSheet::frameResponse() - [ Fatal Error ] - Scene index is invalid -> " + QString::number(sceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
                     // SQA: Implement the code required to update frames state if they contain a tween 
                 }
                 break;
            default:
                 break;
    }
}

void TupExposureSheet::libraryResponse(TupLibraryResponse *response)
{
    switch (response->action()) {
            case TupProjectRequest::Add:
            case TupProjectRequest::InsertSymbolIntoFrame:
                 {
                     if (response->spaceMode() == TupProject::FRAMES_EDITION)
                         k->currentTable->updateFrameState(response->layerIndex(), response->frameIndex(), TupExposureTable::Used);
                 }
                 break;
            case TupProjectRequest::Remove:
                 {
                     updateFramesState();
                 }
                 break;
            default:
                 break;
    }
}

void TupExposureSheet::insertFrames(int n)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::insertFrames()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int scene = k->scenesContainer->currentIndex();
    int layer = k->currentTable->currentLayer();
    int target = k->currentTable->currentFrame() + 1;
    int lastFrame = k->currentTable->framesCountAtCurrentLayer() - 1;

    if (target > lastFrame) {
        for (int i=0; i<n; i++)
             insertFrame(layer, k->currentTable->framesCountAtCurrentLayer());
        selectFrame(layer, k->currentTable->currentFrame() + 1);
    } else {
        int frame = k->currentTable->currentFrame() + 1; 
        for (int i=0; i<n; i++)
             insertFrame(layer, k->currentTable->framesCountAtCurrentLayer());

        for (int index=lastFrame; index >= target; index--) {
             TupProjectRequest event = TupRequestBuilder::createFrameRequest(scene, layer, index, TupProjectRequest::Exchange, index + n);
             emit requestTriggered(&event);
        }

        /*
        lastFrame = k->currentTable->framesCountAtCurrentLayer() - 1;
        for (int index=target; index <= lastFrame; index++) {
             target++;
             TupProjectRequest event = TupRequestBuilder::createFrameRequest(scene, layer, index, TupProjectRequest::Rename, tr("Frame"));
             emit requestTriggered(&event);
        }
        */

        selectFrame(layer, frame);
    }
}

void TupExposureSheet::removeOne()
{
    k->fromMenu = true;
    k->actionBar->emitActionSelected(TupProjectActionBar::RemoveFrame);
}

void TupExposureSheet::clearFrame()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::clearFrame()]";
        #else
            T_FUNCINFO;
        #endif
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

void TupExposureSheet::updateFramesState()
{
    for (int i=0; i < k->project->scenesCount(); i++) {
         TupScene *scene = k->project->scene(i);
         TupExposureTable *tab = k->scenesContainer->getTable(i);
         for (int j=0; j < scene->layersCount(); j++) {
              TupLayer *layer = scene->layer(j);
              for (int k=0; k < layer->framesCount(); k++) {
                   TupFrame *frame = layer->frame(k);
                   if (frame->isEmpty())
                       tab->updateFrameState(j, k, TupExposureTable::Empty);
                   else
                       tab->updateFrameState(j, k, TupExposureTable::Used);
              }
         }
    }
}

void TupExposureSheet::copyTimeLine(int times) 
{
    int currentScene = k->scenesContainer->currentIndex();
    int currentLayer = k->currentTable->currentLayer();
    int currentFrame = k->currentTable->currentFrame();
    int framesCount = k->currentTable->usedFrames(k->currentTable->currentLayer());

    for (int i=0; i < times; i++) {
         for (int j=0; j < framesCount; j++) {
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

void TupExposureSheet::insertFramesFromMenu(QAction *action)
{
    QString actionName = action->text();

    if (actionName.compare(tr("1 frame")) == 0) {
        insertFrames(1);
        return;
    }

    if (actionName.compare(tr("5 frames")) == 0) {
        insertFrames(5);
        return;
    }

    if (actionName.compare(tr("10 frames")) == 0) {
        insertFrames(10);
        return;
    }

    if (actionName.compare(tr("20 frames")) == 0) {
        insertFrames(20);
        return;
    }

    if (actionName.compare(tr("50 frames")) == 0) {
        insertFrames(50);
        return;
    }

    if (actionName.compare(tr("100 frames")) == 0)
        insertFrames(100);
}

void TupExposureSheet::copyTimeLineFromMenu(QAction *action)
{
    QString actionName = action->text();

    if (actionName.compare(tr("1 time")) == 0) {
        copyTimeLine(1);
        return;
    }

    if (actionName.compare(tr("2 times")) == 0) {
        copyTimeLine(2);
        return;
    }

    if (actionName.compare(tr("3 times")) == 0) {
        copyTimeLine(3);
        return;
    }

    if (actionName.compare(tr("4 times")) == 0) {
        copyTimeLine(4);
        return;
    }

    if (actionName.compare(tr("5 times")) == 0) {
        copyTimeLine(5);
        return;
    }
}

void TupExposureSheet::requestUpdateLayerOpacity(double opacity)
{
    int layer = k->currentTable->currentLayer();
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(),
                                                                      layer, TupProjectRequest::UpdateOpacity, opacity);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::updateLayerOpacity(int sceneIndex, int layerIndex)
{
    double opacity = getLayerOpacity(sceneIndex, layerIndex);
    k->scenesContainer->setLayerOpacity(sceneIndex, opacity);
}

double TupExposureSheet::getLayerOpacity(int sceneIndex, int layerIndex)
{
    double opacity = 1.0;
    TupScene *scene = k->project->scene(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layer(layerIndex);
        if (layer) {
            opacity = layer->opacity();
        } else {
            #ifdef K_DEBUG
                QString msg = "TupExposureSheet::getLayerOpacity() - Fatal Error: No layer at index -> " + QString::number(layerIndex);
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureSheet::getLayerOpacity() - Fatal Error: No scene at index -> " + QString::number(sceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return opacity;
}

void TupExposureSheet::initLayerVisibility()
{
    int scenes = k->project->scenesCount(); 
    for (int sceneIndex=0; sceneIndex < scenes; sceneIndex++) {
         TupScene *scene = k->project->scene(sceneIndex);
         if (scene) {
             int layers = scene->layersCount();
             for (int layerIndex=0; layerIndex < layers; layerIndex++) {
                  TupLayer *layer = scene->layer(layerIndex);
                  TupProjectRequest request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::View, layer->isVisible());
                  emit localRequestTriggered(&request);
             }
         }
    }
}

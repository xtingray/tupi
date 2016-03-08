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

#include "tuptimeline.h"

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

struct TupTimeLine::Private
{
    Private() : scenesContainer(0), actionBar(0), selectedLayer(-1), library(0) {}
    
    TupSceneContainer *scenesContainer;
    TupProjectActionBar *actionBar;
    int selectedLayer; 
    TupProject *project;
    TupLibrary *library;
};

TupTimeLine::TupTimeLine(TupProject *project, QWidget *parent) : TupModuleWidgetBase(parent, "TupTimeLine"), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine()]";
        #else
            TINIT;
        #endif
    #endif
    
    setWindowTitle(tr("Time Line"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/time_line.png"));

    k->project = project;
    k->library = k->project->library();

    // SQA: Pending to add the feature "Layer Opacity" as part of this action bar
    
    k->actionBar = new TupProjectActionBar(QString("TimeLine"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::ExtendFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameBackward |
                        TupProjectActionBar::MoveFrameForward |
                        TupProjectActionBar::LockFrame |
                        TupProjectActionBar::InsertScene |
                        TupProjectActionBar::RemoveScene);

    addChild(k->actionBar, Qt::AlignCenter);
    
    k->scenesContainer = new TupSceneContainer(this);
    addChild(k->scenesContainer);
    
    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(requestCommand(int)));
    connect(k->scenesContainer, SIGNAL(currentChanged(int)), this, SLOT(requestSceneSelection(int)));
}

TupTimeLine::~TupTimeLine()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupTimeLine()]";
        #else
            TEND;
        #endif
    #endif

    delete k;
}

TupTimeLineTable *TupTimeLine::framesTable(int sceneIndex)
{
    TupTimeLineTable *framesTable = qobject_cast<TupTimeLineTable *>(k->scenesContainer->widget(sceneIndex));
    if (framesTable)
        return framesTable;
 
    return 0;
}

void TupTimeLine::addScene(int sceneIndex, const QString &name)
{
    if (sceneIndex < 0 || sceneIndex > k->scenesContainer->count())
        return;

    TupTimeLineTable *framesTable = new TupTimeLineTable(sceneIndex, k->scenesContainer);
    framesTable->setItemSize(10, 20);

    connect(framesTable, SIGNAL(frameSelected(int, int)), this, SLOT(selectFrame(int, int)));
    // connect(framesTable, SIGNAL(frameRemoved(int, int)), SLOT(removeFrameCopy(int, int)));
    connect(framesTable, SIGNAL(frameRemoved()), SLOT(removeFrameCopy()));
    connect(framesTable, SIGNAL(frameCopied(int, int)), SLOT(copyFrameForward(int, int)));
    connect(framesTable, SIGNAL(visibilityChanged(int, bool)), this, SLOT(requestLayerVisibilityAction(int, bool)));
    connect(framesTable, SIGNAL(layerNameChanged(int, const QString &)), this, SLOT(requestLayerRenameAction(int, const QString &))); 
    connect(framesTable, SIGNAL(layerMoved(int, int)), this, SLOT(requestLayerMove(int, int)));
    connect(framesTable, SIGNAL(newPerspective(int)), this, SIGNAL(newPerspective(int)));

    k->scenesContainer->addScene(sceneIndex, framesTable, name);
}

void TupTimeLine::removeScene(int sceneIndex)
{
    if (sceneIndex >= 0 && sceneIndex < k->scenesContainer->count())
        k->scenesContainer->removeScene(sceneIndex);
}

void TupTimeLine::closeAllScenes()
{
    k->scenesContainer->blockSignals(true);
    while (k->scenesContainer->currentWidget())
           delete k->scenesContainer->currentWidget();
    k->scenesContainer->blockSignals(false);
}

void TupTimeLine::sceneResponse(TupSceneResponse *response)
{
    // Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::sceneResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 if (response->mode() == TupProjectResponse::Do) {
                     addScene(response->sceneIndex(), response->arg().toString());
                     return;
                 } 

                 if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                     int sceneIndex = response->sceneIndex();
                     k->scenesContainer->restoreScene(sceneIndex, response->arg().toString());
                     TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
                     emit requestTriggered(&request);

                     return;
                 }
            }
            break;
            case TupProjectRequest::Remove:
            {
                 removeScene(response->sceneIndex());
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
            
            }
            break;
            case TupProjectRequest::Select:
            {
                 k->scenesContainer->setCurrentIndex(response->sceneIndex());
            }
            break;
            default:
                 #ifdef K_DEBUG
                     QString msg = "TupTimeLine::sceneResponse : Unknown action :/";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
            break;
    }
}

void TupTimeLine::layerResponse(TupLayerResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::layerResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    TupTimeLineTable *framesTable = this->framesTable(sceneIndex);
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
                         TupScene *scene = k->project->sceneAt(sceneIndex);
                         if (scene) {
                             TupLayer *layer = scene->layerAt(layerIndex);
                             if (layer) {
                                 framesTable->insertLayer(layerIndex, layer->layerName());
                                 QList<TupFrame *> frames = layer->frames();
                                 int total = frames.count();
                                 for(int i=0; i<total; i++)
                                     framesTable->insertFrame(layerIndex);
                             }
                         }
                         return; 
                     }
                }
                break;
                case TupProjectRequest::Remove:
                {
                     framesTable->removeLayer(layerIndex);
                     if (framesTable->layersCount() == 0) {
                         TupProjectRequest request = TupRequestBuilder::createLayerRequest(0, 0, TupProjectRequest::Add, tr("Layer %1").arg(1));
                         emit requestTriggered(&request);

                         request = TupRequestBuilder::createFrameRequest(0, 0, 0, TupProjectRequest::Add, tr("Frame"));
                         emit requestTriggered(&request);
                     }
                }
                break;
                case TupProjectRequest::Move:
                {
                     framesTable->moveLayer(layerIndex, response->arg().toInt());
                }
                break;
                case TupProjectRequest::Lock:
                {
                     // SQA: Pending for implementation
                }
                break;
                case TupProjectRequest::Rename:
                {
                     framesTable->setLayerName(layerIndex, response->arg().toString());
                }
                break;
                case TupProjectRequest::View:
                {
                     framesTable->setLayerVisibility(layerIndex, response->arg().toBool());
                }
                break;
        }
    }
}

void TupTimeLine::frameResponse(TupFrameResponse *response)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::frameResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    TupTimeLineTable *framesTable = this->framesTable(response->sceneIndex());
    if (framesTable) {
        switch (response->action()) {
                case TupProjectRequest::Add:
                {
                     framesTable->insertFrame(response->layerIndex());
                }
                break;
                case TupProjectRequest::Remove:
                {
                     framesTable->removeFrame(response->layerIndex(), response->frameIndex());
                }
                break;
                case TupProjectRequest::Move:
                {
                     // No action taken
                }
                break;
                case TupProjectRequest::Exchange:
                {
                     framesTable->exchangeFrame(response->frameIndex(), response->layerIndex(), response->arg().toInt(), response->layerIndex());
                }
                break;
                case TupProjectRequest::Lock:
                {
                     framesTable->lockFrame(response->layerIndex(), response->frameIndex(), response->arg().toBool());
                }
                break;
                case TupProjectRequest::Rename:
                {
                }
                break;
                case TupProjectRequest::Select:
                {
                     int layerIndex = response->layerIndex();
                     k->selectedLayer = layerIndex;

                     framesTable->selectFrame(layerIndex, response->frameIndex());
                }
                break;
        }
    }
}

void TupTimeLine::libraryResponse(TupLibraryResponse *response)
{
    // Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::libraryResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (response->action() == TupProjectRequest::InsertSymbolIntoFrame) {
        switch (response->symbolType()) {
                case TupLibraryObject::Sound:
                {
                     TupTimeLineTable *framesTable = this->framesTable(response->sceneIndex());
                     if (framesTable) {
                         framesTable->insertSoundLayer(response->layerIndex() + 1, response->arg().toString());
                         framesTable->insertFrame(response->layerIndex() + 1);
                         // framesTable->insertFrame(response->layerIndex() + 1, "");
                     }
                }
                break;
                default:
                    // Do nothing
                break;
        };
    }
}

void TupTimeLine::requestCommand(int action)
{
    int sceneIndex = k->scenesContainer->currentIndex();
    if (sceneIndex < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene index is invalid -> " + QString::number(sceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int layerIndex = framesTable(sceneIndex)->currentLayer();
    if (layerIndex < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer index is invalid -> " + QString::number(layerIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int frameIndex = framesTable(sceneIndex)->lastFrameByLayer(layerIndex);
    if (frameIndex < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame index is invalid -> " + QString::number(frameIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    if (TupProjectActionBar::FrameActions & action) {
        if (!requestFrameAction(action, frameIndex, layerIndex, sceneIndex)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame action has failed!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::LayerActions & action) {
        if (!requestLayerAction(action, layerIndex, sceneIndex)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer action has failed!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::SceneActions & action) {
        if (!requestSceneAction(action, sceneIndex)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene action has failed!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }
}

bool TupTimeLine::requestFrameAction(int action, int frameIndex, int layerIndex, int sceneIndex, const QVariant &arg)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::requestFrameAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    Q_UNUSED(frameIndex);

    TupProjectRequest request;
    int currentFrame = framesTable(sceneIndex)->currentColumn();

    switch (action) {
            case TupProjectActionBar::InsertFrame:
            {
                 int lastFrame = framesTable(sceneIndex)->lastFrameByLayer(layerIndex);
                 if (currentFrame == lastFrame) {
                     request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, lastFrame + 1,
                                                  TupProjectRequest::Add, tr("Frame"));
                     emit requestTriggered(&request);
                 } else {
                     request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame + 1,
                                                  TupProjectRequest::Add, tr("Frame"));
                     emit requestTriggered(&request);
                     int target = currentFrame + 2;
                     for (int index=target; index <= lastFrame+1; index++) {
                          target++;
                          // request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, index, TupProjectRequest::Rename, tr("Frame %1").arg(target));
                          request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, index, TupProjectRequest::Rename, tr("Frame"));
                          emit requestTriggered(&request);
                     }
                 }

                 selectFrame(layerIndex, lastFrame + 1);
                 return true;
            }
            break;
            case TupProjectActionBar::ExtendFrame:
            {
                 copyFrameForward(layerIndex, currentFrame);
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveFrame:
            {
                 int lastFrame = framesTable(sceneIndex)->lastFrameByLayer(layerIndex);
                 if (currentFrame > lastFrame)
                     return false;

                 if (lastFrame == 0) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Reset);
                     emit requestTriggered(&request);
                     return true;
                 }

                 if (currentFrame == lastFrame) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame, TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     if (currentFrame > 0)
                         selectFrame(layerIndex, currentFrame-1);
                     else
                         framesTable(sceneIndex)->clearSelection();
                 } else {
                     request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame, TupProjectRequest::Remove, arg);
                     emit requestTriggered(&request);

                     selectFrame(layerIndex, currentFrame);
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameBackward:
            {
                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame, TupProjectRequest::Exchange, currentFrame - 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameForward:
            {
                 int lastFrame = framesTable(sceneIndex)->lastFrameByLayer(layerIndex);

                 if (currentFrame == lastFrame) {
                     // TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, lastFrame + 1, TupProjectRequest::Add, tr("Frame %1").arg(lastFrame + 2));
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, lastFrame + 1, TupProjectRequest::Add, tr("Frame"));
                     emit requestTriggered(&request);
                 }

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame, TupProjectRequest::Exchange, currentFrame + 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::LockFrame:
            {
                 bool locked = framesTable(sceneIndex)->frameIsLocked(layerIndex, currentFrame);

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentFrame, TupProjectRequest::Lock, !locked);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            default:
                 // Do nothing
            break;
    }
    
    return false;
}

bool TupTimeLine::requestLayerAction(int action, int layerIndex, int sceneIndex, const QVariant &arg)
{
    TupProjectRequest request;

    switch (action) {
            case TupProjectActionBar::InsertLayer:
            {
                 int layerIndex = framesTable(sceneIndex)->layersCount();
                 request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::Add, tr("Layer %1").arg(layerIndex + 1));
                 emit requestTriggered(&request);

                 if (layerIndex == 0) {
                     // request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
                     request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Add, tr("Frame"));
                     emit requestTriggered(&request);
                 } else {
                     int total = framesTable(sceneIndex)->lastFrameByLayer(layerIndex - 1);
                     for (int j=0; j <= total; j++) {
                          // request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, j, TupProjectRequest::Add, tr("Frame %1").arg(j + 1));
                          request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, j, TupProjectRequest::Add, tr("Frame"));
                          emit requestTriggered(&request);
                     }
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::RemoveLayer:
            {
                 request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
    }
    
    return false;
}

bool TupTimeLine::requestSceneAction(int action, int sceneIndex, const QVariant &arg)
{
    TupProjectRequest request;

    switch (action) {
            case TupProjectActionBar::InsertScene:
            {
                 int sceneTarget = k->scenesContainer->count();
                 request = TupRequestBuilder::createSceneRequest(sceneTarget, TupProjectRequest::Add, tr("Scene %1").arg(sceneTarget + 1));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createLayerRequest(sceneTarget, 0, TupProjectRequest::Add, tr("Layer 1"));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createFrameRequest(sceneTarget, 0, 0, TupProjectRequest::Add, tr("Frame"));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createSceneRequest(sceneTarget, TupProjectRequest::Select);
                 emit requestTriggered(&request);
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveScene:
            {
                 request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneUp:
            {
                 request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Move, sceneIndex + 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneDown:
            {
                 request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Move, sceneIndex - 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            default:
                 // Do nothing
            break;
    }
    
    return false;
}

void TupTimeLine::requestLayerVisibilityAction(int layerIndex, bool isVisible)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::requestLayerVisibilityAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    int sceneIndex = k->scenesContainer->currentIndex();

    TupProjectRequest request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::View, isVisible);
    emit requestTriggered(&request);
}

void TupTimeLine::requestLayerRenameAction(int layerIndex, const QString &name)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLine::requestLayerRenameAction()]";
            qDebug() << "name: " << name;
        #else
            T_FUNCINFO << name;
        #endif
    #endif
    */

    int sceneIndex = k->scenesContainer->currentIndex();
    
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::Rename, name);
    emit requestTriggered(&request);
}

void TupTimeLine::selectFrame(int layerIndex, int frameIndex)
{
    int sceneIndex = k->scenesContainer->currentIndex();
    TupScene *scene = k->project->sceneAt(sceneIndex);
    if (scene) {
        int lastFrame = framesTable(sceneIndex)->lastFrameByLayer(layerIndex);

        if (frameIndex > lastFrame) {
            int totalFrames = scene->framesCount();

            if (frameIndex > (totalFrames-1)) {
                int layersCount = scene->layersCount();
                for (int layer=0; layer < layersCount; layer++) {
                     int currentLimit = framesTable(sceneIndex)->lastFrameByLayer(layer); 
                     for (int frame = currentLimit + 1; frame <= frameIndex; frame++) {
                          TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layer, frame,
                                                      TupProjectRequest::Add, tr("Frame"));
                                                      // TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                          emit requestTriggered(&request);
                     }
                }
            } else {
                for (int frame = lastFrame + 1; frame <= frameIndex; frame++) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frame,
                                                 TupProjectRequest::Add, tr("Frame"));
                                                 // TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                     emit requestTriggered(&request);
                }
            }

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex,
                                                           frameIndex, TupProjectRequest::Select, "1");
            emit requestTriggered(&request);
        }

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex,
                                                       frameIndex, TupProjectRequest::Select, "1");
        emit requestTriggered(&request);
    }
}

// void TupTimeLine::removeFrameCopy(int layerIndex, int frameIndex)
void TupTimeLine::removeFrameCopy()
{
    k->actionBar->emitActionSelected(TupProjectActionBar::RemoveFrame);

    /*
    int sceneIndex = k->scenesContainer->currentIndex();
    // TupTimeLineTable *framesTable = k->scenesContainer->currentScene();

    int framesCount;
    TupScene *scene = k->project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer)
            framesCount = layer->framesCount();
        else
            return;
    } else {
        return;
    }

    TupProjectRequest request;
    if (framesCount == 1) {
        request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Reset);
        emit requestTriggered(&request);

        request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Rename, tr("Frame"));
        emit requestTriggered(&request);
    } else {
        request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex, TupProjectRequest::Remove);
        emit requestTriggered(&request);

        if (frameIndex > 0) {
            request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex - 1, TupProjectRequest::Select);
            emit localRequestTriggered(&request);
        }
    }
    */
}

void TupTimeLine::copyFrameForward(int layerIndex, int frameIndex)
{
    int sceneIndex = k->scenesContainer->currentIndex();

    QString frameName = tr("Frame");
    TupScene *scene = k->project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(frameIndex);
            if (frame)
                frameName = frame->frameName();
        }
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex, TupProjectRequest::Copy);
    emit localRequestTriggered(&request);

    int target = frameIndex + 1;
    request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, target, TupProjectRequest::Add, "");
    emit requestTriggered(&request);

    request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, target, TupProjectRequest::Paste);
    emit localRequestTriggered(&request);

    request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, target, TupProjectRequest::Rename, frameName);
    emit requestTriggered(&request);

    request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, target, TupProjectRequest::Select);
    emit localRequestTriggered(&request);
}

void TupTimeLine::requestSceneSelection(int sceneIndex)
{
    if (k->scenesContainer->count() > 1) {
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

/*
void TupTimeLine::emitChangeFrameRequest(int sceneIndex, int layerIndex, int frameIndex)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                                TupProjectRequest::Select, "1");
    emit requestTriggered(&request);
}
*/

void TupTimeLine::requestLayerMove(int oldLayerIndex, int newLayerIndex)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), oldLayerIndex,
                                                   TupProjectRequest::Move, newLayerIndex);
    emit requestTriggered(&request);
}

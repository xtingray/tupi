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
    Private() : container(0), actionBar(0), selectedLayer(-1), library(0) {}
    
    TTabWidget *container;
    TupProjectActionBar *actionBar;
    int selectedLayer; 
    TupProject *project;
    TupLibrary *library;
};

TupTimeLine::TupTimeLine(TupProject *project, QWidget *parent) : TupModuleWidgetBase(parent, "TupTimeLine"), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine()]";
        #else
            TINIT;
        #endif
    #endif
    
    setWindowTitle(tr("Time Line"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/time_line.png"));

    k->project = project;
    k->library = k->project->library();
    
    k->actionBar = new TupProjectActionBar(QString("TimeLine"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameBackward |
                        TupProjectActionBar::MoveFrameForward |
                        TupProjectActionBar::LockFrame);
   
    addChild(k->actionBar, Qt::AlignCenter);
    
    k->container = new TTabWidget(this);
    addChild(k->container);
    
    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(requestCommand(int)));
    connect(k->container, SIGNAL(currentChanged(int)), this, SLOT(requestSceneSelection(int)));
}

TupTimeLine::~TupTimeLine()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupTimeLine()]";
        #else
            TEND;
        #endif
    #endif

    delete k;
}

TupTimeLineTable *TupTimeLine::framesTable(int sceneIndex)
{
    TupTimeLineTable *framesTable = qobject_cast<TupTimeLineTable *>(k->container->widget(sceneIndex));
    if (framesTable)
        return framesTable;
 
    return 0;
}

void TupTimeLine::insertScene(int sceneIndex, const QString &name)
{
    if (sceneIndex < 0 || sceneIndex > k->container->count())
        return;

    TupTimeLineTable *framesTable = new TupTimeLineTable(sceneIndex, k->container);
    framesTable->setItemSize(10, 20);

    connect(framesTable, SIGNAL(frameSelected(int, int)), this, SLOT(selectFrame(int, int)));
    connect(framesTable, SIGNAL(visibilityChanged(int, bool)), this, SLOT(requestLayerVisibilityAction(int, bool)));
    connect(framesTable, SIGNAL(layerNameChanged(int, const QString &)), this, SLOT(requestLayerRenameAction(int, const QString &))); 
    connect(framesTable, SIGNAL(layerMoved(int, int)), this, SLOT(requestLayerMove(int, int)));

    k->container->insertTab(sceneIndex, framesTable, name);
}

void TupTimeLine::removeScene(int sceneIndex)
{
    if (sceneIndex >= 0 && sceneIndex < k->container->count()) {
        QWidget *w = k->container->widget(sceneIndex);
        k->container->removeTab(sceneIndex);
        delete w;
    }
}

void TupTimeLine::closeAllScenes()
{
    k->container->blockSignals(true);
    while (k->container->currentWidget())
           delete k->container->currentWidget();
    k->container->blockSignals(false);
}

void TupTimeLine::sceneResponse(TupSceneResponse *response)
{
    // Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::sceneResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 insertScene(response->sceneIndex(), response->arg().toString());
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
                 k->container->setCurrentIndex(response->sceneIndex());
            }
            break;
            default:
                 #ifdef K_DEBUG
                     QString msg = "TupTimeLine::sceneResponse : Unknown action :/";
                     #ifdef Q_OS_WIN32
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
    // Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::layerResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupTimeLineTable *framesTable = this->framesTable(response->sceneIndex());
    if (framesTable) {
        switch (response->action()) {
                case TupProjectRequest::Add:
                {
                     framesTable->insertLayer(response->layerIndex(), response->arg().toString());
                }
                break;
                case TupProjectRequest::Remove:
                {
                     framesTable->removeLayer(response->layerIndex());
                     if (framesTable->layersCount() == 0) {
                         TupProjectRequest request = TupRequestBuilder::createLayerRequest(0, 0, TupProjectRequest::Add, tr("Layer %1").arg(1));
                         emit requestTriggered(&request);

                         request = TupRequestBuilder::createFrameRequest(0, 0, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
                         emit requestTriggered(&request);
                     }
                }
                break;
                case TupProjectRequest::Move:
                {
                     framesTable->moveLayer(response->layerIndex(), response->arg().toInt());
                }
                break;
                case TupProjectRequest::Lock:
                {
                     // SQA: Pending for implementation
                }
                break;
                case TupProjectRequest::Rename:
                {
                     framesTable->setLayerName(response->layerIndex(), response->arg().toString());
                }
                break;
                case TupProjectRequest::View:
                {
                     framesTable->setLayerVisibility(response->layerIndex(), response->arg().toBool());
                }
                break;
        }
    }
}

void TupTimeLine::frameResponse(TupFrameResponse *response)
{
    // Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::frameResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupTimeLineTable *framesTable = this->framesTable(response->sceneIndex());
    if (framesTable) {
        switch (response->action()) {
                case TupProjectRequest::Add:
                {
                     framesTable->insertFrame(response->layerIndex(), response->arg().toString());
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
        #ifdef Q_OS_WIN32
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
                         framesTable->insertFrame(response->layerIndex() + 1, "");
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
    int scenePos = k->container->currentIndex();

    if (scenePos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene index is invalid -> " + QString::number(scenePos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int layerPos = framesTable(scenePos)->currentLayer();

    if (layerPos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer index is invalid -> " + QString::number(layerPos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int framePos = framesTable(scenePos)->lastFrameByLayer(layerPos);

    if (framePos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame index is invalid -> " + QString::number(framePos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    if (TupProjectActionBar::FrameActions & action) {
        if (!requestFrameAction(action, framePos, layerPos, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::LayerActions & action) {
        if (!requestLayerAction(action, layerPos, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::SceneActions & action) {
        if (!requestSceneAction(action, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }
}

bool TupTimeLine::requestFrameAction(int action, int framePos, int layerPos, int scenePos, const QVariant &arg)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestFrameAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    Q_UNUSED(framePos);

    TupProjectRequest request;

    switch (action) {
            case TupProjectActionBar::InsertFrame:
            {
                 int lastFrame = framesTable(scenePos)->lastFrameByLayer(layerPos);
                 int currentFrame = framesTable(scenePos)->currentColumn();

                 if (currentFrame == lastFrame) {
                     request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, lastFrame + 1,
                                                TupProjectRequest::Add, tr("Frame %1").arg(lastFrame + 2));
                     emit requestTriggered(&request);
                 } else {
                     request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame + 1,
                                                TupProjectRequest::Add, tr("Frame %1").arg(currentFrame + 2));
                     emit requestTriggered(&request);
                     int target = currentFrame + 2;
                     for (int index=target; index <= lastFrame+1; index++) {
                          target++;
                          request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, index, TupProjectRequest::Rename, tr("Frame %1").arg(target));
                          emit requestTriggered(&request);
                     }
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::RemoveFrame:
            {
                 int lastFrame = framesTable(scenePos)->lastFrameByLayer(layerPos);
                 int currentFrame = framesTable(scenePos)->currentColumn(); 

                 if (currentFrame > lastFrame)
                     return false;

                 if (lastFrame == 0) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, 0, TupProjectRequest::Reset);
                     emit requestTriggered(&request);
                     return true;
                 }

                 if (currentFrame == lastFrame) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame, TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     if (currentFrame > 0)
                         selectFrame(layerPos, currentFrame-1);
                     else
                         framesTable(scenePos)->clearSelection();
                 } else {
                     request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame, TupProjectRequest::Remove, arg);
                     emit requestTriggered(&request);

                     selectFrame(layerPos, currentFrame);
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameBackward:
            {
                 int currentFrame = framesTable(scenePos)->currentColumn();

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame, TupProjectRequest::Exchange, currentFrame - 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameForward:
            {
                 int currentFrame = framesTable(scenePos)->currentColumn();
                 int lastFrame = framesTable(scenePos)->lastFrameByLayer(layerPos);

                 if (currentFrame == lastFrame) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, lastFrame + 1, TupProjectRequest::Add, tr("Frame %1").arg(lastFrame + 2));
                     emit requestTriggered(&request);
                 }

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame, TupProjectRequest::Exchange, currentFrame + 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::LockFrame:
            {
                 int currentFrame = framesTable(scenePos)->currentColumn();
                 bool locked = framesTable(scenePos)->frameIsLocked(layerPos, currentFrame);

                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layerPos, currentFrame, TupProjectRequest::Lock, !locked);
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

bool TupTimeLine::requestLayerAction(int action, int layerPos, int scenePos, const QVariant &arg)
{
    TupProjectRequest request;

    switch (action) {
            case TupProjectActionBar::InsertLayer:
            {
                 int layerIndex = framesTable(scenePos)->layersCount();
                 request = TupRequestBuilder::createLayerRequest(scenePos, layerIndex, TupProjectRequest::Add, tr("Layer %1").arg(layerIndex + 1));
                 emit requestTriggered(&request);

                 if (layerIndex == 0) {
                     request = TupRequestBuilder::createFrameRequest(scenePos, layerIndex, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
                     emit requestTriggered(&request);
                 } else {
                     int total = framesTable(scenePos)->lastFrameByLayer(layerIndex - 1);
                     for (int j=0; j <= total; j++) {
                          request = TupRequestBuilder::createFrameRequest(scenePos, layerIndex, j, TupProjectRequest::Add, tr("Frame %1").arg(j + 1));
                          emit requestTriggered(&request);
                     }
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::RemoveLayer:
            {
                 request = TupRequestBuilder::createLayerRequest(scenePos, layerPos, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
    }
    
    return false;
}

bool TupTimeLine::requestSceneAction(int action, int scenePos, const QVariant &arg)
{
    TupProjectRequest request;

    switch (action) {
            case TupProjectActionBar::InsertScene:
            {
                 request = TupRequestBuilder::createSceneRequest(scenePos + 1, TupProjectRequest::Add, tr("Scene %1").arg(scenePos + 2));
                 emit requestTriggered(&request);
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveScene:
            {
                 request = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneUp:
            {
                 request = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move, scenePos + 1);
                 emit requestTriggered(&request);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneDown:
            {
                 request = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move, scenePos - 1);
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

void TupTimeLine::requestLayerVisibilityAction(int layer, bool isVisible)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestLayerVisibilityAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    int scenePos = k->container->currentIndex();

    TupProjectRequest request = TupRequestBuilder::createLayerRequest(scenePos, layer, TupProjectRequest::View, isVisible);
    emit requestTriggered(&request);
}

void TupTimeLine::requestLayerRenameAction(int layer, const QString &name)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestLayerRenameAction()]";
            qDebug() << "name: " << name;
        #else
            T_FUNCINFO << name;
        #endif
    #endif
    */

    int scenePos = k->container->currentIndex();
    
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(scenePos, layer, TupProjectRequest::Rename, name);
    emit requestTriggered(&request);
}

void TupTimeLine::selectFrame(int indexLayer, int indexFrame)
{
    int scenePos = k->container->currentIndex();
    TupScene *scene = k->project->scene(scenePos);
    if (scene) {
        int lastFrame = framesTable(scenePos)->lastFrameByLayer(indexLayer);

        if (indexFrame > lastFrame) {
            int totalFrames = scene->framesCount();

            if (indexFrame > (totalFrames-1)) {
                int layersCount = scene->layersCount();
                for (int layer=0; layer < layersCount; layer++) {
                     int currentLimit = framesTable(scenePos)->lastFrameByLayer(layer); 
                     for (int frame = currentLimit + 1; frame <= indexFrame; frame++) {
                          TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, layer, frame,
                                                      TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                          emit requestTriggered(&request);
                     }
                }
            } else {
                for (int frame = lastFrame + 1; frame <= indexFrame; frame++) {
                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer, frame,
                                                 TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                     emit requestTriggered(&request);
                }
            }

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                           indexFrame, TupProjectRequest::Select, "1");
            emit requestTriggered(&request);
        }

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                       indexFrame, TupProjectRequest::Select, "1");
        emit requestTriggered(&request);
    }
}

void TupTimeLine::requestSceneSelection(int sceneIndex)
{
    if (k->container->count() > 1) {
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupTimeLine::emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                                TupProjectRequest::Select, "1");
    emit requestTriggered(&request);
}

void TupTimeLine::requestLayerMove(int oldIndex, int newIndex)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->container->currentIndex(), oldIndex,
                                                   TupProjectRequest::Move, newIndex);
    emit requestTriggered(&request);
}

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

#include <QList>
#include <QHeaderView>

#include "tuptimeline.h"

// Tupi Framework 
#include "tapplication.h"
#include "tdebug.h"

#include "tupprojectrequest.h"
#include "tuplibraryobject.h"

#include "tupframestable.h"
#include "tuplayermanager.h"

#include "tupprojectactionbar.h"

#include "tuprequestbuilder.h"

#include "tuplibrary.h"

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

struct TupTimeLine::Private
{
    Private() : container(0), actionBar(0), selectedLayer(-1), library(0) {}
    
    TTabWidget *container;
    TupProjectActionBar *actionBar;
    int selectedLayer; 
    const TupLibrary *library;
};

TupTimeLine::TupTimeLine(QWidget *parent) : TupModuleWidgetBase(parent, "TupTimeLine"), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif
    
    setWindowTitle(tr("Time Line"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/time_line.png"));
    
    k->actionBar = new TupProjectActionBar(QString("TimeLine"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        // TupProjectActionBar::MoveLayerUp |
                        // TupProjectActionBar::MoveLayerDown |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameUp |
                        TupProjectActionBar::MoveFrameDown |
                        TupProjectActionBar::LockFrame);
   
    addChild(k->actionBar, Qt::AlignCenter);
    
    k->container = new TTabWidget(this);
    addChild(k->container);
    
    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(requestCommand(int)));
    connect(k->container, SIGNAL(currentChanged(int)), this, SLOT(emitRequestChangeScene(int)));

}

TupTimeLine::~TupTimeLine()
{
    #ifdef K_DEBUG
        TEND;
    #endif
    delete k;
}

TupLayerManager *TupTimeLine::layerManager(int sceneIndex)
{
    QSplitter *splitter = qobject_cast<QSplitter *>(k->container->widget(sceneIndex));
    splitter->setMinimumHeight(146);

    if (splitter)
        return qobject_cast<TupLayerManager *>(splitter->widget(0));
    
    return 0;
}

TupFramesTable *TupTimeLine::framesTable(int sceneIndex)
{
    QSplitter *splitter = qobject_cast<QSplitter *>(k->container->widget(sceneIndex));
    
    if (splitter)
        return qobject_cast<TupFramesTable *>(splitter->widget(1));
    
    return 0;
}

void TupTimeLine::insertScene(int position, const QString &name)
{
    if (position < 0 || position > k->container->count())
        return;
    
    QSplitter *splitter = new QSplitter(k->container);
    splitter->setContentsMargins(1, 1, 1, 1);
    
    TupLayerManager *layerManager = new TupLayerManager(position, splitter);
    layerManager->setFixedWidth(244);

    splitter->addWidget(layerManager);

    connect(layerManager->getLayerIndex(), SIGNAL(localRequest()), this, SLOT(emitSelectionSignal()));

    // connect(layerManager, SIGNAL(requestRenameEvent(int, const QString&)), this,
    //        SLOT(emitRequestRenameLayer(int, const QString &))); // FIXME

    connect(layerManager->getLayerControls(), SIGNAL(layerVisibility(int, int, bool)), this, 
            SLOT(emitLayerVisibility(int, int, bool)));

    TupFramesTable *framesTable = new TupFramesTable(position, splitter);
    splitter->addWidget(framesTable);
    
    framesTable->setItemSize(10, 20);

    connect(layerManager->getLayerIndex()->verticalScrollBar(), SIGNAL(valueChanged(int)), framesTable->verticalScrollBar(),
            SLOT(setValue(int)));

    connect(layerManager->getLayerControls()->verticalScrollBar(), SIGNAL(valueChanged(int)), framesTable->verticalScrollBar(),
            SLOT(setValue(int)));

    connect(framesTable, SIGNAL(frameRequest(int, int, int, int, const QVariant&)), this, 
            SLOT(requestFrameAction(int, int, int, int, const QVariant&)));

    connect(framesTable->verticalScrollBar(), SIGNAL(valueChanged(int)), layerManager->getLayerIndex()->verticalScrollBar(),
            SLOT(setValue(int)));

    connect(framesTable->verticalScrollBar(), SIGNAL(valueChanged(int)), layerManager->getLayerControls()->verticalScrollBar(),
            SLOT(setValue(int)));

    //connect(framesTable, SIGNAL(emitSelection(int, int)), this, SLOT(selectFrame(int, int)));

    k->container->insertTab(position, splitter, name);
}

void TupTimeLine::removeScene(int position)
{
    if (position >= 0 && position < k->container->count()) {
        QWidget *w = k->container->widget(position);
        // k->container->removeWidget(w);
        k->container->removeTab(position);
        
        delete w;
    }
}

void TupTimeLine::closeAllScenes()
{
    while (k->container->currentWidget())
           delete k->container->currentWidget();
}

void TupTimeLine::setLibrary(const TupLibrary *library)
{
    k->library = library;
}

void TupTimeLine::sceneResponse(TupSceneResponse *response)
{
    Q_UNUSED(response);

    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
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
                 tFatal() << "TupTimeLine::sceneResponse : Unknown action :/";
            break;
    }
    */
}

void TupTimeLine::layerResponse(TupLayerResponse *response)
{
    Q_UNUSED(response);

    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
            TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
            if (layerManager)
                layerManager->insertLayer(response->layerIndex(), response->arg().toString());
            
            TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
            if (framesTable)
                framesTable->insertLayer(response->layerIndex(), response->arg().toString());
            }
            
            break;
            case TupProjectRequest::Remove:
            {
                 tFatal() << "TupTimeLine::layerResponse -> Removing layer!";

                 TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 if (layerManager)
                     layerManager->removeLayer(response->layerIndex());
            
                 TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
                 if (framesTable)
                     framesTable->removeLayer(response->layerIndex());
            }
            break;
            case TupProjectRequest::Move:
            {
                 TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 if (layerManager)
                     layerManager->moveLayer(response->layerIndex(), response->arg().toInt());
            
                 TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
                 if (framesTable)
                     framesTable->moveLayer(response->layerIndex(), response->arg().toInt());
            }
            break;
            case TupProjectRequest::Lock:
            {
                 TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
            
                 if (layerManager)
                     layerManager->lockLayer(response->layerIndex(), response->arg().toBool());
            }
            break;
            case TupProjectRequest::Rename:
            {
                 TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
            
                 if (layerManager)
                     layerManager->renameLayer(response->layerIndex(), response->arg().toString());
            }
            break;
            case TupProjectRequest::View:
            {
                 TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 layerManager->getLayerControls()->setLayerVisibility(response->layerIndex(), response->arg().toString());
            }
            break;
    }

    */
}

void TupTimeLine::frameResponse(TupFrameResponse *response)
{
    Q_UNUSED(response);

    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 TupFramesTable *framesTable = this->framesTable(response->sceneIndex());

                 if (framesTable)
                     framesTable->insertFrame(response->layerIndex(), response->arg().toString());
                 else
                     tFatal() << "TupTimeLine::frameResponse -> NO FRAME TABLE AVAILABLE!";
            }
            break;
            case TupProjectRequest::Remove:
            {
                 TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
            
                 if (framesTable)
                     framesTable->removeFrame(response->layerIndex(), response->frameIndex());
            }
            break;
            case TupProjectRequest::Move:
            {
            }
            break;
            case TupProjectRequest::Lock:
            {
                 TupFramesTable *framesTable = this->framesTable(response->sceneIndex());

                 if (framesTable)
                     framesTable->lockFrame(response->layerIndex(), response->frameIndex(), response->arg().toBool());
            }
            break;
            case TupProjectRequest::Rename:
            {
            }
            break;
            case TupProjectRequest::Select:
            {
                 tFatal() << "TupTimeLine::frameResponse() - Just tracing Selection!";

                 int layerIndex = response->layerIndex();

                 if (k->selectedLayer != layerIndex) {
                     layerManager(response->sceneIndex())->getLayerIndex()->setCurrentCell(layerIndex, 0);
                     k->selectedLayer = layerIndex; 
                 }

                 framesTable(response->sceneIndex())->blockSignals(true);
                 framesTable(response->sceneIndex())->setCurrentCell(layerIndex, response->frameIndex());
                 framesTable(response->sceneIndex())->blockSignals(false);
            }
            break;

    }
    */
}

void TupTimeLine::libraryResponse(TupLibraryResponse *response)
{
    Q_UNUSED(response);

    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (response->action() == TupProjectRequest::InsertSymbolIntoFrame) {
        switch (response->symbolType()) {
                case TupLibraryObject::Sound:
                {
                     TupLayerManager *layerManager = this->layerManager(response->sceneIndex());
                     if (layerManager) {
                         layerManager->insertSoundLayer(response->layerIndex()+1, 
                                                        response->arg().toString());
                    
                         TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
                         if (framesTable) {
                             framesTable->insertSoundLayer(response->layerIndex()+1, 
                                                           response->arg().toString());
                             framesTable->insertFrame(response->layerIndex()+1,"");
                         }
                     }
                }
                break;
        };
    }

    */
}

void TupTimeLine::requestCommand(int action)
{
    int scenePos = k->container->currentIndex();

    if (scenePos < 0)
        return;
    
    int layerPos = layerManager(scenePos)->getLayerIndex()->verticalHeader()->visualIndex(
                   layerManager(scenePos)->getLayerIndex()->currentRow());

    int framePos = framesTable(scenePos)->lastFrameByLayer(layerPos) + 1;

    if (!requestFrameAction(action, framePos, layerPos, scenePos)) {
        tFatal() << "TupTimeLine::requestCommand -> It isn't frame action";
        layerPos = layerManager(scenePos)->getLayerIndex()->rowCount();
        framePos = framesTable(scenePos)->lastFrameByLayer(layerPos);
        if (!requestLayerAction(action, layerPos, scenePos)) {
            tFatal() << "TupTimeLine::requestCommand -> It isn't layer action";
            if (!requestSceneAction(action, scenePos)) {
                #ifdef K_DEBUG
                    tFatal("timeline") << "Can't handle action";
                #endif
            }
        }
    }
}

bool TupTimeLine::requestFrameAction(int action, int framePos, int layerPos, int scenePos, const QVariant &arg)
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif
    */

    if (scenePos < 0)
        scenePos = k->container->currentIndex();

    switch (action) {
            case TupProjectActionBar::InsertFrame:
            {
                 int layersTotal = layerManager(scenePos)->getLayerIndex()->rowCount();
                 int usedFrames = framesTable(scenePos)->lastFrameByLayer(layerPos);

                 if (layersTotal == 1) {
                     for (int frame = usedFrames; frame < framePos; frame++) {
                          TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, frame + 1,
                                                   TupProjectRequest::Add, tr("Frame %1").arg(frame + 2));
                          emit requestTriggered(&event);
                     }

                 } else {
                     usedFrames++;

                     for (int layer=0; layer < layersTotal; layer++) {
                          for (int frame = usedFrames; frame <= framePos; frame++) {
                               TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layer, frame,
                                                        TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                               emit requestTriggered(&event);
                          }
                     }
                 }
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveFrame:
            {
                 TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos - 1,
                                          TupProjectRequest::Remove, arg);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameUp:
            {
                 TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          TupProjectRequest::Move, framePos-1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameDown:
            {
                 TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          TupProjectRequest::Move, framePos+1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            /*
            case TupProjectActionBar::SelectFrame:
            {
                 TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          TupProjectRequest::Select, arg);
                 emit localRequestTriggered(&event);

                 return true;
            }
            break;
            */
    }
    
    return false;
}

bool TupTimeLine::requestLayerAction(int action, int layerPos, int scenePos, const QVariant &arg)
{
    if (scenePos < 0)
        scenePos = k->container->currentIndex();
    
    if (scenePos >= 0) {
        if (layerPos < 0)
            layerPos = layerManager(scenePos)->getLayerIndex()->verticalHeader()->visualIndex(
                                    layerManager(scenePos)->getLayerIndex()->currentRow());
    }

    switch (action) {
            case TupProjectActionBar::InsertLayer:
            {
                 TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layerPos,
                                          TupProjectRequest::Add, tr("Layer %1").arg(layerPos + 1));
                 emit requestTriggered(&event);

                 if (layerPos == 0) {
                     event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, 0,
                                               TupProjectRequest::Add, tr("Frame %1").arg(1));
                     emit requestTriggered(&event);
                 } else {
                     int total = framesTable(scenePos)->lastFrameByLayer(layerPos-1);
                     for (int j=0; j <= total; j++) {
                          event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, j,
                                                                       TupProjectRequest::Add, tr("Frame %1").arg(j + 1));
                          emit requestTriggered(&event);
                     }
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::RemoveLayer:
            {
                 TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layerPos-1, 
                                          TupProjectRequest::Remove, arg);
            
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveLayerUp:
            {
                 TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                          TupProjectRequest::Move, layerPos - 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveLayerDown:
            {
                 TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                          TupProjectRequest::Move, layerPos + 1);
                 emit requestTriggered(&event);
                 return true;
            }
            break;
    }
    
    return false;
}

bool TupTimeLine::requestSceneAction(int action, int scenePos, const QVariant &arg)
{
    if (scenePos < 0)
        scenePos = k->container->currentIndex();
    
    switch (action) {
            case TupProjectActionBar::InsertScene:
            {
                 TupProjectRequest event = TupRequestBuilder::createSceneRequest(scenePos + 1, 
                                           TupProjectRequest::Add, tr("Scene %1").arg(scenePos + 2));
            
                 emit requestTriggered(&event);
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveScene:
            {
                 TupProjectRequest event = TupRequestBuilder::createSceneRequest(scenePos, 
                                          TupProjectRequest::Remove, arg);
            
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneUp:
            {
                 TupProjectRequest event = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move,
                                          scenePos + 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneDown:
            {
                 TupProjectRequest event = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move,
                                          scenePos - 1);
                 emit requestTriggered(&event);
                 return true;
            }
            break;
    }
    
    return false;
}

void TupTimeLine::emitRequestRenameLayer(int layer, const QString &name)
{
    /*
    #ifdef K_DEBUG
        T_FUNCINFO << name;
    #endif
    */

    int scenePos = k->container->currentIndex();
    
    TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layer, TupProjectRequest::Rename, name);
    
    emit requestTriggered(&event);
}

void TupTimeLine::emitSelectionSignal()
{
    // tFatal() << "TupTimeLine::emitSelectionSignal() - Just tracing!";

    int scenePos = k->container->currentIndex();
    int layerPos = layerManager(scenePos)->getLayerIndex()->currentRow();
    k->selectedLayer = layerPos;
    int frame = framesTable(scenePos)->currentColumn();

    requestFrameAction(TupProjectRequest::Select, frame, layerPos, scenePos);
}

void TupTimeLine::emitLayerVisibility(int sceneIndex, int layerIndex, bool checked) 
{
    TupProjectRequest event = TupRequestBuilder::createLayerRequest(sceneIndex,
                             layerIndex, TupProjectRequest::View, checked);
    emit requestTriggered(&event);
}

void TupTimeLine::selectFrame(int indexLayer, int indexFrame)
{
    int scenePos = k->container->currentIndex();

    // tFatal() << "TupTimeLine::selectFrame() - Just tracing!";
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                 indexFrame, TupProjectRequest::Select, "1");
    emit requestTriggered(&request);

}

void TupTimeLine::emitRequestChangeScene(int sceneIndex)
{
    if (k->container->count() > 1) {
        // tFatal() << "TupTimeLine::emitRequestChangeScene - Just tracing!";
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupTimeLine::emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex)
{
    // tFatal() << "TupTimeLine::emitRequestChangeFrame - Just tracing!";
    TupProjectRequest event = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                             TupProjectRequest::Select, "1");
    emit requestTriggered(&event);
}


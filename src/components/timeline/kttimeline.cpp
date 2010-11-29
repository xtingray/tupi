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

#include <QList>
#include <QHeaderView>

#include "kttimeline.h"

// Tupi Framework 
#include "kapplication.h"
#include "kdebug.h"

#include "ktprojectrequest.h"
#include "ktlibraryobject.h"

#include "ktframestable.h"
#include "ktlayermanager.h"

#include "ktprojectactionbar.h"

#include "ktrequestbuilder.h"

#include "ktlibrary.h"

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

struct KTTimeLine::Private
{
    Private() : container(0), actionBar(0), selectedLayer(-1), library(0) {}
    
    KTabWidget *container;
    KTProjectActionBar *actionBar;
    int selectedLayer; 
    const KTLibrary *library;
};

KTTimeLine::KTTimeLine(QWidget *parent) : KTModuleWidgetBase(parent, "KTTimeLine"), k(new Private)
{
    #ifdef K_DEBUG
        KINIT;
    #endif
    
    setWindowTitle(tr("Time Line"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/time_line.png"));
    
    k->actionBar = new KTProjectActionBar(QString("TimeLine"), KTProjectActionBar::InsertLayer |
                        KTProjectActionBar::RemoveLayer |
                        // KTProjectActionBar::MoveLayerUp |
                        // KTProjectActionBar::MoveLayerDown |
                        KTProjectActionBar::Separator |
                        KTProjectActionBar::InsertFrame |
                        KTProjectActionBar::RemoveFrame |
                        KTProjectActionBar::MoveFrameUp |
                        KTProjectActionBar::MoveFrameDown |
                        KTProjectActionBar::LockFrame);
   
    addChild(k->actionBar, Qt::AlignCenter);
    
    k->container = new KTabWidget(this);
    addChild(k->container);
    
    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(requestCommand(int)));
    connect(k->container, SIGNAL(currentChanged(int)), this, SLOT(emitRequestChangeScene(int)));

}

KTTimeLine::~KTTimeLine()
{
    #ifdef K_DEBUG
        KEND;
    #endif
    delete k;
}

KTLayerManager *KTTimeLine::layerManager(int sceneIndex)
{
    QSplitter *splitter = qobject_cast<QSplitter *>(k->container->widget(sceneIndex));
    splitter->setMinimumHeight(146);

    if (splitter)
        return qobject_cast<KTLayerManager *>(splitter->widget(0));
    
    return 0;
}

KTFramesTable *KTTimeLine::framesTable(int sceneIndex)
{
    QSplitter *splitter = qobject_cast<QSplitter *>(k->container->widget(sceneIndex));
    
    if (splitter)
        return qobject_cast<KTFramesTable *>(splitter->widget(1));
    
    return 0;
}

void KTTimeLine::insertScene(int position, const QString &name)
{
    if (position < 0 || position > k->container->count())
        return;
    
    QSplitter *splitter = new QSplitter(k->container);
    splitter->setContentsMargins(1, 1, 1, 1);
    
    KTLayerManager *layerManager = new KTLayerManager(position, splitter);
    layerManager->setFixedWidth(244);

    splitter->addWidget(layerManager);

    connect(layerManager->getLayerIndex(), SIGNAL(localRequest()), this, SLOT(emitSelectionSignal()));

    // connect(layerManager, SIGNAL(requestRenameEvent(int, const QString&)), this,
    //        SLOT(emitRequestRenameLayer(int, const QString &))); // FIXME

    connect(layerManager->getLayerControls(), SIGNAL(layerVisibility(int, int, bool)), this, 
            SLOT(emitLayerVisibility(int, int, bool)));

    KTFramesTable *framesTable = new KTFramesTable(position, splitter);
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

void KTTimeLine::removeScene(int position)
{
    if (position >= 0 && position < k->container->count()) {
        QWidget *w = k->container->widget(position);
        // k->container->removeWidget(w);
        k->container->removeTab(position);
        
        delete w;
    }
}

void KTTimeLine::closeAllScenes()
{
    while (k->container->currentWidget())
           delete k->container->currentWidget();
}

void KTTimeLine::setLibrary(const KTLibrary *library)
{
    k->library = library;
}

void KTTimeLine::sceneResponse(KTSceneResponse *response)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    /*

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 insertScene(response->sceneIndex(), response->arg().toString());
            }
            break;
            case KTProjectRequest::Remove:
            {
                 removeScene(response->sceneIndex());
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
            
            }
            break;
            case KTProjectRequest::Select:
            {
                 kFatal() << "KTTimeLine::sceneResponse <- Doing Scene selection!";
                 k->container->setCurrentIndex(response->sceneIndex());
            }
            break;
            default:
                 kFatal() << "KTTimeLine::sceneResponse : Unknown action :/";
            break;
    }

    */
}

void KTTimeLine::layerResponse(KTLayerResponse *response)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    /*

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
            KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
            if (layerManager)
                layerManager->insertLayer(response->layerIndex(), response->arg().toString());
            
            KTFramesTable *framesTable = this->framesTable(response->sceneIndex());
            if (framesTable)
                framesTable->insertLayer(response->layerIndex(), response->arg().toString());
            }
            
            break;
            case KTProjectRequest::Remove:
            {
                 kFatal() << "KTTimeLine::layerResponse -> Removing layer!";

                 KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 if (layerManager)
                     layerManager->removeLayer(response->layerIndex());
            
                 KTFramesTable *framesTable = this->framesTable(response->sceneIndex());
                 if (framesTable)
                     framesTable->removeLayer(response->layerIndex());
            }
            break;
            case KTProjectRequest::Move:
            {
                 KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 if (layerManager)
                     layerManager->moveLayer(response->layerIndex(), response->arg().toInt());
            
                 KTFramesTable *framesTable = this->framesTable(response->sceneIndex());
                 if (framesTable)
                     framesTable->moveLayer(response->layerIndex(), response->arg().toInt());
            }
            break;
            case KTProjectRequest::Lock:
            {
                 KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
            
                 if (layerManager)
                     layerManager->lockLayer(response->layerIndex(), response->arg().toBool());
            }
            break;
            case KTProjectRequest::Rename:
            {
                 KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
            
                 if (layerManager)
                     layerManager->renameLayer(response->layerIndex(), response->arg().toString());
            }
            break;
            case KTProjectRequest::View:
            {
                 KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
                 layerManager->getLayerControls()->setLayerVisibility(response->layerIndex(), response->arg().toString());
            }
            break;
    }

    */
}

void KTTimeLine::frameResponse(KTFrameResponse *response)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    /*

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 KTFramesTable *framesTable = this->framesTable(response->sceneIndex());

                 if (framesTable)
                     framesTable->insertFrame(response->layerIndex(), response->arg().toString());
                 else
                     kFatal() << "KTTimeLine::frameResponse -> NO FRAME TABLE AVAILABLE!";
            }
            break;
            case KTProjectRequest::Remove:
            {
                 KTFramesTable *framesTable = this->framesTable(response->sceneIndex());
            
                 if (framesTable)
                     framesTable->removeFrame(response->layerIndex(), response->frameIndex());
            }
            break;
            case KTProjectRequest::Move:
            {
            }
            break;
            case KTProjectRequest::Lock:
            {
                 KTFramesTable *framesTable = this->framesTable(response->sceneIndex());

                 if (framesTable)
                     framesTable->lockFrame(response->layerIndex(), response->frameIndex(), response->arg().toBool());
            }
            break;
            case KTProjectRequest::Rename:
            {
            }
            break;
            case KTProjectRequest::Select:
            {
                 kFatal() << "KTTimeLine::frameResponse() - Just tracing Selection!";

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

void KTTimeLine::libraryResponse(KTLibraryResponse *response)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    /*

    if (response->action() == KTProjectRequest::AddSymbolToProject) {
        switch (response->symbolType()) {
                case KTLibraryObject::Sound:
                {
                     KTLayerManager *layerManager = this->layerManager(response->sceneIndex());
                     if (layerManager) {
                         layerManager->insertSoundLayer(response->layerIndex()+1, 
                                                        response->arg().toString());
                    
                         KTFramesTable *framesTable = this->framesTable(response->sceneIndex());
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

void KTTimeLine::requestCommand(int action)
{
    int scenePos = k->container->currentIndex();

    if (scenePos < 0)
        return;
    
    int layerPos = layerManager(scenePos)->getLayerIndex()->verticalHeader()->visualIndex(
                   layerManager(scenePos)->getLayerIndex()->currentRow());

    int framePos = framesTable(scenePos)->lastFrameByLayer(layerPos) + 1;

    if (!requestFrameAction(action, framePos, layerPos, scenePos)) {
        kFatal() << "KTTimeLine::requestCommand -> It isn't frame action";
        layerPos = layerManager(scenePos)->getLayerIndex()->rowCount();
        framePos = framesTable(scenePos)->lastFrameByLayer(layerPos);
        if (!requestLayerAction(action, layerPos, scenePos)) {
            kFatal() << "KTTimeLine::requestCommand -> It isn't layer action";
            if (!requestSceneAction(action, scenePos)) {
                #ifdef K_DEBUG
                    kFatal("timeline") << "Can't handle action";
                #endif
            }
        }
    }
}

bool KTTimeLine::requestFrameAction(int action, int framePos, int layerPos, int scenePos, const QVariant &arg)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    if (scenePos < 0)
        scenePos = k->container->currentIndex();

    switch (action) {
            case KTProjectActionBar::InsertFrame:
            {
                 int layersTotal = layerManager(scenePos)->getLayerIndex()->rowCount();
                 int usedFrames = framesTable(scenePos)->lastFrameByLayer(layerPos);

                 if (layersTotal == 1) {
                     for (int frame = usedFrames; frame < framePos; frame++) {
                          KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, frame + 1,
                                                   KTProjectRequest::Add, arg);
                          emit requestTriggered(&event);
                     }

                 } else {
                     usedFrames++;

                     for (int layer=0; layer < layersTotal; layer++) {
                          for (int frame = usedFrames; frame <= framePos; frame++) {
                               KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layer, frame,
                                                        KTProjectRequest::Add, arg);
                               emit requestTriggered(&event);
                          }
                     }
                 }
            
                 return true;
            }
            break;
            case KTProjectActionBar::RemoveFrame:
            {
                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, framePos - 1,
                                          KTProjectRequest::Remove, arg);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveFrameUp:
            {
                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          KTProjectRequest::Move, framePos-1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveFrameDown:
            {
                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          KTProjectRequest::Move, framePos+1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            /*
            case KTProjectActionBar::SelectFrame:
            {
                 KTProjectRequest event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                          KTProjectRequest::Select, arg);
                 emit localRequestTriggered(&event);

                 return true;
            }
            break;
            */
    }
    
    return false;
}

bool KTTimeLine::requestLayerAction(int action, int layerPos, int scenePos, const QVariant &arg)
{
    if (scenePos < 0)
        scenePos = k->container->currentIndex();
    
    if (scenePos >= 0) {
        if (layerPos < 0)
            layerPos = layerManager(scenePos)->getLayerIndex()->verticalHeader()->visualIndex(
                                    layerManager(scenePos)->getLayerIndex()->currentRow());
    }

    switch (action) {
            case KTProjectActionBar::InsertLayer:
            {
                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(scenePos, layerPos,
                                          KTProjectRequest::Add, arg);
                 emit requestTriggered(&event);

                 if (layerPos == 0) {
                     event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, 0,
                                               KTProjectRequest::Add, arg);
                     emit requestTriggered(&event);
                 } else {
                     int total = framesTable(scenePos)->lastFrameByLayer(layerPos-1);
                     for (int j=0; j <= total; j++) {
                          event = KTRequestBuilder::createFrameRequest(scenePos, layerPos, j,
                                                                       KTProjectRequest::Add, arg);
                          emit requestTriggered(&event);
                     }
                 }

                 return true;
            }
            break;
            case KTProjectActionBar::RemoveLayer:
            {
                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(scenePos, layerPos-1, 
                                          KTProjectRequest::Remove, arg);
            
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveLayerUp:
            {
                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                          KTProjectRequest::Move, layerPos - 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveLayerDown:
            {
                 KTProjectRequest event = KTRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                          KTProjectRequest::Move, layerPos + 1);
                 emit requestTriggered(&event);
                 return true;
            }
            break;
    }
    
    return false;
}

bool KTTimeLine::requestSceneAction(int action, int scenePos, const QVariant &arg)
{
    if (scenePos < 0)
        scenePos = k->container->currentIndex();
    
    switch (action) {
            case KTProjectActionBar::InsertScene:
            {
                 KTProjectRequest event = KTRequestBuilder::createSceneRequest(scenePos + 1, 
                                           KTProjectRequest::Add, arg);
            
                 emit requestTriggered(&event);
            
                 return true;
            }
            break;
            case KTProjectActionBar::RemoveScene:
            {
                 KTProjectRequest event = KTRequestBuilder::createSceneRequest(scenePos, 
                                          KTProjectRequest::Remove, arg);
            
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveSceneUp:
            {
                 KTProjectRequest event = KTRequestBuilder::createSceneRequest(scenePos, KTProjectRequest::Move,
                                          scenePos + 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case KTProjectActionBar::MoveSceneDown:
            {
                 KTProjectRequest event = KTRequestBuilder::createSceneRequest(scenePos, KTProjectRequest::Move,
                                     scenePos - 1);
                 emit requestTriggered(&event);
                 return true;
            }
            break;
    }
    
    return false;
}

void KTTimeLine::emitRequestRenameLayer(int layer, const QString &name)
{
    #ifdef K_DEBUG
        K_FUNCINFO << name;
    #endif
    int scenePos = k->container->currentIndex();
    
    KTProjectRequest event = KTRequestBuilder::createLayerRequest(scenePos, layer, KTProjectRequest::Rename, name);
    
    emit requestTriggered(&event);
}

void KTTimeLine::emitSelectionSignal()
{
    kFatal() << "KTTimeLine::emitSelectionSignal() - Just tracing!";

    int scenePos = k->container->currentIndex();
    int layerPos = layerManager(scenePos)->getLayerIndex()->currentRow();
    k->selectedLayer = layerPos;
    int frame = framesTable(scenePos)->currentColumn();

    requestFrameAction(KTProjectRequest::Select, frame, layerPos, scenePos);
}

void KTTimeLine::emitLayerVisibility(int sceneIndex, int layerIndex, bool checked) 
{
    KTProjectRequest event = KTRequestBuilder::createLayerRequest(sceneIndex,
                             layerIndex, KTProjectRequest::View, checked);
    emit requestTriggered(&event);
}

void KTTimeLine::selectFrame(int indexLayer, int indexFrame)
{
    int scenePos = k->container->currentIndex();

    kFatal() << "KTTimeLine::selectFrame() - Just tracing!";
    KTProjectRequest request = KTRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                 indexFrame, KTProjectRequest::Select, "1");
    emit requestTriggered(&request);

}

void KTTimeLine::emitRequestChangeScene(int sceneIndex)
{
    if (k->container->count() > 1) {
        kFatal() << "KTTimeLine::emitRequestChangeScene - Just tracing!";
        KTProjectRequest request = KTRequestBuilder::createSceneRequest(sceneIndex, KTProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void KTTimeLine::emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex)
{
    kFatal() << "KTTimeLine::emitRequestChangeFrame - Just tracing!";
    KTProjectRequest event = KTRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                             KTProjectRequest::Select, "1");
    emit requestTriggered(&event);
}


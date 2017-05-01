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
    QMenu *singleMenu;
    // QMenu *multipleMenu;
    bool localRequest;
    int previousScene;
    int previousLayer;

    QList<QString> framesList;
    QList<TupExposureTable::FrameType> statesList;
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
    k->localRequest = false;
    k->previousScene = 0;
    k->previousLayer = 0;

    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QPixmap(kAppProp->themeDir() + "icons/exposure_sheet.png"));

    QList<TupProjectActionBar::Action> generalActions;
    generalActions << TupProjectActionBar::InsertLayer << TupProjectActionBar::RemoveLayer;
    generalActions << TupProjectActionBar::Separator;
    generalActions << TupProjectActionBar::InsertScene << TupProjectActionBar::RemoveScene;

    k->actionBar = new TupProjectActionBar(QString("Exposure"), generalActions);

    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(applyAction(int)));
    addChild(k->actionBar, Qt::AlignCenter);

    QList<TupProjectActionBar::Action> frameActions;
    frameActions << TupProjectActionBar::InsertFrame << TupProjectActionBar::ExtendFrame << TupProjectActionBar::RemoveFrame;
    frameActions << TupProjectActionBar::Separator;
    frameActions << TupProjectActionBar::MoveFrameBackward << TupProjectActionBar::MoveFrameForward; 
    frameActions << TupProjectActionBar::Separator;
    frameActions << TupProjectActionBar::CopyFrame;
    frameActions << TupProjectActionBar::PasteFrame;

    TupProjectActionBar *framesBar = new TupProjectActionBar(QString("Exposure"), frameActions);

    connect(framesBar, SIGNAL(actionSelected(int)), this, SLOT(applyAction(int)));
    addChild(framesBar, Qt::AlignCenter);

    k->scenesContainer = new TupSceneTabWidget(this);
    connect(k->scenesContainer, SIGNAL(currentChanged(int)), this, SLOT(requestChangeScene(int)));
    connect(k->scenesContainer, SIGNAL(updateLayerOpacity(double)), this, SLOT(requestUpdateLayerOpacity(double)));
    addChild(k->scenesContainer);
    createMenuForAFrame();
    // createMenuForSelection();
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

void TupExposureSheet::createMenuForAFrame()
{
    k->singleMenu = new QMenu(tr("actions"));
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

    k->singleMenu->addMenu(insertMenu);

    QAction *removeAction = new QAction(QIcon(THEME_DIR + "icons/remove_frame.png"), tr("Remove frame"), this);
    removeAction->setIconVisibleInMenu(true);
    k->singleMenu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFrame()));

    QAction *clearAction = new QAction(QIcon(THEME_DIR + "icons/new.png"), tr("Clear frame"), this);
    clearAction->setIconVisibleInMenu(true);
    k->singleMenu->addAction(clearAction);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearFrame()));

    QAction *copyAction = new QAction(QIcon(THEME_DIR + "icons/copy.png"), tr("Copy frame"), this);
    copyAction->setIconVisibleInMenu(true);
    k->singleMenu->addAction(copyAction);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(requestCopyFrameSelection()));

    QAction *pasteAction = new QAction(QIcon(THEME_DIR + "icons/paste.png"), tr("Paste in frame"), this);
    pasteAction->setIconVisibleInMenu(true);
    k->singleMenu->addAction(pasteAction);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(requestPasteSelectionInCurrentFrame()));

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

    k->singleMenu->addMenu(timeLineMenu);
    connect(k->singleMenu, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));
}

/* SQA:: Check if it's possible to use a pop up menu for multiple frames selection
void TupExposureSheet::createMenuForSelection()
{
    k->multipleMenu = new QMenu(tr("actions"));

    QAction *removeAction = new QAction(QIcon(THEME_DIR + "icons/remove_frame.png"), tr("Remove frames"), this);
    removeAction->setIconVisibleInMenu(true);
    k->multipleMenu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFrame()));
}
*/

void TupExposureSheet::addScene(int sceneIndex, const QString &name)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::addScene()] - index: " << QString::number(sceneIndex) << " name: " << name;
        #else
            T_FUNCINFO << " index: " << sceneIndex << " name: " << name;
        #endif
    #endif

    TupExposureTable *scene = new TupExposureTable;
    scene->setSinglePopUpMenu(k->singleMenu);

    connect(scene, SIGNAL(frameUsed(int, int)), this, SLOT(insertFrame(int, int)));
    connect(scene, SIGNAL(frameRenamed(int, int, const QString &)), this, SLOT(renameFrame(int, int, const QString &)));
    connect(scene, SIGNAL(frameSelected(int, int)), SLOT(selectFrame(int, int)));
    connect(scene, SIGNAL(frameRemoved()), SLOT(removeFrame()));
    connect(scene, SIGNAL(frameCopied(int, int)), SLOT(extendFrameForward(int, int)));
    connect(scene, SIGNAL(layerNameChanged(int, const QString &)), this, SLOT(requestRenameLayer(int, const QString &)));
    connect(scene, SIGNAL(layerMoved(int, int)), this, SLOT(moveLayer(int, int)));
    connect(scene, SIGNAL(layerVisibilityChanged(int, bool)), this, SLOT(changeLayerVisibility(int, bool)));

    k->scenesContainer->addScene(sceneIndex, name, scene);
}

void TupExposureSheet::renameScene(int sceneIndex, const QString &name)
{
    k->scenesContainer->renameScene(sceneIndex, name);
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

    // SQA: Refactor scene, layer and frame index variables. Set them before the switch
    // i.e. int sceneIndex = k->scenesContainer->currentIndex();

    switch (action) {
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

            case TupProjectActionBar::ExtendFrame:
               {
                 extendFrameForward(k->currentTable->currentLayer(), k->currentTable->currentFrame());
               }
               break;

            case TupProjectActionBar::RemoveFrame:
               {
                 k->localRequest = true;
                 QList<int> coords = k->currentTable->currentSelection();

                 if (coords.count() == 4) {
                     int scene = k->scenesContainer->currentIndex();
                     int layers = coords.at(1) - coords.at(0) + 1;
                     int frames = coords.at(3) - coords.at(2) + 1;

                     QString flags = "";
                     for (int i=coords.at(0); i<=coords.at(1); i++) {
                         if (k->currentTable->framesCountAtLayer(i) > frames)
                             flags += "0,";
                         else
                             flags += "1,";
                     }
                     flags.chop(1); 

                     QString selection = QString::number(layers) + "," + QString::number(frames) + ":" + flags;

                     TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene, coords.at(0), coords.at(2), 
                                                                                       TupProjectRequest::RemoveSelection, selection);
                     emit requestTriggered(&request);
                 }
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

            case TupProjectActionBar::CopyFrame:
               {
                 requestCopyFrameSelection();
               }
               break;

            case TupProjectActionBar::PasteFrame:
               {
                 requestPasteSelectionInCurrentFrame();
               }
               break;

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

            case TupProjectActionBar::InsertScene:
               {
                 int sceneTarget = k->scenesContainer->count();
                 TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneTarget, TupProjectRequest::Add, tr("Scene %1").arg(sceneTarget + 1));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createLayerRequest(sceneTarget, 0, TupProjectRequest::Add, tr("Layer 1"));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createFrameRequest(sceneTarget, 0, 0, TupProjectRequest::Add, tr("Frame"));
                 emit requestTriggered(&request);

                 request = TupRequestBuilder::createSceneRequest(sceneTarget, TupProjectRequest::Select);
                 emit requestTriggered(&request);
               }
               break;

            case TupProjectActionBar::RemoveScene:
               {
                 int scenesTotal = k->scenesContainer->count();
                 int sceneIndex = k->scenesContainer->currentIndex();

                 TupProjectRequest request;
                 if (scenesTotal > 1) {
                     request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Remove);
                     emit requestTriggered(&request);

                     request = TupRequestBuilder::createFrameRequest(sceneIndex - 1, 0, 0, TupProjectRequest::Select, "1");
                     emit requestTriggered(&request);
                 } else {
                     request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Reset, tr("Scene 1"));
                     emit requestTriggered(&request);
                 }
               }
               break;
    }
}

void TupExposureSheet::setScene(int sceneIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::setScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->scenesContainer->isTableIndexValid(sceneIndex)) {
        k->scenesContainer->blockSignals(true);
        k->scenesContainer->setCurrentIndex(sceneIndex);		
        k->currentTable = k->scenesContainer->getTable(sceneIndex);
        k->scenesContainer->blockSignals(false);
    } else {
        #ifdef K_DEBUG
            QString msg1 = "TupExposureSheet::setScene() - Invalid scene index -> " + QString::number(sceneIndex);
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

void TupExposureSheet::requestChangeScene(int sceneIndex)
{
    if (k->scenesContainer->count() > 1) {
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupExposureSheet::requestCopyFrameSelection()
{
    QList<int> coords = k->currentTable->currentSelection();
    if (coords.count() == 4) {
        QString selection = QString::number(coords.at(0)) + "," + QString::number(coords.at(1)) + ","
                            + QString::number(coords.at(2)) + "," + QString::number(coords.at(3));

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(),
                                                       k->currentTable->currentLayer(),
                                                       k->currentTable->currentFrame(),
                                                       TupProjectRequest::CopySelection, selection);
        emit requestTriggered(&request);
    }
}

void TupExposureSheet::requestPasteSelectionInCurrentFrame()
{
    if (!k->framesList.isEmpty()) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(),
                                                       k->currentTable->currentLayer(),
                                                       k->currentTable->currentFrame(),
                                                       TupProjectRequest::PasteSelection);
        emit requestTriggered(&request);
    }
}

void TupExposureSheet::requestExtendCurrentFrame(int times)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(),
                                                   k->currentTable->currentLayer(),
                                                   k->currentTable->currentFrame(),
                                                   TupProjectRequest::Extend, times);
    emit requestTriggered(&request);
}

void TupExposureSheet::insertFrame(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::insertFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), 
                                                   layerIndex, frameIndex, TupProjectRequest::Add, tr("Frame"));
    emit requestTriggered(&request);
}

void TupExposureSheet::renameFrame(int layerIndex, int frameIndex, const QString & name)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), layerIndex, frameIndex,
                                                   TupProjectRequest::Rename, name);
    emit requestTriggered(&request);
}

void TupExposureSheet::selectFrame(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::selectFrame()] - Layer: " << layerIndex;
            qDebug() << "[TupExposureSheet::selectFrame()] - Frame: " << frameIndex;
        #else
            T_FUNCINFO;
            tWarning() << "Layer -> " << layerIndex;
            tWarning() << "Frame -> " << frameIndex; 
        #endif
    #endif

    QList<QTableWidgetItem *> list = k->currentTable->selectedItems();
    QString selection = "";
    QList<int> coords = k->currentTable->currentSelection();

    if (coords.count() == 4) {
        if ((layerIndex >= coords.at(0) && layerIndex <= coords.at(1)) && 
            (frameIndex >= coords.at(2) && frameIndex <= coords.at(3))) {
            selection = QString::number(coords.at(0)) + "," + QString::number(coords.at(1)) + "," 
                        + QString::number(coords.at(2)) + "," + QString::number(coords.at(3));
        } else {
            selection = QString::number(layerIndex) + "," + QString::number(layerIndex) + "," 
                        + QString::number(frameIndex) + "," + QString::number(frameIndex);  
        }
    } else {
        selection = QString::number(layerIndex) + "," + QString::number(layerIndex) + ","
                    + QString::number(frameIndex) + "," + QString::number(frameIndex);
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scenesContainer->currentIndex(), layerIndex, 
                                                   frameIndex, TupProjectRequest::Select, selection);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::removeFrame()
{
    k->actionBar->emitActionSelected(TupProjectActionBar::RemoveFrame);
}

void TupExposureSheet::extendFrameForward(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureSheet::extendFrameForward()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = k->scenesContainer->currentIndex();
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                                                                      TupProjectRequest::Extend, 1);
    emit requestTriggered(&request);
}

void TupExposureSheet::changeLayerVisibility(int layerIndex, bool visibility)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), layerIndex,
                                                   TupProjectRequest::View, visibility);
    emit localRequestTriggered(&request);
}

void TupExposureSheet::requestRenameLayer(int layerIndex, const QString & name)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->scenesContainer->currentIndex(), layerIndex,
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

    blockSignals(true);
    k->currentTable = 0;
    k->scenesContainer->removeAllTabs();
    blockSignals(false);
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
                    TupScene *scene = k->project->sceneAt(sceneIndex);
                    if (scene)
                        k->scenesContainer->restoreScene(sceneIndex, scene->sceneName());
                    return;
                }
            }
           break;
           case TupProjectRequest::Remove:
            {
                k->scenesContainer->removeScene(sceneIndex, true);
            }
           break;
           case TupProjectRequest::Reset:
            {
                if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                    k->scenesContainer->removeScene(sceneIndex, true);
                    addScene(sceneIndex, tr("Scene %1").arg(sceneIndex + 1));

                    setScene(sceneIndex);
                    k->currentTable->insertLayer(0, tr("Layer 1"));
                    k->currentTable->insertFrame(0, 0, tr("Frame"), false);

                    k->currentTable->blockSignals(true); 
                    k->currentTable->selectFrame(0, 0);
                    k->currentTable->blockSignals(false);
                }

                if (response->mode() == TupProjectResponse::Undo) {
                    k->scenesContainer->removeScene(sceneIndex, false);
                    k->scenesContainer->restoreScene(sceneIndex, response->arg().toString());

                    setScene(sceneIndex);

                    k->currentTable->blockSignals(true);
                    k->currentTable->selectFrame(0, 0); 
                    k->currentTable->blockSignals(false);
                }
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
                         TupScene *scene = k->project->sceneAt(sceneIndex);
                         if (scene) {
                             TupLayer *layer = scene->layerAt(layerIndex); 
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
                          TupScene *scene = k->project->sceneAt(sceneIndex);
                          if (scene) {
                              TupLayer *layer = scene->layerAt(layerIndex);
                              if (layer) {
                                  TupFrame *frame = layer->frameAt(frameIndex);
                                  if (frame) {
                                      table->insertFrame(layerIndex, frameIndex, frame->frameName(), response->external());
                                      if (!frame->isEmpty())
                                          table->updateFrameState(layerIndex, frameIndex, TupExposureTable::Used);
                                  }
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
                             table->removeFrame(layerIndex, frameIndex);
                         } else {
                             int lastFrame = table->framesCountAtCurrentLayer() - 1;
                             int target = frameIndex;
                             if (target == lastFrame) {
                                 table->removeFrame(layerIndex, target);
                                 if (target <= 0)
                                     table->clearSelection();
                             } else {
                                 // When the item deleted is not the last one
                                 TupScene *scene = k->project->sceneAt(sceneIndex);
                                 if (scene) {
                                     TupLayer *layer = scene->layerAt(layerIndex);
                                     if (layer) {
                                         for (int index=target+1; index <= lastFrame; index++) {
                                              TupFrame *frame = layer->frameAt(index-1);
                                              TupExposureTable::FrameType type = TupExposureTable::Empty;
                                              if (!frame->isEmpty())
                                                  type = TupExposureTable::Used;
                                              table->updateFrameState(layerIndex, index - 1, type);
                                              QString label = k->currentTable->frameName(layerIndex, index);
                                              renameFrame(layerIndex, index - 1, label);
                                         }
                                         table->removeFrame(layerIndex, lastFrame);
                                     }
                                 }
                             }
                         }

                         return;
                     }

                     if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                         int lastFrame = table->framesCountAtCurrentLayer() - 1;
                         int target = frameIndex;
                         if (target == lastFrame) {
                             table->removeFrame(layerIndex, frameIndex);
                             if (frameIndex > 0)
                                 frameIndex--;
                             table->selectFrame(layerIndex, frameIndex);
                         } else {
                             table->removeFrame(layerIndex, frameIndex);
                         }

                         return;
                     }
                  }
                break;
                case TupProjectRequest::RemoveSelection: 
                  {
                      QString selection = response->arg().toString();
                      QStringList blocks = selection.split(":");

                      QStringList params = blocks.at(0).split(",");
                      int layers = params.at(0).toInt();
                      int frames = params.at(1).toInt();
                      QStringList flags = blocks.at(1).split(","); 

                      if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                          removeBlock(table, layerIndex, frameIndex, layers, frames);
                      } else {
                          TupScene *scene = k->project->sceneAt(sceneIndex);
                          if (scene) {
                              int layersTotal = layerIndex + layers;
                              for (int i=layerIndex,index=0; i<layersTotal; i++,index++) {
                                  TupLayer *layer = scene->layerAt(i);
                                  if (layer) {
                                      bool remove = flags.at(index).toInt();
                                      if (remove)
                                          table->removeFrame(i, 0);
                                      int framesTotal = frameIndex + frames;
                                      for (int j=frameIndex; j<framesTotal; j++) {
                                          TupFrame *frame = layer->frameAt(j);
                                          if (frame) {
                                              table->insertFrame(i, j, tr("Frame"), response->external());
                                              if (!frame->isEmpty())
                                                  table->updateFrameState(i, j, TupExposureTable::Used);
                                          }
                                      }
                                  }
                              }

                              table->selectFrame(layerIndex, frameIndex);
                          }
                      }
                  }
                break;
                case TupProjectRequest::Reset:
                  {
                      if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                          table->updateFrameState(layerIndex, frameIndex, TupExposureTable::Empty);
                          table->setFrameName(layerIndex, frameIndex, tr("Frame"));
                      } else {
                          TupScene *scene = k->project->sceneAt(sceneIndex);
                          if (scene) {
                              TupLayer *layer = scene->layerAt(layerIndex);
                              if (layer) {
                                  TupFrame *frame = layer->frameAt(frameIndex);
                                  if (frame) {
                                      TupExposureTable::FrameType state = TupExposureTable::Used;
                                      if (frame->isEmpty())
                                          state = TupExposureTable::Empty;
                                      table->updateFrameState(layerIndex, frameIndex, state);
                                      table->setFrameName(layerIndex, frameIndex, frame->frameName());
                                  }
                              }
                          }
                      } 

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
                case TupProjectRequest::Rename:
                  {
                      table->setFrameName(layerIndex, frameIndex, response->arg().toString());
                  }
                break;
                case TupProjectRequest::Select:
                  {
                      table->blockSignals(true);
                      table->selectFrame(layerIndex, response->arg().toString());
                      table->blockSignals(false);

                      if (k->previousScene != sceneIndex || k->previousLayer != layerIndex) {
                          k->previousScene = sceneIndex;
                          k->previousLayer = layerIndex;
                          updateLayerOpacity(sceneIndex, layerIndex);
                      }
                  }
                break;
                case TupProjectRequest::Extend:
                  {
                      int times = response->arg().toInt();
                      if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                          QString frameName = k->currentTable->frameName(layerIndex, frameIndex);
                          TupExposureTable::FrameType state = k->currentTable->frameState(layerIndex, frameIndex);

                          for (int i=1; i<=times; i++) {
                              table->insertFrame(layerIndex, frameIndex + i, frameName, response->external());
                              table->updateFrameState(layerIndex, frameIndex + i, state);
                          }

                          table->clearSelection();

                          table->blockSignals(true);
                          table->selectFrame(layerIndex, frameIndex + times);
                          table->blockSignals(false);

                          selectFrame(layerIndex, frameIndex + times);
                      } else {
                          removeBlock(table, layerIndex, frameIndex, 1, times);
                      }
                  }
                break;
                case TupProjectRequest::CopySelection:
                  {
                      if (response->mode() == TupProjectResponse::Do) {
                          QString selection = response->arg().toString();
                          QStringList params = selection.split(",");

                          if (params.count() == 4) {
                              QList<int> coords;
                              foreach(QString item, params) 
                                  coords << item.toInt();

                              k->framesList.clear();
                              k->statesList.clear();
                              for (int i=coords.at(0); i<=coords.at(1); i++) {
                                  for (int j=coords.at(2); j<=coords.at(3); j++) {
                                      k->framesList << table->frameName(i, j);
                                      k->statesList << table->frameState(i, j);
                                  }
                              }
                          }
                      }
                  }
                break;
                case TupProjectRequest::PasteSelection:
                  {
                      QString selection = response->arg().toString();
                      QStringList params = selection.split(",");

                      if (params.count() == 4) {
                          QList<int> coords;
                          foreach(QString item, params) 
                              coords << item.toInt();

                          int layersTotal = (coords.at(1) - coords.at(0)) + 1;
                          int framesTotal = (coords.at(3) - coords.at(2)) + 1; 
                          int layerLimit = layerIndex + layersTotal;
                          int frameLimit = frameIndex + framesTotal;

                          if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                              int index = 0;
                              for (int i=layerIndex; i<layerLimit; i++) {
                                  for (int j=frameIndex; j<frameLimit; j++) {
                                      table->insertFrame(i, j, k->framesList.at(index), response->external());
                                      table->updateFrameState(i, j, k->statesList.at(index));
                                      index++;
                                  }
                              }
                          } else {
                              removeBlock(table, layerIndex, frameIndex, layersTotal, framesTotal);
                          }
                      }
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

void TupExposureSheet::insertFrames(int times)
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
        for (int i=0; i<times; i++)
             insertFrame(layer, k->currentTable->framesCountAtCurrentLayer());
        selectFrame(layer, k->currentTable->currentFrame() + 1);
    } else {
        int frame = k->currentTable->currentFrame() + 1; 
        for (int i=0; i<times; i++)
             insertFrame(layer, k->currentTable->framesCountAtCurrentLayer());

        for (int index=lastFrame; index >= target; index--) {
             TupProjectRequest event = TupRequestBuilder::createFrameRequest(scene, layer, index, TupProjectRequest::Exchange, index + times);
             emit requestTriggered(&event);
        }

        selectFrame(layer, frame);
    }
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

void TupExposureSheet::updateFramesState()
{
    for (int i=0; i < k->project->scenesCount(); i++) {
         TupScene *scene = k->project->sceneAt(i);
         TupExposureTable *tab = k->scenesContainer->getTable(i);
         for (int j=0; j < scene->layersCount(); j++) {
              TupLayer *layer = scene->layerAt(j);
              for (int k=0; k < layer->framesCount(); k++) {
                   TupFrame *frame = layer->frameAt(k);
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
                                                           currentLayer, j, TupProjectRequest::Copy); 
            emit localRequestTriggered(&request);

            int frameIndex = k->currentTable->usedFrames(currentLayer);
            insertFrame(currentLayer, frameIndex);

            request = TupRequestBuilder::createFrameRequest(currentScene,
                                                           currentLayer, frameIndex,
                                                           TupProjectRequest::Paste);
            emit localRequestTriggered(&request);
        }
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(currentScene, currentLayer,
                                                   currentFrame, TupProjectRequest::Select, "1");
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
    TupScene *scene = k->project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "TupExposureSheet::initLayerVisibility()";
        #else
            T_FUNCINFO << "TupExposureSheet::initLayerVisibility()";
        #endif
    #endif

    int scenes = k->project->scenesCount(); 
    for (int sceneIndex=0; sceneIndex < scenes; sceneIndex++) {
         TupScene *scene = k->project->sceneAt(sceneIndex);
         if (scene) {
             int layers = scene->layersCount();
             for (int layerIndex=0; layerIndex < layers; layerIndex++) {
                  TupLayer *layer = scene->layerAt(layerIndex);
                  k->scenesContainer->getTable(sceneIndex)->setLayerVisibility(layerIndex, layer->isVisible());
             }
         }
    }
}

void TupExposureSheet::removeBlock(TupExposureTable *table, int layerIndex, int frameIndex, 
                                   int layersTotal, int framesTotal)
{
    int layerPos = layerIndex;
    int framePos;
    for (int i=0; i<layersTotal; i++) {
        framePos = frameIndex;
        for (int j=0; j<framesTotal; j++) {
            table->removeFrame(layerPos, framePos);
            framePos++;
        }
        layerPos++;
    }

    int init = frameIndex + framesTotal;
    int lastIndex = table->framesCountAtCurrentLayer() + framesTotal;
    layerPos = layerIndex;
    for (int i=0; i<layersTotal; i++) {
        framePos = frameIndex;
        for (int j=init; j<lastIndex; j++) {
            QTableWidgetItem *item = table->takeItem(j, layerPos);
            table->setItem(framePos, layerPos, item);
            framePos++;
        }
        layerPos++;
    }

    layerPos = layerIndex;
    for (int i=0; i<layersTotal; i++) {
        if (table->framesCountAtLayer(layerPos) == 0)
            table->insertFrame(layerPos, 0, tr("Frame"), false);
        layerPos++;
    }

    lastIndex = table->framesCountAtLayer(layerIndex) - 1;
    if (lastIndex < frameIndex)
        table->selectFrame(layerIndex, lastIndex);
    else
        table->selectFrame(layerIndex, frameIndex);
}

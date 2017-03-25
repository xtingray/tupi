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

#include "tupprojectcommand.h"
#include "tupcommandexecutor.h"
#include "tupprojectrequest.h"
#include "tuppaintareaevent.h"
#include "tuprequestparser.h"
#include "tupprojectresponse.h"
#include "tupsvg2qt.h"

struct TupProjectCommand::Private 
{
    TupCommandExecutor *executor;
    TupProjectResponse *response;
    bool executed;
};

TupProjectCommand::TupProjectCommand(TupCommandExecutor *executor, const TupProjectRequest *request) : QUndoCommand(), k(new Private())
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupRequestParser parser;
    if (!parser.parse(request->xml())) {
        #ifdef K_DEBUG
            QString msg = "TupProjectCommand::TupProjectCommand(): - Fatal error: request xml can't be parsed!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
        return;
    }

    k->executor = executor;
    k->executed = false;
    k->response = parser.response();
    k->response->setExternal(request->isExternal());

    if (!k->response) {
        #ifdef K_DEBUG
            QString msg = "TupProjectCommand::TupProjectCommand() - Unparsed response!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
    }

    initText();
}

TupProjectCommand::TupProjectCommand(TupCommandExecutor *executor, TupProjectResponse *response) : QUndoCommand(), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
	
    k->executor = executor;
    k->response = response;
    k->executed = false;

    initText();
}

void TupProjectCommand::initText()
{
    switch (k->response->part()) {
            case TupProjectRequest::Frame:
             {
                 setText(actionString(k->response->action()) + " frame");
             }
             break;
            case TupProjectRequest::Layer:
             {
                 setText(actionString(k->response->action()) + " layer");
             }
             break;
            case TupProjectRequest::Scene:
             {
                 setText(actionString(k->response->action()) + " scene");
             }
            break;
            case TupProjectRequest::Item:
             {
                 setText(actionString(k->response->action()) + " item");
             }
            break;
            case TupProjectRequest::Library:
             {
                 setText(actionString(k->response->action()) + " symbol");
             }
            break;
            default:
             {				  
                 #ifdef K_DEBUG
                     QString msg = "TProjectCommand::initText() - Error: can't handle ID: " + QString::number(k->response->part());
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                #endif
             }
            break;
    }
}

QString TupProjectCommand::actionString(int action)
{
    switch(action)
    {
        case TupProjectRequest::Add:
        {
            return QObject::tr("add");
        }
        break;
        case TupProjectRequest::Remove:
        {
            return QObject::tr("remove");
        }
        break;
        case TupProjectRequest::Move:
        {
            return QObject::tr("move");
        }
        break;
        case TupProjectRequest::Lock:
        {
            return QObject::tr("lock");
        }
        break;
        case TupProjectRequest::Rename:
        {
            return QObject::tr("rename");
        }
        break;
        case TupProjectRequest::Select:
        {
            return QObject::tr("select");
        }
        break;
        case TupProjectRequest::EditNodes:
        {
            return QObject::tr("edit node");
        }
        break;
        case TupProjectRequest::Pen:
        {
            return QObject::tr("pen");
        }
        break;
        case TupProjectRequest::Brush:
        {
            return QObject::tr("brush");
        }
        break;
        case TupProjectRequest::View:
        {
            return QObject::tr("view");
        }
        break;
        case TupProjectRequest::Transform:
        {
            return QObject::tr("transform");
        }
        break;
        case TupProjectRequest::Convert:
        {
            return QObject::tr("convert");
        }
        break;
    }
    
    return QString("Unknown");
}

TupProjectCommand::~TupProjectCommand()
{
    delete k->response;
    delete k;
}

void TupProjectCommand::redo()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand::redo()] - Executing REDO action...";
            qDebug() << "[TupProjectCommand::redo()] - k->response->part(): " << k->response->part();
        #else
            T_FUNCINFO << "[TupProjectCommand::redo()] - Executing REDO action...";
            T_FUNCINFO << k->response->part();
        #endif
    #endif
    */
	
    if (k->executed) {
        k->response->setMode(TupProjectResponse::Redo);
    } else {
        k->response->setMode(TupProjectResponse::Do);
        k->executed = true;
    }
    
    switch (k->response->part()) {
            case TupProjectRequest::Project:
            {
                #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::redo() - Error: Project response isn't implemented";
                     #ifdef Q_OS_WIN
                         qWarning() << msg;
                     #else
                         tWarning() << msg;
                     #endif
                #endif
            }
            break;
            case TupProjectRequest::Frame:
            {
                 frameCommand();
            }
            break;
            case TupProjectRequest::Layer:
            {
                 layerCommand();
            }
            break;
            case TupProjectRequest::Scene:
            {
                 sceneCommand();
            }
            break;
            case TupProjectRequest::Item:
            {
                 itemCommand();
            }
            break;
            case TupProjectRequest::Library:
            {
                 libraryCommand();
            }
            break;
            default:
            {
                #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::redo() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                #endif
            }
            break;
    }
}

void TupProjectCommand::undo()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand::undo()] - Executing UNDO action...";
        #else
            T_FUNCINFO << "[TupProjectCommand::undo()] - Executing UNDO action...";
        #endif
    #endif

    k->response->setMode(TupProjectResponse::Undo);
    switch (k->response->part()) {
            case TupProjectRequest::Project:
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::undo() - Error: Project response isn't implemented";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
            case TupProjectRequest::Frame:
            {
                 frameCommand();
            }
            break;
            case TupProjectRequest::Layer:
            {
                 layerCommand();
            }
            break;
            case TupProjectRequest::Scene:
            {
                 sceneCommand();
            }
            break;
            case TupProjectRequest::Item:
            {
                 itemCommand();
            }
            break;
            case TupProjectRequest::Library:
            {
                 libraryCommand();
            }
            break;
            default:
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::undo() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::frameCommand()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand::frameCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupFrameResponse *response = static_cast<TupFrameResponse *>(k->response);

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 k->executor->createFrame(response);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 k->executor->removeFrame(response);
            }
            break;
            case TupProjectRequest::RemoveSelection:
            {
                 k->executor->removeFrameSelection(response);
            }
            break;
            case TupProjectRequest::Reset:
            {
                 k->executor->resetFrame(response);
            }
            break;
            case TupProjectRequest::Exchange:
            {
                 k->executor->exchangeFrame(response);
            }
            break;
            case TupProjectRequest::Move:
            {
                 k->executor->moveFrame(response);
            }
            break;
            case TupProjectRequest::Lock:
            {
                 k->executor->lockFrame(response);
            }
            break;
            case TupProjectRequest::Rename:
            {
                 k->executor->renameFrame(response);
            }
            break;
            case TupProjectRequest::Select:
            {
                 k->executor->selectFrame(response);
            }
            break;
            case TupProjectRequest::View:
            {
                 k->executor->setFrameVisibility(response);
            }
            break;
            case TupProjectRequest::Expand:
            {
                 k->executor->expandFrame(response);
            }
            break;
            case TupProjectRequest::Paste:
            {
                 k->executor->pasteFrame(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::frameCommand() - Fatal Error: Unknown project request";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::layerCommand()
{
    TupLayerResponse *response = static_cast<TupLayerResponse *>(k->response);

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 k->executor->createLayer(response);
            }
            break;
            case TupProjectRequest::AddLipSync:
            {
                 k->executor->addLipSync(response);
            }
            break;
            case TupProjectRequest::UpdateLipSync:
            {
                 k->executor->updateLipSync(response);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 k->executor->removeLayer(response);
            }
            break;
            case TupProjectRequest::RemoveLipSync:
            {
                 k->executor->removeLipSync(response);
            }
            break;
            case TupProjectRequest::Move:
            {
                 k->executor->moveLayer(response);
            }
            break;
            case TupProjectRequest::Lock:
            {
                 k->executor->lockLayer(response);
            }
            break;
            case TupProjectRequest::Rename:
            {
                 k->executor->renameLayer(response);
            }
            break;
            case TupProjectRequest::Select:
            {
                 k->executor->selectLayer(response);
            }
            break;
            case TupProjectRequest::View:
            {
                 k->executor->setLayerVisibility(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::layerCommand() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::sceneCommand()
{
    TupSceneResponse *response = static_cast<TupSceneResponse *>(k->response);

    switch (response->action()) {
            case TupProjectRequest::GetInfo:
            {
                 k->executor->getScenes(response);
            }
            case TupProjectRequest::Add:
            {
                 k->executor->createScene(response);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 k->executor->removeScene(response);
            }
            break;
            case TupProjectRequest::Reset:
            {
                 k->executor->resetScene(response);
            }
            break;
            case TupProjectRequest::Move:
            {
                 k->executor->moveScene(response);
            }
            break;
            case TupProjectRequest::Lock:
            {
                 k->executor->lockScene(response);
            }
            break;
            case TupProjectRequest::Rename:
            {
                 k->executor->renameScene(response);
            }
            break;
            case TupProjectRequest::Select:
            {
                 k->executor->selectScene(response);
            }
            break;
            case TupProjectRequest::View:
            {
                 k->executor->setSceneVisibility(response);
            }
            break;
            case TupProjectRequest::BgColor:
            {
                 k->executor->setBgColor(response);
            }
            break;

            default: 
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::sceneCommand() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::itemCommand()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand::itemCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    TupItemResponse *response = static_cast<TupItemResponse *>(k->response);

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 k->executor->createItem(response);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 k->executor->removeItem(response);
            }
            break;
            case TupProjectRequest::Move:
            {
                 k->executor->moveItem(response);
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
            case TupProjectRequest::Convert:
            {
                 k->executor->convertItem(response);
            }
            break;
            case TupProjectRequest::EditNodes:
            {
                 k->executor->setPathItem(response);
            }
            break;
            case TupProjectRequest::Pen:
            {
                 k->executor->setPen(response);
            }
            break;
            case TupProjectRequest::Brush:
            {
                 k->executor->setBrush(response);
            }
            break;
            case TupProjectRequest::Select:
            {
            }
            break;
            case TupProjectRequest::View:
            {
            }
            break;
            case TupProjectRequest::Transform:
            {
                 k->executor->transformItem(response);
            }
            break;
            case TupProjectRequest::Group:
            {
                 k->executor->groupItems(response);
            }
            break;
            case TupProjectRequest::Ungroup:
            {
                 k->executor->ungroupItems(response);
            }
            break;
            case TupProjectRequest::SetTween:
            {
                 k->executor->setTween(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::itemCommand() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::libraryCommand()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectCommand::libraryCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    
    TupLibraryResponse *response = static_cast<TupLibraryResponse *>(k->response);

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 k->executor->createSymbol(response);
            }
            break;

            case TupProjectRequest::Remove:
            {
                 k->executor->removeSymbol(response);
            }
            break;

            case TupProjectRequest::InsertSymbolIntoFrame:
            {
                 k->executor->insertSymbolIntoFrame(response);
            }
            break;

            case TupProjectRequest::RemoveSymbolFromFrame:
            {
                 k->executor->removeSymbolFromFrame(response);
            }
            break;

            default:
            {
                 #ifdef K_DEBUG
                     QString msg = "TupProjectCommand::libraryCommand() - Error: Unknown project response";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
            }
            break;
    }
}

void TupProjectCommand::paintAreaCommand()
{
    // SQA: FIX ME in tupprojectcommand.cpp
	
    /*
     if (redo)
         k->executor->reemitEvent(response);
     else
         k->executor->reemitEvent(response);
    */
}

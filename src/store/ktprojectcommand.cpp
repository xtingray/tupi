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

#include "ktprojectcommand.h"
#include "tdebug.h"
#include "ktcommandexecutor.h"
#include "ktprojectrequest.h"
#include "ktpaintareaevent.h"
#include "ktrequestparser.h"
#include "ktprojectresponse.h"
#include "ktsvg2qt.h"

struct KTProjectCommand::Private 
{
    KTCommandExecutor *executor;
    KTProjectResponse *response;
    bool executed;
};

KTProjectCommand::KTProjectCommand(KTCommandExecutor *executor, const KTProjectRequest *request) : QUndoCommand(), k(new Private())
{
    k->executor = executor;
    k->executed = false;

    KTRequestParser parser;
    if (!parser.parse(request->xml())) {
        #ifdef K_DEBUG
               tFatal() << "==> KTProjectCommand::KTProjectCommand(): - Parser error!";
        #endif
    }

    k->response = parser.response();
    k->response->setExternal(request->isExternal());

    if (!k->response) {
        #ifdef K_DEBUG
               tFatal() << "==> KTProjectCommand::KTProjectCommand() - Unparsed response!";
        #endif
    }

    initText();
}

KTProjectCommand::KTProjectCommand(KTCommandExecutor *executor, KTProjectResponse *response) : QUndoCommand(), k(new Private)
{
    k->executor = executor;
    k->response = response;
    k->executed = false;

    initText();
}

void KTProjectCommand::initText()
{
    switch (k->response->part()) {
            case KTProjectRequest::Frame:
             {
                 setText(actionString(k->response->action()) + " frame");
             }
             break;
            case KTProjectRequest::Layer:
             {
                 setText(actionString(k->response->action()) + " layer");
             }
             break;
            case KTProjectRequest::Scene:
             {
                 setText(actionString(k->response->action()) + " scene");
             }
            break;
            case KTProjectRequest::Item:
             {
                 setText(actionString(k->response->action()) + " item");
             }
            break;
            case KTProjectRequest::Library:
             {
                 setText(actionString(k->response->action()) + " symbol");
             }
            break;
            default:
             {
                 #ifdef K_DEBUG
                        tfDebug << "TProjectCommand::initText() - CAN'T HANDLE ID: " << k->response->part();
                  #endif
             }
            break;
    }
}

QString KTProjectCommand::actionString(int action)
{
    switch(action)
    {
        case KTProjectRequest::Add:
        {
            return QObject::tr("add");
        }
        break;
        case KTProjectRequest::Remove:
        {
            return QObject::tr("remove");
        }
        break;
        case KTProjectRequest::Move:
        {
            return QObject::tr("move");
        }
        break;
        case KTProjectRequest::Lock:
        {
            return QObject::tr("lock");
        }
        break;
        case KTProjectRequest::Rename:
        {
            return QObject::tr("rename");
        }
        break;
        case KTProjectRequest::Select:
        {
            return QObject::tr("select");
        }
        break;
        case KTProjectRequest::EditNodes:
        {
            return QObject::tr("edit node");
        }
        break;
        case KTProjectRequest::View:
        {
            return QObject::tr("view");
        }
        break;
        case KTProjectRequest::Transform:
        {
            return QObject::tr("transform");
        }
        break;
        case KTProjectRequest::Convert:
        {
            return QObject::tr("convert");
        }
        break;
    }
    
    return QString("Unknown");
}

KTProjectCommand::~KTProjectCommand()
{
    delete k->response;
    delete k;
}

void KTProjectCommand::redo()
{
    #ifdef K_DEBUG
           T_FUNCINFO << k->response->part();
    #endif    

    if (k->executed) {
        k->response->setMode(KTProjectResponse::Redo);
    } else {
        k->response->setMode(KTProjectResponse::Do);
        k->executed = true;
    }
    
    switch (k->response->part()) {
            case KTProjectRequest::Project:
            {
                 #ifdef K_DEBUG
                        tDebug() << "Project response isn't handle";
                 #endif
            }
            break;
            case KTProjectRequest::Frame:
            {
                 frameCommand();
            }
            break;
            case KTProjectRequest::Layer:
            {
                 layerCommand();
            }
            break;
            case KTProjectRequest::Scene:
            {
                 sceneCommand();
            }
            break;
            case KTProjectRequest::Item:
            {
                 itemCommand();
            }
            break;
            case KTProjectRequest::Library:
            {
                 libraryCommand();
            }
            break;
            default:
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::redo() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::undo()
{
    k->response->setMode(KTProjectResponse::Undo);
    
    switch (k->response->part()) {
            case KTProjectRequest::Project:
            {
                 #ifdef K_DEBUG
                        tDebug() << "Project response isn't handle";
                 #endif
            }
            break;
            case KTProjectRequest::Frame:
            {
                 frameCommand();
            }
            break;
            case KTProjectRequest::Layer:
            {
                 layerCommand();
            }
            break;
            case KTProjectRequest::Scene:
            {
                 sceneCommand();
            }
            break;
            case KTProjectRequest::Item:
            {
                 itemCommand();
            }
            break;
            case KTProjectRequest::Library:
            {
                 libraryCommand();
            }
            break;
            default:
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::undo() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::frameCommand()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    KTFrameResponse *response = static_cast<KTFrameResponse *>(k->response);

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 k->executor->createFrame(response);
            }
            break;
            case KTProjectRequest::Remove:
            {
                 k->executor->removeFrame(response);
            }
            break;
            case KTProjectRequest::Reset:
            {
                 k->executor->resetFrame(response);
            }
            break;
            case KTProjectRequest::Exchange:
            {
                 k->executor->exchangeFrame(response);
            }
            break;
            case KTProjectRequest::Move:
            {
                 k->executor->moveFrame(response);
            }
            break;
            case KTProjectRequest::Lock:
            {
                 k->executor->lockFrame(response);
            }
            break;
            case KTProjectRequest::Rename:
            {
                 k->executor->renameFrame(response);
            }
            break;
            case KTProjectRequest::Select:
            {
                 k->executor->selectFrame(response);
            }
            break;
            case KTProjectRequest::View:
            {
                 k->executor->setFrameVisibility(response);
            }
            break;
            case KTProjectRequest::Expand:
            {
                 k->executor->expandFrame(response);
            }
            break;
            case KTProjectRequest::Paste:
            {
                 k->executor->pasteFrame(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::frameCommand() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::layerCommand()
{
    KTLayerResponse *response = static_cast<KTLayerResponse *>(k->response);
    
    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 k->executor->createLayer(response);
            }
            break;
            case KTProjectRequest::Remove:
            {
                 k->executor->removeLayer(response);
            }
            break;
            case KTProjectRequest::Move:
            {
                 k->executor->moveLayer(response);
            }
            break;
            case KTProjectRequest::Lock:
            {
                 k->executor->lockLayer(response);
            }
            break;
            case KTProjectRequest::Rename:
            {
                 k->executor->renameLayer(response);
            }
            break;
            case KTProjectRequest::Select:
            {
                 k->executor->selectLayer(response);
            }
            break;
            case KTProjectRequest::View:
            {
                 k->executor->setLayerVisibility(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::layerCommand() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::sceneCommand()
{
    KTSceneResponse *response = static_cast<KTSceneResponse *>(k->response);

    switch (response->action()) {
            case KTProjectRequest::GetInfo:
            {
                 k->executor->getScenes(response);
            }
            case KTProjectRequest::Add:
            {
                 k->executor->createScene(response);
            }
            break;
            case KTProjectRequest::Remove:
            {
                 k->executor->removeScene(response);
            }
            break;
            case KTProjectRequest::Reset:
            {
                 k->executor->resetScene(response);
            }
            break;
            case KTProjectRequest::Move:
            {
                 k->executor->moveScene(response);
            }
            break;
            case KTProjectRequest::Lock:
            {
                 k->executor->lockScene(response);
            }
            break;
            case KTProjectRequest::Rename:
            {
                 k->executor->renameScene(response);
            }
            break;
            case KTProjectRequest::Select:
            {
                 k->executor->selectScene(response);
            }
            break;
            case KTProjectRequest::View:
            {
                 k->executor->setSceneVisibility(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::sceneCommand() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::itemCommand()
{
    /*
    #ifdef K_DEBUG
        T_FUNCINFO;
    #endif
    */

    KTItemResponse *response = static_cast<KTItemResponse *>(k->response);

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 k->executor->createItem(response);
            }
            break;
            case KTProjectRequest::Remove:
            {
                 k->executor->removeItem(response);
            }
            break;
            case KTProjectRequest::Move:
            {
                 k->executor->moveItem(response);
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
            case KTProjectRequest::Convert:
            {
                 k->executor->convertItem(response);
            }
            break;
            case KTProjectRequest::EditNodes:
            {
                 k->executor->setPathItem(response);
            }
            break;
            case KTProjectRequest::Select:
            {
            }
            break;
            case KTProjectRequest::View:
            {
            }
            break;
            case KTProjectRequest::Transform:
            {
                 k->executor->transformItem(response);
            }
            break;
            case KTProjectRequest::Group:
            {
                 k->executor->groupItems(response);
            }
            break;
            case KTProjectRequest::Ungroup:
            {
                 k->executor->ungroupItems(response);
            }
            break;
            case KTProjectRequest::SetTween:
            {
                 k->executor->setTween(response);
            }
            break;
            default: 
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::itemCommand() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::libraryCommand()
{
    #ifdef K_DEBUG
        T_FUNCINFO;
    #endif
    
    KTLibraryResponse *response = static_cast<KTLibraryResponse *>(k->response);

    switch (response->action()) {
            case KTProjectRequest::Add:
            {
                 k->executor->createSymbol(response);
            }
            break;

            case KTProjectRequest::Remove:
            {
                 k->executor->removeSymbol(response);
            }
            break;

            case KTProjectRequest::AddSymbolToProject:
            {
                 k->executor->addSymbolToProject(response);
            }
            break;

            case KTProjectRequest::RemoveSymbolFromProject:
            {
                 k->executor->removeSymbolFromProject(response);
            }
            break;

            default:
            {
                 #ifdef K_DEBUG
                        tError() << "KTProjectCommand::libraryCommand() - Error: Unknown project response";
                 #endif
            }
            break;
    }
}

void KTProjectCommand::paintAreaCommand()
{

    #ifdef K_DEBUG
           tError() << "KTProjectCommand::paintAreaCommand() - Error: FIX ME in ktprojectcommand.cpp";
    #endif

    /*
     if (redo)
         k->executor->reemitEvent(response);
     else
         k->executor->reemitEvent(response);
    */
}

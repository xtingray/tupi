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

#include "tupcommandexecutor.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupframe.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"
#include "tdebug.h"

// This method adds an object into the library

bool TupCommandExecutor::createSymbol(TupLibraryResponse *response)
{
    #ifdef K_DEBUG
           tFatal() << "TupCommandExecutor::createSymbol() - Creating object: " << response->arg().toString();
    #endif

    if (m_project->createSymbol(response->symbolType(), response->arg().toString(), 
                                response->data(), response->parent())) {
        emit responsed(response);

        return true;
    } 
    
    return false;
}

// This method removes an object from the library

bool TupCommandExecutor::removeSymbol(TupLibraryResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if ((response->sceneIndex() > -1) && (response->layerIndex() > -1) && (response->frameIndex() > -1)) {

        if (m_project->removeSymbol(response->arg().toString(), response->symbolType(), response->spaceMode(), 
            response->sceneIndex(), response->layerIndex(), response->frameIndex())) {
            emit responsed(response);
            return true;
        } 
    } else {
        if (m_project->removeSymbol(response->arg().toString())) {
            emit responsed(response);
            return true;
        }
    }
    
    return false;
}

// This method inserts an object into one frame 

bool TupCommandExecutor::insertSymbolIntoFrame(TupLibraryResponse *response)
{
    #ifdef K_DEBUG
           tFatal() << "TupCommandExecutor::insertSymbolIntoFrame() - Adding symbol to project: " << response->arg().toString();
    #endif


    if (m_project->scenesTotal() > 0) {
        if (m_project->insertSymbolIntoFrame(response->spaceMode(), response->arg().toString(), 
            response->sceneIndex(), response->layerIndex(), response->frameIndex())) {
            TupScene *scene = m_project->scene(response->sceneIndex()); 
            if (scene) {
                TupLayer *layer = scene->layer(response->layerIndex());
                if (layer) {
                    TupFrame *frame = layer->frame(response->frameIndex());
                    if (frame)
                        response->setFrameState(frame->isEmpty());
                }
            }
            emit responsed(response);
            return true;
        } 
    } else {
        #ifdef K_DEBUG
               tError() << "TupCommandExecutor::insertSymbolIntoFrame() - No scenes available!";
        #endif
    } 

    return false;
}

// This method removes an object from one frame

bool TupCommandExecutor::removeSymbolFromFrame(TupLibraryResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (m_project->scenesTotal() > 0) {
        if (m_project->removeSymbolFromFrame(response->arg().toString(), response->symbolType())) {
            TupScene *scene = m_project->scene(response->sceneIndex());
            if (scene) {
                TupLayer *layer = scene->layer(response->layerIndex());
                if (layer) {
                    TupFrame *frame = layer->frame(response->frameIndex());
                    if (frame) 
                        response->setFrameState(frame->isEmpty());
                }
            }
            emit responsed(response);
            return true;
        } else {
            #ifdef K_DEBUG
                   tError() << "TupCommandExecutor::removeSymbolFromFrame() - Error: Symbol can't be removed from project!";
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupCommandExecutor::removeSymbolFromFrame() - No scenes available!";
        #endif
    }

    return false;
}

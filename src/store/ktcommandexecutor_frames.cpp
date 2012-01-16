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

#include "ktcommandexecutor.h"
#include "ktscene.h"
#include "ktprojectrequest.h"
#include "ktrequestbuilder.h"
#include "ktlayer.h"
#include "ktframe.h"
#include "ktprojectresponse.h"

#include "tdebug.h"

bool KTCommandExecutor::createFrame(KTFrameResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int position = response->frameIndex();
    QString name = response->arg().toString();

    // QString state = response->state();
    
    KTScene *scene = m_project->scene(scenePosition);
    
    if (!scene) 
        return false;
    
    KTLayer *layer = scene->layer(layerPosition);
    
    if (layer) {
        KTFrame *frame = layer->createFrame(name, position);
        
        if (!frame)
            return false;
       
        /* 
        if (!name.isEmpty()) {
            #ifdef K_DEBUG
                tDebug("items") << name;
            #endif
            frame->setFrameName(name);
        } else {
            response->setArg(frame->frameName());
        }
        */

        response->setArg(frame->frameName());
        response->setFrameIndex(layer->visualIndexOf(frame));

        emit responsed(response);

        /* SQA: Check if this code is really necessary
        if (!state.isEmpty()) {
            frame->fromXml(state);
            response->setArg(frame->frameName());
        }
        */

        return true;
    }
    
    return false;
}

bool KTCommandExecutor::removeFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    int realPosition = response->arg().toInt();

    KTScene *scene = m_project->scene(scenePos);
    
    if (scene) {
        KTLayer *layer = scene->layer(layerPos);
        if (layer) {
            KTFrame *frame = layer->frame(position);
            if (frame) {
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                response->setArg(frame->frameName());

                scene->removeTweensFromFrame(realPosition);
                
                if (layer->removeFrame(position)) {
                    response->setState(doc.toString(0));
                    emit responsed(response);
                    
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool KTCommandExecutor::resetFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
   
    KTScene *scene = m_project->scene(scenePos);
   
    if (scene) {
        KTLayer *layer = scene->layer(layerPos);
        if (layer) {
            KTFrame *frame = layer->frame(position);
            if (frame) {
                /*
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                response->setArg(frame->frameName());
                */
                if (layer->resetFrame(position)) {
                    //response->setState(doc.toString(0));
                    emit responsed(response);

                    return true;
                }
            }
        }
    }
   
    return false;
}

bool KTCommandExecutor::moveFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    int newPosition = response->arg().toInt();
    KTScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {
        if (layer->moveFrame(position, newPosition)) {
            emit responsed(response);
            return true;
        } else {
            #ifdef K_DEBUG
                   tWarning() << "KTCommandExecutor::moveFrame() - Failed moving frame";
            #endif
            return false;
        }
    }
    
    return false;
}

bool KTCommandExecutor::exchangeFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    int newPosition = response->arg().toInt();
    KTScene *scene = m_project->scene(scenePos);
   
    if (!scene)
        return false;
   
    KTLayer *layer = scene->layer(layerPos);
   
    if (layer) {
        if (layer->exchangeFrame(position, newPosition)) {
            emit responsed(response);
            return true;
        } else {
            #ifdef K_DEBUG
                   tWarning() << "Failed moving frame";
            #endif
            return false;
        }
    }
   
    return false;
}

bool KTCommandExecutor::lockFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    bool lock = response->arg().toBool();
    
    KTScene *scene = m_project->scene(scenePos);
    
    if (!scene)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {
        KTFrame *frame = layer->frame(position);
        
        if (! frame) 
            return false;
        
        frame->setLocked(lock);
        emit responsed(response);

        return true;
    }
    
    return false;
}


bool KTCommandExecutor::renameFrame(KTFrameResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    QString newName= response->arg().toString();

    QString oldName;
    
    KTScene *scene = m_project->scene(scenePos);
    
    if (!scene)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {
        KTFrame *frame = layer->frame(position);
        
        if (! frame) 
            return false;
        
        oldName = frame->frameName();

        if (oldName.compare(newName) != 0)
            frame->setFrameName(newName);
        
        emit responsed(response);

        return true;
    }
    
    return false;
}


bool KTCommandExecutor::selectFrame(KTFrameResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    // bool prioritary = response->arg().toBool();

    KTScene *scene = m_project->scene(scenePos);

    if (!scene || scenePos < 0 || position < 0)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {

        if (position < layer->framesTotal()) {
            KTFrame *frame = layer->frame(position);
        
            if (! frame) 
                return false;
        
            emit responsed(response);
            return true;
        } 
    } 
    
    return false;
}

bool KTCommandExecutor::setFrameVisibility(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    bool view = response->arg().toBool();
    
    KTScene *scene = m_project->scene(scenePos);
    
    if (!scene)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {
        KTFrame *frame = layer->frame(position);
        
        if (! frame) 
            return false;
        
        frame->setVisible(view);
        emit responsed(response);

        return true;
    }
    
    return false;
}

bool KTCommandExecutor::expandFrame(KTFrameResponse *response)
{
    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    int times = response->arg().toInt();
    
    KTScene *scene = m_project->scene(scenePos);
    
    if (!scene)
        return false;
    
    KTLayer *layer = scene->layer(layerPos);
    
    if (layer) {
        if (layer->expandFrame(position, times)) {
            emit responsed(response);

            return true;
        }
    }

    return false;
}

bool KTCommandExecutor::pasteFrame(KTFrameResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scenePos = response->sceneIndex();
    int layerPos = response->layerIndex();
    int position = response->frameIndex();
    QString copyFrame = response->arg().toString();

    KTScene *scene = m_project->scene(scenePos);
    if (scene) {
        KTLayer *layer = scene->layer(layerPos);
        if (layer) {
            KTFrame *frame = layer->frame(position);
            if (frame) {
                QString name = frame->frameName();
                
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                response->setArg(doc.toString(0));
                frame->clear();
                frame->fromXml(copyFrame);
                frame->setFrameName(name);

                response->setFrameState(frame->isEmpty());
                emit responsed(response);

                return true;
            }
        }
    }

    return false;
}

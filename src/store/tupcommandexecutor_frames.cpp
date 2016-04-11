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
#include "tupscene.h"
#include "tupprojectrequest.h"
#include "tuprequestbuilder.h"
#include "tuplayer.h"
#include "tupframe.h"
#include "tupprojectresponse.h"

bool TupCommandExecutor::createFrame(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::createFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    QString name = response->arg().toString();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->insertStoryBoardScene(position);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (response->mode() == TupProjectResponse::Do) {
                TupFrame *frame = layer->createFrame(name, position);
                if (frame) {
                    emit responsed(response);
                    return true;
                }
            }

            if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                bool success = layer->restoreFrame(position);
                if (success) {
                    emit responsed(response);
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::removeFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    int realPosition = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->removeStoryBoardScene(position);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) {
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                scene->removeTweensFromFrame(layerIndex, realPosition);
                
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

bool TupCommandExecutor::resetFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
   
    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->resetStoryBoardScene(position);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) {
                if (layer->resetFrame(position)) {
                    frame->updateRenderStatus(true);
                    emit responsed(response);
                    return true;
                }
            }
        }
    }
   
    return false;
}

bool TupCommandExecutor::moveFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    int newPosition = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->moveStoryBoardScene(position, newPosition); 
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->moveFrame(position, newPosition)) {
                emit responsed(response);
                return true;
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::moveFrame() - Fatal error while moving frame!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif  
                return false;
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::exchangeFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    int newPosition = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->moveStoryBoardScene(position, newPosition);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->exchangeFrame(position, newPosition)) {
                emit responsed(response);
                return true;
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::exchangeFrame() - Error while exchanging frames";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif  
                return false;
            }
        }
    }
   
    return false;
}

bool TupCommandExecutor::lockFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    bool lock = response->arg().toBool();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) {
                frame->setLocked(lock);
                emit responsed(response);
                return true;
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::renameFrame(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::renameFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif	

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    QString newName = response->arg().toString();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) { 
                QString oldName = frame->frameName();
                if (oldName.compare(newName) != 0)
                    frame->setFrameName(newName);
        
                emit responsed(response);
                return true;
            }
        }
    }
    
    return false;
}


bool TupCommandExecutor::selectFrame(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::selectFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif	
	
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();

    if (sceneIndex < 0 || position < 0)
        return false;

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (position < layer->framesCount()) {
                TupFrame *frame = layer->frameAt(position);
                if (frame) {
                    emit responsed(response);
                    return true;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::selectFrame() - Invalid frame index -> " + QString::number(position);
                        #ifdef Q_OS_WIN
                           qDebug() << msg;
                        #else
                           tError() << msg;
                        #endif
                    #endif
                    return false;
                }
            }
        }
    } 
    
    return false;
}

bool TupCommandExecutor::setFrameVisibility(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    bool view = response->arg().toBool();
    TupScene *scene = m_project->sceneAt(sceneIndex);

    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) { 
                frame->setVisible(view);
                emit responsed(response);

                return true;
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::expandFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    int times = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->expandFrame(position, times)) {
                emit responsed(response);
                return true;
            }
        }
    }

    return false;
}

bool TupCommandExecutor::pasteFrame(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::pasteFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif	
	
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int position = response->frameIndex();
    QString copyFrame = response->arg().toString();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(position);
            if (frame) {
                QString name = frame->frameName();
                
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                response->setArg(doc.toString(0));
                frame->clear();
                frame->fromXml(copyFrame);
                frame->setFrameName(name);

                frame->updateRenderStatus(true);
                response->setFrameState(frame->isEmpty());
                emit responsed(response);

                return true;
            }
        }
    }

    return false;
}

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
    int pos = response->frameIndex();
    QString name = response->arg().toString();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->insertStoryBoardScene(pos);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (response->mode() == TupProjectResponse::Do) {
                TupFrame *frame = layer->createFrame(name, pos);
                if (frame) {
                    emit responsed(response);
                    return true;
                }
            }

            if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) {
                bool success = layer->restoreFrame(pos);
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::removeFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int pos = response->frameIndex();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->removeStoryBoardScene(pos);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (pos >= layer->framesCount())
                pos = layer->framesCount() - 1;

            TupFrame *frame = layer->frameAt(pos);
            if (frame) {
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                scene->removeTweensFromFrame(layerIndex, pos);

                if (layer->removeFrame(pos)) {
                    response->setState(doc.toString(0));
                    emit responsed(response);

                    return true;
                }
            }
        }
    }

    return false;
}

bool TupCommandExecutor::restoreFrameSelection(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::restoreFrameSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int frameIndex = response->frameIndex();
    QString selection = response->arg().toString();

    QStringList params = selection.split(",");
    int layers = params.at(0).toInt(); 
    int frames = params.at(1).toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        int layersTotal = layerIndex + layers;
        for (int i=layerIndex; i<layersTotal; i++) {
            TupLayer *layer = scene->layerAt(i);
            if (layer) {
                int framesTotal = frameIndex + frames;
                for (int j=frameIndex; j<framesTotal; j++) {
                    bool success = layer->restoreFrame(frameIndex);
                    if (!success) {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::restoreFrameSelection() "
                                          "- Fatal Error: Can't restore frame at index: " 
                                          + QString::number(frameIndex);
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
    }

    emit responsed(response);
    return true;
}

bool TupCommandExecutor::removeFrameSelection(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::removeFrameSelection()]";
        #else
            T_FUNCINFO;
            tWarning() << "selection: " << response->arg().toString(); 
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int frameIndex = response->frameIndex();
    QString selection = response->arg().toString();

    QStringList params = selection.split(",");
    int layers = params.at(0).toInt();  
    int frames = params.at(1).toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        int layersTotal = layerIndex + layers;
        for (int i=layerIndex; i<layersTotal; i++) {
            TupLayer *layer = scene->layerAt(i);
            if (layer) {
                int framesTotal = frameIndex + frames;
                for (int j=frameIndex; j<framesTotal; j++) {
                     if (frameIndex < layer->framesCount()) {
                         TupFrame *frame = layer->frameAt(frameIndex);
                         if (frame) {
                             scene->removeStoryBoardScene(frameIndex);
                             scene->removeTweensFromFrame(i, frameIndex);
                             if (layer->framesCount() == 1) {
                                 if (!layer->resetFrame(frameIndex))
                                     return false; 
                             } else {
                                 if (!layer->removeFrame(frameIndex)) {
                                     #ifdef K_DEBUG
                                         QString msg = "TupCommandExecutor::removeFrameSelection() - "
                                                       "Fatal Error: Can't remove frame at index: " 
                                                       + QString::number(frameIndex);
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
                }
            }
        }
    }
    emit responsed(response);

    return true; 
}

bool TupCommandExecutor::resetFrame(TupFrameResponse *response)
{
    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int pos = response->frameIndex();
   
    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->resetStoryBoardScene(pos);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(pos);
            if (frame) {
                if (layer->resetFrame(pos)) {
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
    int pos = response->frameIndex();
    int newPosition = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->moveStoryBoardScene(pos, newPosition); 
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->moveFrame(pos, newPosition)) {
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
    int pos = response->frameIndex();
    int newPosition = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        scene->moveStoryBoardScene(pos, newPosition);
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->exchangeFrame(pos, newPosition)) {
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
    int pos = response->frameIndex();
    bool lock = response->arg().toBool();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(pos);
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
    int pos = response->frameIndex();
    QString newName = response->arg().toString();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(pos);
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
    int pos = response->frameIndex();

    if (sceneIndex < 0 || pos < 0)
        return false;

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (pos < layer->framesCount()) {
                TupFrame *frame = layer->frameAt(pos);
                if (frame) {
                    emit responsed(response);
                    return true;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::selectFrame()"
                                      " - Invalid frame index -> " 
                                      + QString::number(pos);
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
    int pos = response->frameIndex();
    bool view = response->arg().toBool();
    TupScene *scene = m_project->sceneAt(sceneIndex);

    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            TupFrame *frame = layer->frameAt(pos);
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
    int pos = response->frameIndex();
    int times = response->arg().toInt();

    TupScene *scene = m_project->sceneAt(sceneIndex);
    if (scene) {
        TupLayer *layer = scene->layerAt(layerIndex);
        if (layer) {
            if (layer->expandFrame(pos, times)) {
                emit responsed(response);
                return true;
            }
        }
    }

    return false;
}

bool TupCommandExecutor::copyFrameSelection(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::copyFrameSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    selectionFramesCopy.clear();
    int sceneIndex = response->sceneIndex();
    // int layerIndex = response->layerIndex();
    // int pos = response->frameIndex();
    QString selection = response->arg().toString();
    QStringList params = selection.split(",");

    if (params.count() == 4) {
        copyParams = params;
        int initLayer = params.at(0).toInt();
        int endLayer = params.at(1).toInt();
        int initFrame = params.at(2).toInt();
        int endFrame = params.at(3).toInt();

        TupScene *scene = m_project->sceneAt(sceneIndex);
        if (scene) {
            for (int i=initLayer; i<=endLayer; i++) {
                 TupLayer *layer = scene->layerAt(i);
                 if (layer) {
                     for (int j=initFrame; j<=endFrame; j++) {
                         TupFrame *frame = layer->frameAt(j);
                         if (frame) {
                             QDomDocument doc;
                             doc.appendChild(frame->toXml(doc));
                             selectionFramesCopy << doc.toString();
                         } else {
                             return false;
                         }
                     }
                 } else {
                     return false;
                 }
            }

            emit responsed(response);
            return true;
        }
    }

    return false;
}

bool TupCommandExecutor::pasteFrameSelection(TupFrameResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::pasteFrameSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int sceneIndex = response->sceneIndex();
    int layerIndex = response->layerIndex();
    int pos = response->frameIndex();

    if (copyParams.count() == 4) {
        int layerLimit = layerIndex + (copyParams.at(1).toInt() - copyParams.at(0).toInt()) + 1;
        int frameLimit = pos + (copyParams.at(3).toInt() - copyParams.at(2).toInt()) + 1;

        TupScene *scene = m_project->sceneAt(sceneIndex);
        if (scene) {
            if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
                int index = 0;
                for (int i=layerIndex; i<layerLimit; i++) {
                    TupLayer *layer = scene->layerAt(i);
                    if (layer) {
                        for (int j=pos; j<frameLimit; j++) {
                            TupFrame *frame = new TupFrame(layer);
                            frame->fromXml(selectionFramesCopy.at(index));
                            layer->setFrame(j, frame);
                            index++;
                        }
                    } else {
                        return false;
                    }
                }
            } else {
                for (int i=layerIndex; i<layerLimit; i++) {
                    TupLayer *layer = scene->layerAt(i);
                    if (layer) {
                        for (int j=pos; j<frameLimit; j++) {
                            if (pos < layer->framesCount()) {
                                TupFrame *frame = layer->frameAt(pos);
                                if (frame) {
                                    // SQA: Check if these instructions are required in this context 
                                    scene->removeStoryBoardScene(pos);
                                    scene->removeTweensFromFrame(i, pos);

                                    if (layer->framesCount() == 1) {
                                        if (!layer->resetFrame(pos))
                                            return false;
                                    } else {
                                        if (!layer->removeFrame(pos)) {
                                            #ifdef K_DEBUG
                                                QString msg = "TupCommandExecutor::pasteFrameSelection() - "
                                                              "Fatal Error: Can't remove frame at index: "
                                                              + QString::number(pos);
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
                        }
                    }
                }
            }

            response->setArg(copyParams.at(0) + "," + copyParams.at(1) + "," 
                             + copyParams.at(2) + "," + copyParams.at(3));
            emit responsed(response);
            return true;
        }
    }

    return false;
}

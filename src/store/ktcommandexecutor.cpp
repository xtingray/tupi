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

#include <ktproject.h>
#include <ktscene.h>

#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"

#include "ktprojectresponse.h"

#include "kdebug.h"

KTCommandExecutor::KTCommandExecutor(KTProject *project) : QObject(project), m_project(project)
{
}

KTCommandExecutor::~KTCommandExecutor()
{
}

void KTCommandExecutor::getScenes(KTSceneResponse *response)
{
    response->setScenes(m_project->scenes());
    emit responsed(response);
}

bool KTCommandExecutor::createScene(KTSceneResponse *response)
{
    #ifdef K_DEBUG
        K_FUNCINFO;
    #endif
    
    int position = response->sceneIndex();
    QString name = response->arg().toString();
    
    if (position < 0 || position > m_project->scenes().count())
        return false;
    
    KTScene *scene = m_project->createScene(name, position);
    if (!scene) 
        return false;
   
    /* 
    if (!name.isEmpty())
        scene->setSceneName(name);
    else
        response->setArg(scene->sceneName());
    */
    
    emit responsed(response);
   
    /* SQA: Check if this code is really necessary 
    QString state =  response->state();
    
    if (! state.isEmpty()) {
        scene->fromXml(state);
        response->setArg(scene->sceneName());
    }
    */
    
    return true;
}

bool KTCommandExecutor::removeScene(KTSceneResponse *response)
{
    #ifdef K_DEBUG
        K_FUNCINFO;
    #endif    

    int position = response->sceneIndex();
    
    KTScene *toRemove = m_project->scene(position);
    
    if (toRemove) {
        QDomDocument document;
        document.appendChild(toRemove->toXml(document));
        
        response->setState(document.toString());
        response->setArg(toRemove->sceneName());
        
        if (m_project->removeScene(position)) {
            emit responsed(response);
            return true;
        }
    }
    
    return false;
}

bool KTCommandExecutor::moveScene(KTSceneResponse *response)
{
    int position = response->sceneIndex();
    int newPosition = response->arg().toInt();
    if (m_project->moveScene(position, newPosition)) {
        emit responsed(response);
        return true;
    }
    
    return false;
}

bool KTCommandExecutor::lockScene(KTSceneResponse *response)
{
    int position = response->sceneIndex();
    bool lock = response->arg().toBool();

    #ifdef K_DEBUG
        kWarning() << "Lock scene: " << lock;
    #endif    

    KTScene *scene = m_project->scene(position);
    
    if (!scene)
        return false;
    
    scene->setLocked(lock);
    
    emit responsed(response);

    return true;
}

bool KTCommandExecutor::renameScene(KTSceneResponse *response)
{
    int position = response->sceneIndex();
    QString newName = response->arg().toString();
    KTScene *scene = m_project->scene(position);

    kFatal() << "KTCommandExecutor::renameScene() - New name: " << newName;
    
    if (!scene)
        return false;
    
    // KTProjectRequest request = KTRequestBuilder::createSceneRequest(position, KTProjectRequest::Rename, newName);
    
    scene->setSceneName(newName);
    
    emit responsed(response);

    return true;
}

void KTCommandExecutor::selectScene(KTSceneResponse *response)
{
    emit responsed(response);
}

bool KTCommandExecutor::setSceneVisibility(KTSceneResponse *response)
{
    int position = response->sceneIndex();
    bool view = response->arg().toBool();
    
    KTScene *scene = m_project->scene(position);
    
    if (!scene)
        return false;
    
    scene->setVisible(view);
    
    emit responsed(response);
    
    return true;
}

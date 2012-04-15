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

#include "tupcommandexecutor.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"
#include "tdebug.h"

TupCommandExecutor::TupCommandExecutor(TupProject *project) : QObject(project), m_project(project)
{
}

TupCommandExecutor::~TupCommandExecutor()
{
}

void TupCommandExecutor::getScenes(TupSceneResponse *response)
{
    response->setScenes(m_project->scenes());
    emit responsed(response);
}

bool TupCommandExecutor::createScene(TupSceneResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int position = response->sceneIndex();
    QString name = response->arg().toString();
    
    // if (position < 0 || position > m_project->scenes().count())
    if (position < 0)
        return false;
    
    TupScene *scene = m_project->createScene(name, position);
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

bool TupCommandExecutor::removeScene(TupSceneResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif    

    int position = response->sceneIndex();
    int scenesTotal = m_project->scenesTotal();

    TupScene *toRemove = m_project->scene(position);

    if (toRemove) {
        QDomDocument document;
        document.appendChild(toRemove->toXml(document));
        
        response->setState(document.toString());
        response->setArg(toRemove->sceneName());
        
        if (m_project->removeScene(position)) {

            if (position+1 < scenesTotal) {
                for (int i = position + 1; i < scenesTotal; i++)
                     m_project->moveScene(i, i-1);
            }

            emit responsed(response);

            return true;
        } 
    }
    
    return false;
}

bool TupCommandExecutor::moveScene(TupSceneResponse *response)
{
    int position = response->sceneIndex();
    int newPosition = response->arg().toInt();
    if (m_project->moveScene(position, newPosition)) {
        emit responsed(response);
        return true;
    }
    
    return false;
}

bool TupCommandExecutor::lockScene(TupSceneResponse *response)
{
    int position = response->sceneIndex();
    bool lock = response->arg().toBool();

    #ifdef K_DEBUG
        tWarning() << "Lock scene: " << lock;
    #endif    

    TupScene *scene = m_project->scene(position);
    
    if (!scene)
        return false;
    
    scene->setLocked(lock);
    
    emit responsed(response);

    return true;
}

bool TupCommandExecutor::renameScene(TupSceneResponse *response)
{
    int position = response->sceneIndex();
    QString newName = response->arg().toString();
    TupScene *scene = m_project->scene(position);

    if (!scene)
        return false;
    
    // TupProjectRequest request = TupRequestBuilder::createSceneRequest(position, TupProjectRequest::Rename, newName);
    
    scene->setSceneName(newName);
    
    emit responsed(response);

    return true;
}

void TupCommandExecutor::selectScene(TupSceneResponse *response)
{
    emit responsed(response);
}

bool TupCommandExecutor::setSceneVisibility(TupSceneResponse *response)
{
    int position = response->sceneIndex();
    bool view = response->arg().toBool();
    
    TupScene *scene = m_project->scene(position);
    
    if (!scene)
        return false;
    
    scene->setVisible(view);
    
    emit responsed(response);
    
    return true;
}

bool TupCommandExecutor::resetScene(TupSceneResponse *response)
{
    int position = response->sceneIndex();
    QString name = response->arg().toString();
    TupScene *scene = m_project->scene(position);

    if (!scene)
        return false;

    scene->reset(name);

    emit responsed(response);

    return true;
}

void TupCommandExecutor::setBgColor(TupSceneResponse *response)
{
    QString colorName = response->arg().toString();
    m_project->setBgColor(QColor(colorName));

    emit responsed(response);
}

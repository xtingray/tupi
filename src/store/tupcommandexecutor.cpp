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
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"

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
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::createScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int position = response->sceneIndex();
    QString name = response->arg().toString();
    if (position < 0)
        return false;

    if (response->mode() == TupProjectResponse::Do) {
        TupScene *scene = m_project->createScene(name, position);
        if (!scene) 
            return false;
    }

    if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) { 
        bool success = m_project->restoreScene(position);
        if (!success)
            return false;
    }

    emit responsed(response);
    return true;
}

bool TupCommandExecutor::removeScene(TupSceneResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::removeScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif	

    int position = response->sceneIndex();
    // int scenesCount = m_project->scenesCount();

    TupScene *toRemove = m_project->scene(position);

    if (toRemove) {
        QDomDocument document;
        document.appendChild(toRemove->toXml(document));
        response->setState(document.toString());
        response->setArg(toRemove->sceneName());
        
        if (m_project->removeScene(position)) {
            emit responsed(response);
            return true;
        } 
    } else {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::removeScene() - Scene index doesn't exist -> " + QString::number(position);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError("library") << msg;
            #endif
        #endif
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
        QString msg = "TupCommandExecutor::lockScene() - Scene is locked: " + QString::number(lock);
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("library") << msg;
        #endif
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

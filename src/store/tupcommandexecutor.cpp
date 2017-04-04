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

    int pos = response->sceneIndex();
    QString name = response->arg().toString();
    if (pos < 0)
        return false;

    if (response->mode() == TupProjectResponse::Do) {
        TupScene *scene = m_project->createScene(name, pos);
        if (!scene) 
            return false;
    }

    if (response->mode() == TupProjectResponse::Redo || response->mode() == TupProjectResponse::Undo) { 
        bool success = m_project->restoreScene(pos);
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

    int pos = response->sceneIndex();
    TupScene *scene = m_project->sceneAt(pos);
    if (scene) {
        QDomDocument document;
        document.appendChild(scene->toXml(document));
        response->setState(document.toString());
        response->setArg(scene->sceneName());
        
        if (m_project->removeScene(pos)) {
            emit responsed(response);
            return true;
        } 
    } else {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::removeScene() - Fatal Error: No scene at index -> " + QString::number(pos);
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
    int pos = response->sceneIndex();
    int newPos = response->arg().toInt();
    if (m_project->moveScene(pos, newPos)) {
        emit responsed(response);
        return true;
    }
    
    return false;
}

bool TupCommandExecutor::lockScene(TupSceneResponse *response)
{
    int pos = response->sceneIndex();
    bool lock = response->arg().toBool();

    #ifdef K_DEBUG
        QString msg = "TupCommandExecutor::lockScene() - Scene is locked: " + QString::number(lock);
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("library") << msg;
        #endif
    #endif  

    TupScene *scene = m_project->sceneAt(pos);
    if (scene) {
        scene->setLocked(lock);
        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::renameScene(TupSceneResponse *response)
{
    int pos = response->sceneIndex();
    QString newName = response->arg().toString();

    TupScene *scene = m_project->sceneAt(pos);
    if (scene) {
        scene->setSceneName(newName);
        emit responsed(response);
        return true;
    }

    return false;
}

void TupCommandExecutor::selectScene(TupSceneResponse *response)
{
    emit responsed(response);
}

bool TupCommandExecutor::setSceneVisibility(TupSceneResponse *response)
{
    int pos = response->sceneIndex();
    bool view = response->arg().toBool();
    
    TupScene *scene = m_project->sceneAt(pos);
    if (scene) {
        scene->setVisible(view);
        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::resetScene(TupSceneResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCommandExecutor::resetScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int pos = response->sceneIndex();
    QString newName = response->arg().toString();

    TupScene *scene = m_project->sceneAt(pos);
    if (scene) {
        if (response->mode() == TupProjectResponse::Do || response->mode() == TupProjectResponse::Redo) {
            if (m_project->resetScene(pos, newName)) {
                emit responsed(response);
                return true;
            }
        }

        if (response->mode() == TupProjectResponse::Undo) {
            QString oldName = m_project->recoverScene(pos);
            response->setArg(oldName);
            emit responsed(response);
            return true;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::resetScene() - Fatal Error: No scene at index -> " + QString::number(pos);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return false;
}

void TupCommandExecutor::setBgColor(TupSceneResponse *response)
{
    QString colorName = response->arg().toString();
    m_project->setBgColor(QColor(colorName));

    emit responsed(response);
}

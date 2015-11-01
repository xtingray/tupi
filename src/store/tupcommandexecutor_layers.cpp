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
#include "tuplayer.h"
#include "tupprojectrequest.h"
#include "tuprequestbuilder.h"
#include "tupprojectresponse.h"

bool TupCommandExecutor::createLayer(TupLayerResponse *response)
{	
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::createLayer()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int position = response->layerIndex();

    QString name = response->arg().toString();
    QString state = response->state();

    TupScene *scene = m_project->scene(scenePosition);

    if (scene) {
        TupLayer *layer = scene->createLayer(name, position);

        if (! layer) 
            return false;

        /*
        if (!name.isEmpty())
            layer->setLayerName(name);
        else
            response->setArg(layer->layerName());
        */

        layer->setLayerName(name);

        // response->setArg(layer->layerName());
        // layer->fromXml(state);
        // m_project->updateScene(scenePosition, scene);

        emit responsed(response);

        return true;
    }

    return false;
}

bool TupCommandExecutor::removeLayer(TupLayerResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::removeLayer()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
		
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();

    TupScene *scene = m_project->scene(scenePos);

    if (scene) {
        TupLayer *layer = scene->layer(position);
        if (layer) {
            QDomDocument document;
            document.appendChild(layer->toXml(document));
            response->setState(document.toString());
            response->setArg(layer->layerName());

            if (scene->removeLayer(position)) {
                emit responsed(response);
                return true;
            } 
        } 
    }

    return false;
}

bool TupCommandExecutor::moveLayer(TupLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    int newPosition = response->arg().toInt();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    if (! scene->moveLayer(position, newPosition)) {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::moveLayer() - Error while moving layer!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif	
        return false;
    } else {
        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::lockLayer(TupLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    bool lock = response->arg().toBool();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    TupLayer *layer = scene->layer(position);

    if (layer) {
        layer->setLocked(lock);
        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::renameLayer(TupLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    QString newName = response->arg().toString();
	
    #ifdef K_DEBUG
        QString msg = "TupCommandExecutor::renameLayer() - Renaming layer to: " + newName;
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif	

    // QString oldName;

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    TupLayer *layer = scene->layer(position);

    if (layer) {
        QString current = layer->layerName();
        layer->setLayerName(newName);

        emit responsed(response);
        response->setArg(current);

        return true;
    }

    return false;
}

bool TupCommandExecutor::selectLayer(TupLayerResponse *response)
{
    emit responsed(response);

    return true;
}

bool TupCommandExecutor::setLayerVisibility(TupLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    bool view = response->arg().toBool();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    TupLayer *layer = scene->layer(position);

    if (layer) {
        layer->setVisible(view);
        responsed(response);

        return true;
    }

    return false;
}

bool TupCommandExecutor::addLipSync(TupLayerResponse *response)
{
    #ifdef K_DEBUG
        QString msg = "TupCommandExecutor::addLipSync() - Adding lipsync...";
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    QString xml = response->arg().toString();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    TupLayer *layer = scene->layer(position);

    if (layer) {
        TupLipSync *lipsync = new TupLipSync();
        lipsync->fromXml(xml);
        layer->addLipSync(lipsync);

        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::updateLipSync(TupLayerResponse *response)
{
    #ifdef K_DEBUG
        QString msg = "TupCommandExecutor::updateLipSync() - Updating lipsync...";
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    int scenePos = response->sceneIndex();
    QString xml = response->arg().toString();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    TupLipSync *lipsync = new TupLipSync();
    lipsync->fromXml(xml);
    if (scene->updateLipSync(lipsync)) {
        emit responsed(response);
        return true;
    }

    return false;
}

bool TupCommandExecutor::removeLipSync(TupLayerResponse *response)
{
    #ifdef K_DEBUG
        QString msg = "TupCommandExecutor::removeLipSync() - Adding lipsync...";
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    int scenePos = response->sceneIndex();
    QString name = response->arg().toString();

    TupScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    if (scene->removeLipSync(name)) {
        emit responsed(response);
        return true;
    }

    return false;
}

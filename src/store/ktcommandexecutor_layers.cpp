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
#include "ktlayer.h"

#include "ktprojectrequest.h"
#include "ktrequestbuilder.h"
#include "ktprojectresponse.h"

#include "tdebug.h"

bool KTCommandExecutor::createLayer(KTLayerResponse *response)
{
    #ifdef K_DEBUG
        T_FUNCINFO;
    #endif

    int scenePosition = response->sceneIndex();
    int position = response->layerIndex();

    QString name = response->arg().toString();
    QString state = response->state();

    KTScene *scene = m_project->scene(scenePosition);

    if (scene) {
        KTLayer *layer = scene->createLayer(name, position);

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

bool KTCommandExecutor::removeLayer(KTLayerResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int scenePos = response->sceneIndex();
    int position = response->layerIndex();

    KTScene *scene = m_project->scene(scenePos);

    if (scene) {
        KTLayer *layer = scene->layer(position);
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

bool KTCommandExecutor::moveLayer(KTLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    int newPosition = response->arg().toInt();

    KTScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    if (! scene->moveLayer(position, newPosition)) {
        #ifdef K_DEBUG
               tWarning() << "Failed moving layer";
        #endif
        return false;
    } else {
        emit responsed(response);
        return true;
    }

    return false;
}

bool KTCommandExecutor::lockLayer(KTLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    bool lock = response->arg().toBool();

    KTScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    KTLayer *layer = scene->layer(position);

    if (layer) {
        layer->setLocked(lock);
        emit responsed(response);
        return true;
    }

    return false;
}

bool KTCommandExecutor::renameLayer(KTLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    QString newName = response->arg().toString();

    #ifdef K_DEBUG
           tWarning() << "Renamed layer: " << newName;
    #endif

    QString oldName;

    KTScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    KTLayer *layer = scene->layer(position);

    if (layer) {
        QString current = layer->layerName();
        layer->setLayerName(newName);

        emit responsed(response);
        response->setArg(current);

        return true;
    }

    return false;
}

bool KTCommandExecutor::selectLayer(KTLayerResponse *response)
{
    emit responsed(response);

    return true;
}


bool KTCommandExecutor::setLayerVisibility(KTLayerResponse *response)
{
    int scenePos = response->sceneIndex();
    int position = response->layerIndex();
    bool view = response->arg().toBool();

    KTScene *scene = m_project->scene(scenePos);

    if (!scene)
        return false;

    KTLayer *layer = scene->layer(position);

    if (layer) {
        layer->setVisible(view);
        responsed(response);

        return true;
    }

    return false;
}

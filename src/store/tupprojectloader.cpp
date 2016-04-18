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

#include "tupprojectloader.h"
#include "tupproject.h"
#include "tupprojectresponse.h"
#include "tuplibraryobject.h"

TupProjectLoader::TupProjectLoader()
{
}

TupProjectLoader::~TupProjectLoader()
{
}

void TupProjectLoader::createItem(int scenePosition, int layerPosition, int framePosition, int itemPosition, QPointF point, TupLibraryObject::Type type, const QString &xml, TupProject *project)
{
    TupItemResponse response(TupProjectRequest::Item, TupProjectRequest::Add);

    response.setMode(TupProjectResponse::Do);
    response.setSceneIndex(scenePosition);
    response.setLayerIndex(layerPosition);
    response.setFrameIndex(framePosition);
    response.setItemIndex(itemPosition);
    response.setItemType(type);
    response.setPosX(point.x());
    response.setPosY(point.y());
    response.setArg(xml);
    
    project->emitResponse(&response);
}

void TupProjectLoader::createFrame(int scenePosition, int layerPosition, int framePosition, const QString &name, TupProject *project)
{
    TupFrameResponse response(TupProjectRequest::Frame, TupProjectRequest::Add);

    response.setMode(TupProjectResponse::Do);
    response.setSceneIndex(scenePosition);
    response.setLayerIndex(layerPosition);
    response.setFrameIndex(framePosition);
    response.setArg(name);

    project->emitResponse(&response);
}

void TupProjectLoader::createLayer(int scenePosition, int layerPosition, const QString &name, TupProject *project)
{
    TupLayerResponse response(TupProjectRequest::Layer, TupProjectRequest::Add);
   
    response.setMode(TupProjectResponse::Do); 
    response.setSceneIndex(scenePosition);
    response.setLayerIndex(layerPosition);
    response.setArg(name);
    
    project->emitResponse(&response);
}

void TupProjectLoader::createSoundLayer(int scenePosition, int layerPosition, const QString &name, TupProject *project)
{
    TupLibraryResponse response(TupProjectRequest::Library, TupProjectRequest::InsertSymbolIntoFrame);
   
    response.setMode(TupProjectResponse::Do); 
    response.setSceneIndex(scenePosition);
    response.setLayerIndex(layerPosition);
    response.setArg(name);
    response.setSymbolType(TupLibraryObject::Sound);
    
    project->emitResponse(&response);
}

void TupProjectLoader::createScene(const QString &name, int scenePosition, TupProject *project)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProjectLoader::createScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupSceneResponse response(TupProjectRequest::Scene, TupProjectRequest::Add);

    response.setMode(TupProjectResponse::Do);
    response.setSceneIndex(scenePosition);
    response.setArg(name);
    
    project->emitResponse(&response);
}

void TupProjectLoader::createSymbol(TupLibraryObject::Type type, const QString &name, const QString &parent, const QByteArray &data, TupProject *project)
{
    TupLibraryResponse response(TupProjectRequest::Library, TupProjectRequest::Add);
  
    response.setMode(TupProjectResponse::Do); 
    response.setArg(name);
    response.setData(data);
    response.setSymbolType(type);
    response.setParent(parent);
    
    project->emitResponse(&response);
}

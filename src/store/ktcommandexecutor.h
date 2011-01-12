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

#ifndef KTCOMMANDEXECUTOR_H
#define KTCOMMANDEXECUTOR_H

#include <QObject>
#include "ktscene.h"

class KTProject;
class KTProjectRequest;
class KTFrameResponse;
class KTItemResponse;
class KTSceneResponse;
class KTLayerResponse;
class KTProjectResponse;
class KTLibraryResponse;

/**
 * @author David Cuadrado \<krawek@gmail.com\>
*/
class KTCommandExecutor : public QObject
{
    Q_OBJECT;
    public:
        KTCommandExecutor(KTProject *project);
        ~KTCommandExecutor();

        void getScenes(KTSceneResponse *response);
        bool createScene(KTSceneResponse *response);
        bool createLayer(KTLayerResponse *response);
        bool createFrame(KTFrameResponse *response );
        
        bool createItem(KTItemResponse *response);
        
        bool convertItem(KTItemResponse *response);
        bool transformItem(KTItemResponse *response);
        bool setPathItem(KTItemResponse *response);
        
        bool removeScene(KTSceneResponse *response);
        bool removeLayer(KTLayerResponse *response);
        bool removeFrame(KTFrameResponse *response);
        bool resetFrame(KTFrameResponse *response);
        
        bool removeItem(KTItemResponse *response);
        bool moveItem(KTItemResponse *response);
        
        bool groupItems(KTItemResponse *response);
        bool ungroupItems(KTItemResponse *response);
        
        bool moveScene(KTSceneResponse *response);
        bool moveLayer(KTLayerResponse *response);
        bool moveFrame(KTFrameResponse *response);
        bool exchangeFrame(KTFrameResponse *response);
        
        bool lockScene(KTSceneResponse *response);
        bool lockLayer(KTLayerResponse *response);
        bool lockFrame(KTFrameResponse *response);
        
        bool renameScene(KTSceneResponse *response);
        bool renameLayer(KTLayerResponse *response);
        bool renameFrame(KTFrameResponse *response);
        
        void selectScene(KTSceneResponse *response);
        bool selectLayer(KTLayerResponse *response);
        bool selectFrame(KTFrameResponse *response);
        
        bool setFrameVisibility(KTFrameResponse *response);
        bool setLayerVisibility(KTLayerResponse *response);
        bool setSceneVisibility(KTSceneResponse *response);
        
        bool createSymbol(KTLibraryResponse *response);
        bool removeSymbol(KTLibraryResponse *response);
        bool addSymbolToProject(KTLibraryResponse *response);
        bool removeSymbolFromProject(KTLibraryResponse *response);
        
        bool setTween(bool update, KTItemResponse *response);
        
        bool expandFrame(KTFrameResponse *response);
        bool pasteFrame( KTFrameResponse *response);
        
    signals:
        void responsed(KTProjectResponse *response);
        
    private:
        KTProject *m_project;
};

#endif

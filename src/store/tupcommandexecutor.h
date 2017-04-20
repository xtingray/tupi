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

#ifndef TUPCOMMANDEXECUTOR_H
#define TUPCOMMANDEXECUTOR_H

#include "tglobal.h"
#include "tupscene.h"

#include <QObject>

class TupProject;
class TupProjectRequest;
class TupFrameResponse;
class TupItemResponse;
class TupSceneResponse;
class TupLayerResponse;
class TupProjectResponse;
class TupLibraryResponse;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupCommandExecutor : public QObject
{
    Q_OBJECT

    public:
        TupCommandExecutor(TupProject *project);
        ~TupCommandExecutor();

        void getScenes(TupSceneResponse *response);
        bool createScene(TupSceneResponse *response);
        bool createLayer(TupLayerResponse *response);
        bool addLipSync(TupLayerResponse *response);
        bool createFrame(TupFrameResponse *response);
        // bool restoreFrameSelection(TupFrameResponse *response);
        
        bool createItem(TupItemResponse *response);

        bool updateLipSync(TupLayerResponse *response);
        
        bool convertItem(TupItemResponse *response);
        bool transformItem(TupItemResponse *response);
        bool setPathItem(TupItemResponse *response);

        bool setBrush(TupItemResponse *response);
        bool setPen(TupItemResponse *response);
        
        bool removeScene(TupSceneResponse *response);
        bool removeLayer(TupLayerResponse *response);
        bool removeLipSync(TupLayerResponse *response);
        bool removeFrame(TupFrameResponse *response);
        bool removeFrameSelection(TupFrameResponse *response);

        bool resetFrame(TupFrameResponse *response);
        
        bool removeItem(TupItemResponse *response);
        bool moveItem(TupItemResponse *response);
        
        bool groupItems(TupItemResponse *response);
        bool ungroupItems(TupItemResponse *response);
        
        bool moveScene(TupSceneResponse *response);
        bool moveLayer(TupLayerResponse *response);
        bool moveFrame(TupFrameResponse *response);
        bool exchangeFrame(TupFrameResponse *response);
        
        bool lockScene(TupSceneResponse *response);
        bool lockLayer(TupLayerResponse *response);
        // bool lockFrame(TupFrameResponse *response);
        
        bool renameScene(TupSceneResponse *response);
        bool renameLayer(TupLayerResponse *response);
        bool renameFrame(TupFrameResponse *response);
        
        void selectScene(TupSceneResponse *response);
        bool selectLayer(TupLayerResponse *response);
        bool selectFrame(TupFrameResponse *response);
        
        bool setFrameVisibility(TupFrameResponse *response);
        bool setLayerVisibility(TupLayerResponse *response);
        bool setSceneVisibility(TupSceneResponse *response);
        
        bool createSymbol(TupLibraryResponse *response);
        bool removeSymbol(TupLibraryResponse *response);
        bool insertSymbolIntoFrame(TupLibraryResponse *response);
        bool removeSymbolFromFrame(TupLibraryResponse *response);
        
        bool setTween(TupItemResponse *response);
        bool extendFrame(TupFrameResponse *response);
        bool copyFrameSelection(TupFrameResponse *response);
        // bool pasteFrame( TupFrameResponse *response);
        bool pasteFrameSelection(TupFrameResponse *response);
        bool resetScene(TupSceneResponse *response);

        void setBgColor(TupSceneResponse *response);
        
    signals:
        void responsed(TupProjectResponse *response);
        
    private:
        TupProject *m_project;
        QList<QString> selectionFramesCopy;
        QStringList copyParams;
};

#endif

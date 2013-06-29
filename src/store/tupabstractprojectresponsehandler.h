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

#ifndef TUPABSTRACTPROJECTRESPONSEHANDLER_H
#define TUPABSTRACTPROJECTRESPONSEHANDLER_H

#include "tupglobal_store.h"
#include <QObject>

class TupPaintAreaEvent;
class TupProjectResponse;
class TupItemResponse;
class TupFrameResponse;
class TupSceneResponse;
class TupLayerResponse;
class TupLibraryResponse;

/**
 * @author David Cuadrado
**/

class STORE_EXPORT TupAbstractProjectResponseHandler
{
    public:
        TupAbstractProjectResponseHandler();
        virtual ~TupAbstractProjectResponseHandler();
        
        virtual bool handleResponse(TupProjectResponse *response);
        
    protected:
        virtual void itemResponse(TupItemResponse *itemResponse) = 0;
        virtual void frameResponse(TupFrameResponse *frameResponse) = 0;
        virtual void layerResponse(TupLayerResponse *layerResponse) = 0;
        virtual void sceneResponse(TupSceneResponse *sceneResponse) = 0;
        virtual void projectResponse(TupProjectResponse *projectResponse) = 0;
        virtual void libraryResponse(TupLibraryResponse *libraryResponse) = 0;
};

#endif

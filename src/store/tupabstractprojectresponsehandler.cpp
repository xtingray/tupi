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

#include "tupabstractprojectresponsehandler.h"
#include "tupprojectresponse.h"
#include "tuppaintareaevent.h"

TupAbstractProjectResponseHandler::TupAbstractProjectResponseHandler()
{
}

TupAbstractProjectResponseHandler::~TupAbstractProjectResponseHandler()
{
}

bool TupAbstractProjectResponseHandler::handleResponse(TupProjectResponse *response)
{
    switch (response->part()) {
            case TupProjectRequest::Item:
            {
                 itemResponse(static_cast<TupItemResponse *>(response));
            }
            break;
            case TupProjectRequest::Project:
            {
                 projectResponse(response);
            }
            break;
            case TupProjectRequest::Frame:
            {
                 frameResponse(static_cast<TupFrameResponse *>(response));
            }
            break;
            case TupProjectRequest::Layer:
            {
                 layerResponse(static_cast<TupLayerResponse *>(response));
            }
            break;
            case TupProjectRequest::Scene:
            {
                 sceneResponse(static_cast<TupSceneResponse *>(response));
            }
            break;
            case TupProjectRequest::Library:
            {
                 libraryResponse(static_cast<TupLibraryResponse *>(response));
            }
            break;
            default:
            {
                 #ifdef K_DEBUG
                     QString msg = "TupAbstractProjectResponseHandler::handleResponse() - Error: Unknown project response: " + QString::number(response->part());
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif

                 return false;
            }
            break;
    }
    
    return true;
}


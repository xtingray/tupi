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

#include "ktabstractprojectresponsehandler.h"
#include "ktprojectresponse.h"
#include "ktpaintareaevent.h"

#include "kdebug.h"

KTAbstractProjectResponseHandler::KTAbstractProjectResponseHandler()
{
}

KTAbstractProjectResponseHandler::~KTAbstractProjectResponseHandler()
{
}

bool KTAbstractProjectResponseHandler::handleResponse(KTProjectResponse *response)
{
    switch (response->part()) {
            case KTProjectRequest::Item:
            {
                 itemResponse(static_cast<KTItemResponse *>(response));
            }
            break;
            case KTProjectRequest::Project:
            {
                 projectResponse(response);
            }
            break;
            case KTProjectRequest::Frame:
            {
                 frameResponse(static_cast<KTFrameResponse *>(response));
            }
            break;
            case KTProjectRequest::Layer:
            {
                 layerResponse(static_cast<KTLayerResponse *>(response));
            }
            break;
            case KTProjectRequest::Scene:
            {
                 sceneResponse(static_cast<KTSceneResponse *>(response));
            }
            break;
            case KTProjectRequest::Library:
            {
                 libraryResponse(static_cast<KTLibraryResponse *>(response));
            }
            break;
            default:
            {
                 qWarning("Unknown project response!");
                 return false;
            }
            break;
    }
    
    return true;
}


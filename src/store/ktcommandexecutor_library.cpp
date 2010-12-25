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
#include "ktproject.h"
#include "ktprojectrequest.h"

#include "ktprojectresponse.h"
#include <QDebug>

#include "kdebug.h"

bool KTCommandExecutor::createSymbol(KTLibraryResponse *response)
{
    kFatal() << "KTCommandExecutor::createSymbol() - Just tracing!";
    if (m_project->createSymbol(response->symbolType(), response->arg().toString(), response->data(), response->parent())) {
        emit responsed(response);

        return true;
    } 
    
    return false;
}


bool KTCommandExecutor::removeSymbol(KTLibraryResponse *response)
{
    if ((response->sceneIndex() > -1) && (response->layerIndex() > -1) && (response->frameIndex() > -1)) {

        if (m_project->removeSymbol(response->arg().toString(), response->symbolType(), response->sceneIndex(), response->layerIndex(), response->frameIndex())) {
            emit responsed(response);
            return true;
        } 
    } else {
        if (m_project->removeSymbol(response->arg().toString())) {
            emit responsed(response);
            return true;
        }
    }
    
    return false;
}

bool KTCommandExecutor::addSymbolToProject(KTLibraryResponse *response)
{
    kFatal() << "KTCommandExecutor::addSymbolToProject() - Just tracing!";
    if (m_project->scenesTotal() > 0) {
        kFatal() << "KTCommandExecutor::addSymbolToProject() - Flag 1";
        if (m_project->addSymbolToProject(response->arg().toString(), response->sceneIndex(), response->layerIndex(), response->frameIndex())) {
            emit responsed(response);
            return true;
        } 
    } 

    return false;
}

bool KTCommandExecutor::removeSymbolFromProject(KTLibraryResponse *response)
{
    if (m_project->scenesTotal() > 0) {
        if (m_project->removeSymbolFromProject(response->arg().toString(), response->symbolType())) {
            emit responsed(response);
            return true;
        }
    }

    return false;
}

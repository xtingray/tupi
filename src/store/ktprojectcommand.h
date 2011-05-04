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

#ifndef KTPROJECTCOMMAND_H
#define KTPROJECTCOMMAND_H

#include <QUndoCommand>
#include <QVariant>

#include "ktglobal_store.h"

class KTProject;
class KTProjectRequest;
class KTProjectRequest;
class KTProjectRequest;
class KTProjectRequest;
class KTProjectRequest;
class KTPaintAreaEvent;
class KTCommandExecutor;
class KTProjectResponse;

/**
 * @author David Cuadrado \<krawek@gmail.com\>
*/
class STORE_EXPORT KTProjectCommand : public QUndoCommand
{
    public:
        KTProjectCommand(KTCommandExecutor *executor, const KTProjectRequest *event);
        KTProjectCommand(KTCommandExecutor *executor, KTProjectResponse *response);
        ~KTProjectCommand();
        
        virtual void redo();
        virtual void undo();
        
        void frameCommand();
        void layerCommand();
        void sceneCommand();
        void itemCommand();
        
        void libraryCommand();
        void paintAreaCommand();
        
    private:
        QString actionString(int action);
        void initText();
        
    private:
        struct Private;
        Private *const k;
};

#endif

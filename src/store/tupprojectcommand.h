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

#ifndef TUPPROJECTCOMMAND_H
#define TUPPROJECTCOMMAND_H

#include "tglobal.h"

#include <QUndoCommand>
#include <QVariant>

class TupProject;
class TupProjectRequest;
class TupPaintAreaEvent;
class TupCommandExecutor;
class TupProjectResponse;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupProjectCommand : public QUndoCommand
{
    public:
        TupProjectCommand(TupCommandExecutor *executor, const TupProjectRequest *event);
        TupProjectCommand(TupCommandExecutor *executor, TupProjectResponse *response);
        ~TupProjectCommand();
        
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

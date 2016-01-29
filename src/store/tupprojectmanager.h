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

#ifndef TUPPROJECTMANAGER_H
#define TUPPROJECTMANAGER_H

#include "tglobal.h"

#include <QUndoStack>
#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QSize>

class TupProject;
class TupProjectRequest;
class TupProjectCommand;
class TupProjectManagerParams;
class TupAbstractProjectHandler;
class QUndoStack;
class TupCommandExecutor;
class TupProjectResponse;

/**
 * Events handler class for the project
 * @author David Cuadrado
*/

class TUPI_EXPORT TupProjectManager : public QObject
{
    Q_OBJECT

    public:
        TupProjectManager(QObject *parent = 0);
        virtual ~TupProjectManager();

        void setParams(TupProjectManagerParams *params);
        TupProjectManagerParams *params() const;

        virtual void setupNewProject();
        virtual void closeProject();

        bool isOpen() const;
        bool isModified() const;
        TupProject *project();
        void setHandler(TupAbstractProjectHandler *handler, bool isNetworked);
        TupAbstractProjectHandler *handler() const;

        void createCommand(TupProjectCommand *command);
        void clearUndoStack();

        virtual bool saveProject(const QString &fileName);
        virtual bool loadProject(const QString &fileName);

        bool isValid() const;
        void undoModified();
        void setOpen(bool isOpen);

        void updateProjectDimension(const QSize size);
   
    protected slots:
        virtual void handleProjectRequest(const TupProjectRequest *request);
        virtual void handleLocalRequest(const TupProjectRequest *request);
        virtual void createCommand(const TupProjectRequest *request, bool addToStack);

    private slots:
        void emitResponse(TupProjectResponse *response);
        void undo();
        void redo();

    signals:
        void responsed(TupProjectResponse *reponse);
        void requestOpenProject(const QString& filename);
        // void projectHasChanged(); 
        //void updateAnimationModule(TupProject *project, int sceneIndex, int layerIndex, int frameIndex);

    private:
        bool cleanProjectPath(QString &projectPath);
        class Private;
        Private *const k;
};

#endif

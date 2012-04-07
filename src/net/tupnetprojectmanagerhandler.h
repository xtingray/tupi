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

#ifndef TupNETPROJECTMANAGER_H
#define TupNETPROJECTMANAGER_H

#include "tupabstractprojectmanagerhandler.h"

#include <QDomDocument>
#include <QTabWidget>

class TupChat;
class TupProjectCommand;
class TupNetSocket;
class TupNetProjectManagerParams;

/**
 * @author David Cuadrado
*/

class TupNetProjectManagerHandler : public TupAbstractProjectHandler
{
    Q_OBJECT

    public:
        TupNetProjectManagerHandler(QObject *parent = 0);
        ~TupNetProjectManagerHandler();

        virtual void initialize(TupProjectManagerParams *params);
        virtual bool setupNewProject(TupProjectManagerParams *params);
        virtual bool closeProject();

        virtual void handleProjectRequest(const TupProjectRequest* event);
        virtual bool commandExecuted(TupProjectResponse *response);
        virtual bool saveProject(const QString &fileName, TupProject *project);
        virtual bool loadProject(const QString &fileName, TupProject *project);

        void handlePackage(const QString &root, const QString &package);
        virtual bool isValid() const;
        void sendPackage(const QDomDocument &doc);

        QTabWidget *communicationWidget();
        void closeConnection();

        void sendVideoRequest(const QString &title, const QString &topics, const QString &description, int fps, const QList<int> sceneIndexes);
        void sendStoryboardRequest(const QString &title, const QString &topics, const QString &description, const QList<int> sceneIndexes);

    signals:
        void savingSuccessful();
        void postOperationDone();
        void connectionHasBeenLost();

    private:
        void loadProjectFromServer(const QString &projectID, const QString &owner);
        void emitRequest(TupProjectRequest *request, bool toStack);
        void setProject(TupProject *project);

    private slots:
        void sendChatMessage(const QString &message);
        void connectionLost();
        void sendExportImageRequestToServer(int frameIndex, int sceneIndex, const QString &title, const QString &topics, const QString &description);

    private:
        struct Private;
        Private *const k;
};

#endif

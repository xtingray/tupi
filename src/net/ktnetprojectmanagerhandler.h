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

#ifndef KTNETPROJECTMANAGER_H
#define KTNETPROJECTMANAGER_H

#include "ktabstractprojectmanagerhandler.h"

#include <QDomDocument>
#include <QTabWidget>

class KTChat;
class KTProjectCommand;
class KTNetSocket;
class KTNetProjectManagerParams;

/**
 * @author David Cuadrado
*/

class KTNetProjectManagerHandler : public KTAbstractProjectHandler
{
    Q_OBJECT

    public:
        KTNetProjectManagerHandler(QObject *parent = 0);
        ~KTNetProjectManagerHandler();

        virtual void initialize(KTProjectManagerParams *params);
        virtual bool setupNewProject(KTProjectManagerParams *params);
        virtual bool closeProject();

        virtual void handleProjectRequest(const KTProjectRequest* event);
        virtual bool commandExecuted(KTProjectResponse *response);
        virtual bool saveProject(const QString &fileName, KTProject *project);
        virtual bool loadProject(const QString &fileName, KTProject *project);

        void handlePackage(const QString &root, const QString &package);
        virtual bool isValid() const;
        void sendPackage(const QDomDocument &doc);

        QTabWidget *communicationWidget();
        void closeConnection();

    signals:
        void savingSuccessful();
        void connectionHasBeenLost();

    private:
        bool loadProjectFromServer(const QString &name);
        void emitRequest(KTProjectRequest *request, bool toStack);
        void setProject(KTProject *project);

    private slots:
        void sendChatMessage(const QString & message);
        void connectionLost();

    private:
        struct Private;
        Private *const k;
};

#endif

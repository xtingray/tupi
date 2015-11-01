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

#ifndef TUPABSTRACTPROJECTHANDLER_H
#define TUPABSTRACTPROJECTHANDLER_H

#include "tglobal.h"

#include <QObject>
#include <QStringList>

class TupProjectResponse;
class TupProjectRequest;
class TupProjectManagerParams;
class TupProjectCommand;
class TupProject;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupAbstractProjectHandler : public QObject
{
    Q_OBJECT

    public:
        TupAbstractProjectHandler(QObject *parent = 0);
        virtual ~TupAbstractProjectHandler();

        virtual void initialize(TupProjectManagerParams *params);
        virtual bool setupNewProject(TupProjectManagerParams *params);
        virtual bool closeProject();
        virtual void handleProjectRequest(const TupProjectRequest *request) = 0;
        virtual bool commandExecuted(TupProjectResponse *response);
        virtual bool saveProject(const QString &fileName, TupProject *project) = 0;
        virtual bool loadProject(const QString &fileName, TupProject *project) = 0;
        virtual void setProject(TupProject *project) = 0;
        virtual bool isValid() const;

    signals:
        void sendCommand(const TupProjectRequest *request, bool addToStack);
        void sendLocalCommand(const TupProjectRequest *request);
        void openNewArea(const QString &name);
        void openNewArea(const QString &name, const QStringList &users);
        void authenticationSuccessful();
        void updateUsersList(const QString &login, int state);
};

#endif

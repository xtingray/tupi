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

#ifndef TUPNETFILEMANAGER_H
#define TUPNETFILEMANAGER_H 

#include "tglobal.h"
#include "tupfilemanager.h"
#include "tupproject.h"
#include "tupnetprojectmanagerparams.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

// class TupNetProjectManagerParams;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupNetFileManager : public TupFileManager 
{
    public:

        TupNetFileManager();
        TupNetFileManager(const QString &server, int port);
        ~TupNetFileManager();
        
        // virtual bool save(const QString &filename, const TupProject *project);
        virtual bool save(const QString &filename, TupProject *project);
        virtual bool load(const QString &filename, TupProject *project);
        
        TupNetProjectManagerParams *params(const QString &filename);
        
    private:
        QString m_server;
        int m_port;
};

#endif
